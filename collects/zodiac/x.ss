; $Id: x.ss,v 1.63 2000/07/10 23:42:03 mflatt Exp $

(unit/sig zodiac:expander^
  (import
    zodiac:misc^ zodiac:sexp^
    zodiac:structures^
    (z : zodiac:reader-structs^)
    zodiac:scheme-core^
    zodiac:interface^)

  ; ----------------------------------------------------------------------

  (define-struct resolutions (name user?))
  (define-struct (micro-resolution struct:resolutions) (rewriter))
  (define-struct (macro-resolution struct:resolutions) (rewriter))

  ; ----------------------------------------------------------------------

  (define reference-namespace (make-namespace))
  (define-struct z:syntax (name))

  (define (make-mz-binding s)
    (make-z:syntax s))

  (define (syntax-symbol->id s)
    (parameterize ([current-namespace reference-namespace])
      (with-handlers ([void (lambda (x) (let ([v (make-mz-binding s)])
					  (global-defined-value s v)
					  v))])
	(global-defined-value s))))

  ; ----------------------------------------------------------------------

  (define-struct vocabulary-record (name namespace-based? this rest
					 symbol-x literal-x
					 list-x ilist-x
					 symbol-error literal-error
					 list-error ilist-error
					 on-demand subexpr-vocab))

  (define get-vocabulary-name vocabulary-record-name)

  (define (self-subexpr-vocab v)
    (set-vocabulary-record-subexpr-vocab! v v)
    v)

  (define (set-subexpr-vocab! v subexpr-v)
    (set-vocabulary-record-subexpr-vocab! v subexpr-v))

  (define create-vocabulary
    (opt-lambda (name (root #f)
		      (namespace-based? (if root
					    (vocabulary-record-namespace-based? root)
					    #t))
		      (symbol-error (if root
					(vocabulary-record-symbol-error root)
					"symbol invalid in this position"))
		      (literal-error (if root
					 (vocabulary-record-literal-error root)
					 "literal invalid in this position"))
		      (list-error (if root
				      (vocabulary-record-list-error root)
				      "list invalid in this position"))
		      (ilist-error (if root
				       (vocabulary-record-ilist-error root)
				       "improper-list syntax invalid in this position")))
      (let ((h (make-hash-table)))
	(self-subexpr-vocab
	 (make-vocabulary-record
	  name namespace-based? h root
	  #f #f #f #f
	  symbol-error literal-error list-error ilist-error
	  null #f)))))

  (define append-vocabulary
    (opt-lambda (new old (name #f))
      (let loop ((this new) (first? #t))
	(let ((name (if (and first? name) name
		      (vocabulary-record-name this))))
	  (self-subexpr-vocab
	   (make-vocabulary-record
	    name
	    (vocabulary-record-namespace-based? this)
	    (vocabulary-record-this this)
	    (if (vocabulary-record-rest this)
		(loop (vocabulary-record-rest this) #f)
		old)
	    (vocabulary-record-symbol-x this)
	    (vocabulary-record-literal-x this)
	    (vocabulary-record-list-x this)
	    (vocabulary-record-ilist-x this)
	    (vocabulary-record-symbol-error this)
	    (vocabulary-record-literal-error this)
	    (vocabulary-record-list-error this)
	    (vocabulary-record-ilist-error this)
	    (vocabulary-record-on-demand this)
	    #f))))))

  (define add-micro/macro-form
    (lambda (constructor stdname?)
      (lambda (name/s vocab rewriter)
	(let* ((v (vocabulary-record-this vocab))
	       (names (if (symbol? name/s) (list name/s) name/s))
	       (ids (if (vocabulary-record-namespace-based? vocab)
			(map (if stdname? 
				 syntax-symbol->id
				 make-mz-binding)
			     names)
			names))
	       (r (constructor rewriter)))
	  (set-resolutions-name! r name/s)
	  (map (lambda (n id)
		 (hash-table-put! v id r))
	       names ids)
;	  (unless stdname? -- see PR 1622
	  (when (and (not stdname?)
		  (vocabulary-record-namespace-based? vocab))
	    (for-each
	     (lambda (name id)
	       (global-defined-value name id))
	     names ids))))))

  (define vocab->list
    (lambda (vocab)
      (cons (vocabulary-record-name vocab)
	(hash-table-map cons (vocabulary-record-this vocab)))))

  (define add-micro-form
    (add-micro/macro-form (lambda (r)
			    (make-micro-resolution 'dummy #f r))
			  #t))

  (define add-system-macro-form
    (add-micro/macro-form (lambda (r)
			    (make-macro-resolution 'dummy #f r))
			  #t))

  (define add-user-macro-form
    (add-micro/macro-form (lambda (r)
			    (make-macro-resolution 'dummy #t r))
			  #f))

  (define add-macro-form add-system-macro-form)

  (define list-micro-kwd
    (string->uninterned-symbol "list-expander"))
  (define ilist-micro-kwd
    (string->uninterned-symbol "ilist-expander"))
  (define sym-micro-kwd
    (string->uninterned-symbol "symbol-expander"))
  (define lit-micro-kwd
    (string->uninterned-symbol "literal-expander"))

  (define (add-list/sym/lit-micro setter! kwd)
    (lambda (vocab rewriter)
      (setter! vocab (make-micro-resolution kwd #f rewriter))))

  (define add-list-micro (add-list/sym/lit-micro 
			  set-vocabulary-record-list-x!
			  list-micro-kwd))
  (define add-ilist-micro (add-list/sym/lit-micro
			   set-vocabulary-record-ilist-x!
			   ilist-micro-kwd))
  (define add-sym-micro (add-list/sym/lit-micro 
			 set-vocabulary-record-symbol-x!
			 sym-micro-kwd))
  (define add-lit-micro (add-list/sym/lit-micro
			 set-vocabulary-record-literal-x!
			 lit-micro-kwd))
  
  (define (get-list/lit/sym getter)
    (letrec ([get (lambda (vocab)
		    (and vocab
			 (or (getter vocab)
			     (get (vocabulary-record-rest vocab)))))])
      get))

  (define get-list-micro (get-list/lit/sym vocabulary-record-list-x))
  (define get-ilist-micro (get-list/lit/sym vocabulary-record-ilist-x))
  (define get-sym-micro (get-list/lit/sym vocabulary-record-symbol-x))
  (define get-lit-micro (get-list/lit/sym vocabulary-record-literal-x))

  (define (add-on-demand-form kind name vocab micro)
    (set-vocabulary-record-on-demand!
     vocab
     (cons (list* name kind micro)
	   (vocabulary-record-on-demand vocab))))

  (define (find-on-demand-form name vocab)
    (let ([v (assq name (vocabulary-record-on-demand vocab))])
      (if v
	  (list (cadr v) (cddr v))
	  (let ([super (vocabulary-record-rest vocab)])
	    (and super (find-on-demand-form name super))))))

  ; ----------------------------------------------------------------------

  (define expand-expr
    (lambda (expr env attributes vocab)
      ; (printf "Expanding in ~s:~n" (get-vocabulary-name vocab))
      ;   (pretty-print (sexp->raw expr)) (newline)
      ; (printf "top-level-status: ~s~n" (get-top-level-status attributes))
      ; (printf "Expanding~n") (pretty-print expr) (newline)
      ; (printf "Expanding~n") (pretty-print (sexp->raw expr)) (newline)
      ; (printf "Expanding~n") (display expr) (newline) (newline)
      ; (printf "in ~s~n" (get-vocabulary-name vocab))
      ;	(printf "in attributes~n") (hash-table-map attributes cons)
      ; (printf "in~n") (print-env env)
      ; (newline)
      (cond
	((z:symbol? expr)
	  (let ((sym-expander (get-sym-micro vocab)))
	    (cond
	      ((micro-resolution? sym-expander)
		((micro-resolution-rewriter sym-expander)
		  expr env attributes (vocabulary-record-subexpr-vocab vocab)))
	      (sym-expander
		(internal-error expr "Invalid sym expander ~s" sym-expander))
	      (else
		(static-error
		  "symbol syntax" 'term:invalid-pos-symbol
		  expr
		  (vocabulary-record-symbol-error vocab))))))
	((or (z:scalar? expr)		; "literals" = scalars - symbols
	   (z:vector? expr))
	  (let ((lit-expander (get-lit-micro vocab)))
	    (cond
	      ((micro-resolution? lit-expander)
		((micro-resolution-rewriter lit-expander)
		  expr env attributes (vocabulary-record-subexpr-vocab vocab)))
	      (lit-expander
		(internal-error expr
		  "Invalid lit expander ~s" lit-expander))
	      (else
		(static-error
		  "literal syntax" 'term:invalid-pos-literal
		  expr
		  (vocabulary-record-literal-error vocab))))))
	((z:list? expr)
	  (let ((invoke-list-expander
		  (lambda ()
		    (let ((list-expander (get-list-micro vocab)))
		      (cond
			((micro-resolution? list-expander)
			  ((micro-resolution-rewriter list-expander)
			    expr env attributes (vocabulary-record-subexpr-vocab vocab)))
			(list-expander
			  (internal-error expr
			    "Invalid list expander ~s" list-expander))
			(else
			  (static-error
			    "list syntax" 'term:invalid-pos-list
			    expr
			    (vocabulary-record-list-error vocab)))))))
		 (contents (expose-list expr)))
	    (if (null? contents)
	      (invoke-list-expander)
	      (let ((app-pos (car contents)))
		(if (z:symbol? app-pos)
		  (let ((r (resolve app-pos env vocab)))
		    (cond
		      ((macro-resolution? r)
			(with-handlers ((exn:user?
					  (lambda (exn)
					    (static-error
					      "macro error"
					      'term:macro-error
					      expr
					      (exn-message exn)))))
			  (let* ((rewriter (macro-resolution-rewriter r))
				  (m (new-mark))
				  (rewritten (rewriter expr env))
				  (structurized (structurize-syntax
						  rewritten expr (list m)
						  #f
						  (make-origin 'macro
						    expr))))
			    (expand-expr structurized env
			      attributes vocab))))
		      ((micro-resolution? r)
			((micro-resolution-rewriter r)
			  expr env attributes (vocabulary-record-subexpr-vocab vocab)))
		      (else
			(invoke-list-expander))))
		  (invoke-list-expander))))))
	((z:improper-list? expr)
	  (let ((ilist-expander (get-ilist-micro vocab)))
	    (cond
	      ((micro-resolution? ilist-expander)
		((micro-resolution-rewriter ilist-expander)
		  expr env attributes (vocabulary-record-subexpr-vocab vocab)))
	      (ilist-expander
		(internal-error expr
		  "Invalid ilist expander ~s" ilist-expander))
	      (else
		(static-error
		  "improper list syntax" 'term:invalid-pos-ilist
		  expr
		  (vocabulary-record-ilist-error vocab))))))
	(else
	  (internal-error expr
	    "Invalid body: ~s" expr)))))

  (define m3-elaboration-evaluator #f)
  (define m3-macro-body-evaluator #f)

  (define expand
    (lambda (expr attr vocab elaboration-eval macro-body-eval)
      (fluid-let ((m3-elaboration-evaluator elaboration-eval)
		  (m3-macro-body-evaluator macro-body-eval))
	(expand-expr expr (make-new-environment) attr vocab))))

  (define expand-program
    (lambda (exprs attr vocab elaboration-eval macro-body-eval)
      (fluid-let ((m3-elaboration-evaluator elaboration-eval)
		  (m3-macro-body-evaluator macro-body-eval))
	(put-attribute attr 'top-levels (make-hash-table))
	(map (lambda (expr)
	       (expand-expr expr (make-new-environment) attr vocab))
	  exprs))))

  ; ----------------------------------------------------------------------

  (define make-attributes make-hash-table)
  (define put-attribute
    (lambda (table key value)
      (hash-table-put! table key value)
      table))
  (define get-attribute
    (opt-lambda (table key (failure-thunk (lambda () #f)))
      (hash-table-get table key failure-thunk)))

  ; ----------------------------------------------------------------------

  (define introduce-identifier
    (lambda (new-name old-id)
      (z:make-symbol (zodiac-origin old-id)
	(zodiac-start old-id) (zodiac-finish old-id)
	new-name new-name (z:symbol-marks old-id))))

  (define introduce-fresh-identifier
    (lambda (new-name source)
      (z:make-symbol (make-origin 'non-source 'never-mind)
	(zodiac-start source) (zodiac-finish source)
	new-name new-name '())))
  
  (define introduce-bound-id
    (lambda (binding-gen name-gen old-id old-id-marks)
      (let* ((base-name (binding-var old-id))
	      (real-base-name (binding-orig-name old-id))
	      (new-base-name (name-gen real-base-name))
	      (new-name (symbol-append base-name "-init")))
	(let ((s (z:make-symbol (zodiac-origin old-id)
		   (zodiac-start old-id) (zodiac-finish old-id)
		   new-base-name new-base-name old-id-marks)))
	  ((create-binding+marks binding-gen
	     (lambda (_) new-name))
	    s)))))

  ; ----------------------------------------------------------------------

  (define-struct (top-level-resolution struct:resolutions) ())

  ; ----------------------------------------------------------------------

  (define make-new-environment make-hash-table)

  (define make-empty-environment make-new-environment)

  (define resolve
    (lambda (id env vocab)
      (let ((name (z:read-object id)) (marks (z:symbol-marks id)))
	(or (resolve-in-env name marks env)
	  (resolve-in-vocabulary name vocab)))))

  (define resolve-in-env
    (lambda (name marks env)
      (let ((v (hash-table-get env name (lambda () #f)))) ; name-eq?
	(and v
	  (let ((w (assoc marks v)))	; marks-equal?
	    (and w (cdr w)))))))

  (define resolve-in-vocabulary
    (let ((top-level-resolution (make-top-level-resolution 'dummy #f))) ; name-eq?
      (lambda (name vocab)
	(let ([id (delay (with-handlers ([void (lambda (x) #f)])
			   (global-defined-value name)))])
	  (let loop ((vocab vocab))
	    (if vocab
		(let ([id (if (vocabulary-record-namespace-based? vocab)
			      (force id)
			      name)])
		  (if id
		      (hash-table-get 
		       (vocabulary-record-this vocab)
		       id
		       (lambda ()
			 (loop (vocabulary-record-rest vocab))))
		      (loop (vocabulary-record-rest vocab))))
		top-level-resolution))))))

  (define (prepare-current-namespace-for-vocabulary vocab)
    ;; For everything in the vocabulary, ensure that the 
    ;;  namespace contains a binding to the expected key.
    ;;  Assume pre-existing bindings are ok (especially
    ;;  since we can't change the bindings of keywords).
    (let loop ((vocab vocab))
      (hash-table-for-each
       (vocabulary-record-this vocab)
       (lambda (k v)
	 (for-each
	  (lambda (n)
	    (with-handlers ([void (lambda (x)
				    (global-defined-value n k))])
	      (global-defined-value n)))
	  (let ([ns (resolutions-name v)])
	    (if (symbol? ns)
		(list ns)
		ns)))))
      (let ([v (vocabulary-record-rest vocab)])
	(when v
	  (loop v)))))
    
  (define (update-current-namespace name)
    (let ([id (syntax-symbol->id name)])
      (global-defined-value name id)))    

  (define print-env
    (lambda (env)
      (hash-table-map env (lambda (key value)
			    (printf "~s ->~n" key)
			    (pretty-print value)))))

  ; ----------------------------------------------------------------------

  (define extend-env
    (lambda (new-vars+marks env)
      (for-each
	(lambda (var+marks)
	  (let ((new-var (car var+marks)))
	    (let ((real-name (binding-orig-name new-var)))
	      (hash-table-put! env real-name
		(cons (cons (cdr var+marks) new-var)
		  (hash-table-get env real-name (lambda () '())))))))
	new-vars+marks)))

  (define retract-env
    (lambda (vars env)
      (let ((names (map binding-orig-name vars)))
	(for-each (lambda (name)
		    (hash-table-put! env name
		      (cdr (hash-table-get env name
			     (lambda ()
			       '(internal-error:dummy-for-sake-of-cdr!))))))
	  names))))

  (define copy-env
    (lambda (env)
      (let ([new (make-hash-table)])
	(hash-table-for-each
	 env
	 (lambda (key val)
	   (hash-table-put! new key val)))
	new)))

  )
