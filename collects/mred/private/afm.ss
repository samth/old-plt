(module afm mzscheme
  (require (lib "file.ss")
	   (lib "list.ss"))

  (provide (protect afm-draw-text
		    afm-get-text-extent
		    afm-expand-name
		    afm-glyph-exists?
		    current-ps-afm-file-paths
		    current-ps-cmap-file-paths))

  (define orig-err (current-error-port))
  (define (report-exn exn)
    (fprintf orig-err "PostScript/AFM error: ~a~n"
	     (if (exn? exn)
		 (exn-message exn)
		 exn)))

  (define bytes->number
    (case-lambda
     [(s) (bytes->number s 10)]
     [(s r) (string->number (bytes->string/latin-1 s) r)]))

  ;; ------------------------------------------------------------
  ;; Paths

  (define (check-paths who)
    (lambda (l)
      (unless (and (list? l)
		   (andmap path-string? l))
	(raise-type-error who "list of paths/strings" l))
      (apply list-immutable
	     (map (lambda (i)
		    (if (string? i) (string->path i) i))
		  l))))

  ;; Paths that users can set
  ;;  Location of .afm files:
  (define afm-home
    (with-handlers ([exn:fail:filesystem? (lambda (x) (current-directory))])
      (collection-path "afm")))
  (define current-ps-afm-file-paths
    (make-parameter (path-list-string->path-list 
		     (or (getenv "PLTAFMPATHS") "")
		     (list afm-home))
		    (check-paths 'current-post-script-afm-file-paths)))
  ;;  Location of CMap files (for CID fonts)
  (define current-ps-cmap-file-paths
    (make-parameter (path-list-string->path-list 
		     (or (getenv "PLTCMAPPATHS") "")
		     (list (build-path afm-home "CMap")))
		    (check-paths 'current-post-script-cmap-file-paths)))

  (define (find-path l f)
    (or (ormap (lambda (d)
		 (let ([f (build-path d f)])
		   (and (file-exists? f)
			f)))
	       l)
	(error 'find-path "file not found: ~e in dirs: ~e" f l)))

  ;; ------------------------------------------------------------
  ;; Adobe <-> Unicode

  ;; Table mapping PS char names (as bytes) to Unicode integers.
  ;;  It's loaded on demand.
  (define adobe-name-to-code-point #f)
  ;; In fact, we load a small table at first, and
  ;;  only load the full PS char name table if needed;
  ;;  the following flag indicated whether the full
  ;;  table has been loaded.
  (define got-long-name-list? #f)

  ;; Reads the Adobe char name -> Unicode table
  (define (read-names! gl.txt long?)
    (let ([ht (make-hash-table 'equal)])
      (with-handlers ([exn:fail? report-exn])
	(call-with-input-file* 
	 (find-path (current-ps-afm-file-paths) gl.txt)
	 (lambda (i)
	   (let loop ()
	     (let ([l (read-bytes-line i)])
	       (unless (eof-object? l)
		 (let ([m (regexp-match #rx#"^([a-zA-Z0-9_-]+);([0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])$"
					l)])
		   (when m
		     (hash-table-put! ht
				      (cadr m)
				      (bytes->number (caddr m) 16))))
		 (loop)))))))
      (set! adobe-name-to-code-point ht)
      (set! got-long-name-list? long?)))

  ;; Maps Adbobe char name to Unicode, loading the table as necesary
  (define (find-unicode name)
    (unless adobe-name-to-code-point
      (read-names! "glyphshortlist.txt" #f))
    (hash-table-get adobe-name-to-code-point
		    name
		    (lambda ()
		      (if got-long-name-list?
			  #f
			  (begin
			    (read-names! "glyphlist.txt" #t)
			    (find-unicode name))))))

  
  ;; ------------------------------------------------------------
  ;; CMap

  ;; read-cmap : path hash-table-or-false -> hash-table[int->int]
  ;;  A CMap file maps from one character encoding (e.g., Adobe-CNS1-0)
  ;;  to another (e.g., UTF-16). We'll always have to read one
  ;;  UTF-32 -> Adobe-CNS table (invert it), and then use that
  ;;  when reading any other XXX -> Adobe-CNS to generate an 
  ;;  XXX -> UTF-32 table.
  (define (read-cmap file to-unicode)
    (let* ([ht (make-hash-table 'equal)]
	   [put! (if to-unicode
		     (lambda (c cns)
		       (hash-table-put! ht 
					c
					(hash-table-get to-unicode cns
							(lambda () #f))))
		     (lambda (c cns)
		       (hash-table-put! ht cns c)))])
      (with-handlers ([exn:fail? report-exn])
	(call-with-input-file* 
	 file
	 (lambda (i)
	   (let loop ()
	     (let ([l (read-bytes-line i)])
	       (cond
		[(eof-object? l) (void)]
		[(regexp-match #rx#"begincidchar" l)
		 (let char-loop ()
		   (let ([l (read-bytes-line i)])
		     (cond
		      [(regexp-match #rx#"endcidchar" l)
		       (loop)]
		      [(regexp-match #rx#"^<([0-9A-Fa-f]*)> *([0-9]+)$" l)
		       => (lambda (m)
			    (put! (bytes->number (cadr m) 16)
				  (bytes->number (caddr m)))
			    (char-loop))]
		      [else
		       ;; parse error
		       (char-loop)])))]
		[(regexp-match #rx#"begincidrange" l)
		 (let range-loop ()
		   (let ([l (read-bytes-line i)])
		     (cond
		      [(regexp-match #rx#"endcidrange" l)
		       (loop)]
		      [(regexp-match #rx#"^<([0-9A-Fa-f]+)> <([0-9A-Fa-f]+)> *([0-9]+)$" l)
		       => (lambda (m)
			    (let* ([end (bytes->number (caddr m) 16)])
			      (let loop ([start (bytes->number (cadr m) 16)]
					 [v (bytes->number (cadddr m))])
				(put! start v)
				(unless (= start end)
				  (loop (add1 start) (add1 v)))))
			    (range-loop))]
		      [else
		       ;; parse error
		       (range-loop)])))]
		[else (loop)]))))))
      ht))

  (define cns->unicode-table #f)
  (define (read-cns->unicode!)
    (set! cns->unicode-table
	  (read-cmap
	   (find-path (current-ps-cmap-file-paths) "UniCNS-UTF32-H")
	   #f)))
  
  (define cmap-table (make-hash-table 'equal))
  (define (get-cmap name)
    (unless cns->unicode-table
      (read-cns->unicode!))
    (hash-table-get
     cmap-table
     name
     (lambda ()
       (let ([t (read-cmap
		 (find-path (current-ps-cmap-file-paths) (bytes->path name))
		 cns->unicode-table)])
	 (hash-table-put! cmap-table name t)
	 t))))

  ;; ----------------------------------------
  ;; AFM

  ;; An achar is either
  ;;  - (cons num-or-bytes num)
  ;;  - (list num-or-bytes num ligature-assoc kern-assoc)

  (define make-achar
    (case-lambda
     [(enc width ligatures)
      (if (null? ligatures)
	  (cons enc width)
	  (list enc width ligatures null))]
     [(enc width) (cons enc width)]))
  (define achar-enc car)   ; Number (unicode or CID) or bytes (Adobe char name)
  (define achar-width ; Integer, 0 to 1000
    (lambda (x)
      (if (pair? (cdr x))
	  (cadr x)
	  (cdr x))))
  (define achar-ligatures
    (lambda (x)
      (if (pair? (cdr x))
	  (caddr x)
	  null)))
  (define achar-kerns
    (lambda (x)
      (if (pair? (cdr x))
	  (cadddr x)
	  null)))
  (define (strip-ligatures-and-kerning achar)
    (make-achar (achar-enc achar) (achar-width achar)))
  (define (achar-add-kern! achar v amt)
    (when (not (pair? (cdr achar)))
      (set-cdr! achar (list (cdr achar) null null)))
    (set-car! (cdddr achar) (cons (cons v amt) (cadddr achar))))

  (define-struct font (descent ascent achars is-cid? char-set-name))

  (define re:hex #rx#"^<[0-9a-fA-F]>$")

  (define (parse-afm file)
    (let ([descender #f]
	  [bbox-down #f]
	  [cap-height #f]
	  [achars (make-hash-table 'equal)]
	  [kern-pairs null]
	  [char-set #f]
	  [char-set-name #f]
	  [is-cid? #f])
      (parameterize ([read-case-sensitive #f])
	(call-with-input-file*
	 file
	 (lambda (i)
	   (let loop ()
	     (let ([n (read i)])
	       (unless (eof-object? n)
		 (case n
		   [(descender) (set! descender (read i))]
		   [(fontbbox) (let ([l (read i)]
				     [t (read i)]
				     [r (read i)]
				     [b (read i)])
				 (set! bbox-down t))]
		   [(capheight) (set! cap-height (read i))]
		   [(characterset) (let ([m (regexp-match #rx#"[a-zA-Z_0-9-]+" (read-bytes-line i))])
				     (when m
				       (set! char-set-name (car m))
				       (set! char-set (get-cmap char-set-name))))]
		   [(iscidfont) (set! is-cid?
				      (regexp-match #rx#"true" (read-bytes-line i)))]
		   [(startcharmetrics)
		    (let ([cnt (read i)])
		      (let loop ()
			(let ([n (read i)])
			  (when (or (eq? n 'c)
				    (eq? n 'ch))
			    (let ([v (read i)]
				  [rest (read-bytes-line i)])
			      (let ([nm (regexp-match #rx#"; *N +([a-zA-Z0-9]+) *;" rest)]
				    [wm (regexp-match #rx#"; *W.?X +([0-9]+) *;" rest)])
				(when (or (and (eq? n 'c)
					       (integer? v))
					  (and (eq? n 'ch)
					       (symbol? v)
					       (regexp-match #rx#"^<[0-9a-fA-F]>$" (symbol->string v))))
				  (let ([name (or (and nm 
						       (if is-cid?
							   (bytes->number (cadr nm))
							   (cadr nm)))
						  0)])
				    (hash-table-put!
				     achars
				     (if (and char-set is-cid?)
					 (hash-table-get char-set name (lambda () 0))
					 (find-unicode name))
				     (make-achar
				      (let ([v (if (eq? n 'c)
						   v
						   (let ([s (symbol->string v)])
						     (string->number (substring s 1 (sub1 (string-length s))) 16)))])
					(if (= v -1)
					    name
					    v))
				      (or (and wm (bytes->number (cadr wm))) 500)
				      (extract-ligatures rest)))))))
			    (loop)))))]
		   [(startkernpairs)
		    (let ([cnt (read i)])
		      (let loop ()
			(let ([kp (read i)])
			  (when (or (eq? kp 'kp)
				    (eq? kp 'kpx))
			    (let ([v1 (parameterize ([read-case-sensitive #t])
					(read i))]
				  [v2 (parameterize ([read-case-sensitive #t])
					(read i))]
				  [amt (read i)])
			      (read-bytes-line i)
			      (let ([v1 (find-unicode (string->bytes/utf-8 (symbol->string v1)))]
				    [v2 (find-unicode (string->bytes/utf-8 (symbol->string v2)))])
				(set! kern-pairs (cons (list v1 v2 amt) kern-pairs))))
			    (loop)))))]
		   [else (read-bytes-line i)])
		 (loop)))))))
      (for-each (lambda (kp)
		  (let ([c1 (car kp)]
			[c2 (cadr kp)]
			[amt (caddr kp)])
		    (let ([achar (hash-table-get achars c1 (lambda () (make-achar 0 0)))])
		      (achar-add-kern! achar c2 amt))))
		kern-pairs)
      (make-font (- (or descender bbox-down 0))
		 (or cap-height 1000)
		 achars
		 (and char-set #t)
		 char-set-name)))

  (define (extract-ligatures rest)
    (let ([m (regexp-match #rx#"; *L +([a-zA-Z0-9-]+) +([a-zA-Z0-9-]+)(.*)$" rest)])
      (if m
	  (let ([next (cadr m)]
		[ligature (caddr m)]
		[rest (cadddr m)])
	    (cons (cons (find-unicode next)
			(find-unicode ligature))
		  (extract-ligatures rest)))
	  null)))

  (define fonts (make-hash-table 'equal))

  (define (get-font name)
    (hash-table-get fonts name
		    (lambda ()
		      (hash-table-put! fonts
				       name
				       (with-handlers ([void (lambda (exn)
							       (report-exn exn)
							       #f)])
					 (parse-afm 
					  (find-path (current-ps-afm-file-paths)
						     (format "~a.afm" name)))))
		      (get-font name))))

  ;; ----------------------------------------

  (define font-list #f)
  (define (get-all-fonts)
    (or font-list
	(begin
	  (set! font-list
		(apply append (map (lambda (dir)
				     (map (lambda (s)
					    (path->string (path-replace-suffix s #"")))
					  (filter (lambda (s)
						    (regexp-match #rx"[.][aA][fF][mM]$" (path->string s)))
						  (directory-list dir))))
				   (current-ps-afm-file-paths))))
	  font-list)))

  ;; ----------------------------------------

  (define symbol-map
    #(0 0 0 0 0 0 0 0
	0 0 0 0 0 0 0 0
	0 0 0 0 0 0 0 0
	0 0 0 0 0 0 0 0
	0 0 8704 0 8707 0 0 8717
	0 0 8727 0 0 8722 0 0
	0 0 0 0 0 0 0 0
	0 0 0 0 0 0 0 0
	8773 913 914 935 916 917 934 915
	919 921 977 922 923 924 925 927
	928 920 929 931 932 933 962 937
	926 936 918 0 8756 0 8869 0
	0 945 946 967 948 949 966 947
	951 953 981 954 955 956 957 959
	960 952 961 963 964 965 982 969
	958 968 950 0 0 0 8764 0
	0 0 0 0 0 0 0 0
	0 0 0 0 0 0 0 0
	0 0 0 0 0 0 0 0
	0 0 0 0 0 0 0 0
	0 978 8242 8804 8260 8734 402 9827
	9830 9829 9824 8596 8592 8593 8594 8595
	0 177 8243 8805 215 8733 8706 8729
	247 8800 8801 8776 8230 9168 8212 8629
	8501 8465 8476 8472 8855 8853 8709 8745
	8746 8835 8839 8836 8834 8838 8712 8713
	8736 8711 174 169 8482 8719 8730 8901
	172 8743 8744 8660 8656 8657 8658 8659
	9674 9001 174 169 8482 8721 9115 9116
	9117 9121 9122 9123 9127 9128 9129 9130
	8364 9002 8747 8992 9134 8993 9118 9119
	9120 9124 9125 9126 9131 9132 9133 0))

  (define (map-symbols sym-map? l)
    (if sym-map?
	(map (lambda (c)
	       (let ([v (char->integer c)])
		 (if (<= 0 v 255)
		     (integer->char (vector-ref symbol-map v))
		     c)))
	     l)
	l))

  ;; ----------------------------------------
  ;; Draw/measure text

  (define (afm-get-text-extent font-name size string kern? sym-map?)
    (let* ([font (or (get-font font-name)
		     (make-font 0 1000 #hash() #f #f))]
	   [scale (/ size 1000.0)]
	   [descent (* scale (font-descent font))])
      (values (* scale
		 (let loop ([cl (map-symbols sym-map? (string->list string))][width 0])
		   (cond
		    [(empty? cl) width]
		    [else (let ([achar (hash-table-get 
					(font-achars font)
					(char->integer (car cl))
					(lambda ()
					  (find-substitute 
					   (car cl)
					   (lambda (name font achar)
					     (strip-ligatures-and-kerning achar))
					   (lambda ()
					     (make-achar 0 500)))))])
			    (cond
			     [(and kern?
				   (pair? (cdr cl))
				   (assoc (char->integer (cadr cl))
					  (achar-ligatures achar)))
			      => (lambda (p)
				   ;; Replace the first two characters with a
				   ;; ligature and try again
				   (loop (cons (integer->char (cdr p)) (cddr cl)) 
					 width))]
			     [(and kern?
				   (pair? (cdr cl))
				   (assoc (char->integer (cadr cl))
					  (achar-kerns achar)))
			      => (lambda (p)
				   ;; Reduce width by kerning amount
				   (loop (cdr cl)
					 (+ width (achar-width achar) (cdr p))))]
			     [else
			      (loop (cdr cl) (+ width (achar-width achar)))]))])))
	      (+ size descent)
	      descent
	      (* scale (- 1000 (font-ascent font))))))

  (define (afm-draw-text font-name size string out kern? sym-map?)
    (let* ([l (map-symbols sym-map? (string->list string))]
	   [font (or (get-font font-name)
		     (make-font 0 0 #hash() #f #f))]
	   [show-simples (lambda (simples special-font-name special-font)
			   (unless (null? simples)
			     (when special-font
			       (fprintf out "gsave~n/~a findfont~n~a scalefont setfont~n"
					(afm-expand-name special-font-name)
					size))
			     (if (font-is-cid? (or special-font font))
				 (fprintf out "<~a> show\n"
					  (apply
					   string-append
					   (map (lambda (s)
						  (let ([s (format "000~x" s)])
						    (substring s (- (string-length s) 4))))
						(reverse simples))))
				 (let loop ([bytes (list->bytes (reverse simples))])
				   (unless (bytes=? #"" bytes)
				     (if ((bytes-length bytes) . > . 100)
					 ;; > 100, so break up the string to avoid over-long lines:
					 (begin
					   (loop (subbytes bytes 0 100))
					   (loop (subbytes bytes 100)))
					 ;; <= 100, so try to write it, but look for unsafe chars
					 (let ([m (regexp-match-positions #rx#"[^-a-z A-Z0-9!@#$%^&*_=+,.<>:;'\"`~|/?]+" 
									  bytes)])
					   (if m
					       ;; Encode the unsafe chars in hex
					       (begin
						 (loop (subbytes bytes 0 (caar m)))
						 (fprintf out "<~a> show\n" 
							  (apply string-append
								 (map
								  (lambda (c)
								    (let ([s (format "0~x" c)])
								      (substring s (- (string-length s) 2))))
								  (bytes->list (subbytes bytes (caar m) (cdar m))))))
						 (loop (subbytes bytes (cdar m))))
					       ;; All safe - write directly
					       (fprintf out "(~a) show\n" bytes)))))))
			     (when special-font
			       (fprintf out "grestore~n"))))])
      (let loop ([l l][simples null][special-font-name #f][special-font #f])
	(cond
	 [(null? l)
	  (show-simples simples special-font-name special-font)]
	 [(hash-table-get (font-achars font) (char->integer (car l))
			  (lambda () #f))
	  => (lambda (achar)
	       (if (integer? (achar-enc achar))
		   ;; It's simple...
		   (if special-font
		       ;; Flush simples using special font
		       (begin
			 (show-simples simples special-font-name special-font)
			 (loop (cdr l) (list (achar-enc achar)) #f #f))
		       ;; Continue simple chain. Look ahead for ligatures and kerning:
		       (cond
			[(and kern?
			      (pair? (cdr l))
			      (assoc (char->integer (cadr l))
				     (achar-ligatures achar)))
			 => (lambda (p)
			      ;; Substitute ligature
			      (loop (cons (integer->char (cdr p)) (cddr l))
				    simples
				    #f
				    #f))]
			[(and kern?
			      (pair? (cdr l))
			      (assoc (char->integer (cadr l))
				     (achar-kerns achar)))
			 => (lambda (p)
			      ;; Output result so far
			      (show-simples (cons (achar-enc achar) simples)
					    special-font-name special-font)
			      ;; Kern
			      (fprintf out "~a 0 rmoveto\n" (* size (/ (cdr p) 1000.0)))
			      ;; Continue
			      (loop (cdr l) null #f #f))]
			[else
			 ;; Normal additional character
			 (loop (cdr l) 
			       (cons (achar-enc achar) simples)
			       #f
			       #f)]))
		   ;; Not simple... use glyphshow
		   (begin
		     (show-simples simples special-font-name special-font)
		     (fprintf out "/~a glyphshow\n" (achar-enc achar))
		     (loop (cdr l) null #f #f))))]
	 [else
	  (find-substitute
	   (car l)
	   (lambda (this-font-name this-font achar)
	     ;; Found a substitute font. (Don't try to kern, though.)
	     (if (and (equal? special-font-name this-font-name)
		      (integer? (achar-enc achar)))
		 ;; Continue special-font simple chain:
		 (loop (cdr l)
		       (cons (achar-enc achar) simples)
		       special-font-name
		       special-font)
		 ;; End current simples, etc.
		 (begin
		   (show-simples simples special-font-name special-font)
		   (if (integer? (achar-enc achar))
		       (loop (cdr l)
			     (list (achar-enc achar))
			     this-font-name
			     this-font)
		       (begin
			 ;; Not simple... use glyphshow
			 (fprintf out "gsave~n/~a findfont~n~a scalefont setfont~n"
				  (afm-expand-name this-font-name)
				  size)
			 (fprintf out "/~a glyphshow\n" (achar-enc achar))
			 (fprintf out "grestore~n")
			 (loop (cdr l) null #f #f))))))
	   (lambda ()
	     ;; No mapping for the character anywhere.
	     (show-simples simples special-font-name special-font)
	     ;; Future work: a box. For now, we just skip some space.
	     (fprintf out "~a 0 rmoveto\n" (/ size 2))
	     (loop (cdr l) null #f #f)))]))))

  ;; ----------------------------------------
  ;; Name expansion

  ;; Adds a char-set to the name, if necessary
  (define (afm-expand-name font-name)
    (let ([f (get-font font-name)])
      (if (and f (font-char-set-name f))
	  (string-append font-name "-" (bytes->string/latin-1 (font-char-set-name f)))
	  font-name)))

  ;; ----------------------------------------
  ;; Font substitution

  (define (find-substitute char find-k none-k)
    (let ([v (afm-glyph-exists? #f (char->integer char) #f)])
      (if v
	  (apply find-k v)
	  (none-k))))

  (define (afm-glyph-exists?* font-name char-val)
    (let ([f (get-font font-name)])
      (and f 
	   (let ([achar (hash-table-get (font-achars f) 
					char-val
					(lambda () #f))])
	     (and achar
		  (list font-name f achar))))))
  
  (define (afm-glyph-exists? font-name char-val sym-map?)
    (let ([char-val (if (and sym-map?
			     (<= 0 char-val 255))
			(vector-ref symbol-map char-val)
			char-val)])
      (or (and font-name
	       (afm-glyph-exists?* font-name char-val))
	  (ormap (lambda (fn)
		   (afm-glyph-exists?* fn char-val))
		 (get-all-fonts))))))


