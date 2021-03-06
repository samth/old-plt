(module match-internal-func mzscheme

  (provide (all-defined))
  
  (require-for-syntax "gen-match.ss"                      
                      "match-helper.ss"
                      "match-error.ss"
                      #;"convert-pat.ss")
  
  (require-for-template "gen-match.ss"                      
                        "match-helper.ss"
                        mzscheme)                        

  (require (lib "etc.ss")           
           (lib "list.ss")
           "gen-match.ss"
           "match-expander.ss"
           "match-helper.ss"
           "match-error.ss"
           "render-helpers.ss")

  
  (define-syntax (match stx)
    (syntax-case stx ()
      [(_ exp . clauses)       
       (if (identifier? #'exp)
           (gen-match #'exp '() #'clauses stx)
           (with-syntax ([body (gen-match #'x '() #'clauses stx)])
             #`(let ((x exp)) body)))]))
  
  (define-syntax (match-lambda stx)
    (syntax-case stx ()
      [(k . clauses)
       #'(lambda (exp) (match exp . clauses))]))

  (define-syntax (match-lambda* stx)
    (syntax-case stx ()
      [(k . clauses)
       #'(lambda exp (match exp . clauses))]))
  
  ;; there's lots of duplication here to handle named let
  ;; some factoring out would do a lot of good
  (define-syntax (match-let stx)
    (syntax-case stx ()
      ;; an empty body is an error
      [(_ nm (clauses ...))
       (identifier? #'nm)
       (match:syntax-err stx "bad syntax (empty body)")]
      [(_ (clauses ...)) (match:syntax-err stx "bad syntax (empty body)")]
      ;; with no bindings, there's nothing to do
      [(_ name () body ...) 
       (identifier? #'name)
       #'(let name () body ...)]
      [(_ () body ...) #'(let () body ...)]  
      ;; optimize the all-variable case            
      [(_ ([pat exp]...) body ...)
       (andmap pattern-var? (syntax->list #'(pat ...)))
       #'(let name ([pat exp] ...) body ...)]           
      [(_ name ([pat exp]...) body ...)
       (and (identifier? (syntax name))
            (andmap pattern-var? (syntax->list #'(pat ...))))
       #'(let name ([pat exp] ...) body ...)]
      ;; now the real cases
      [(_ name ([pat exp] ...) . body)
       #'(letrec ([name (match-lambda* ((list pat ...) . body))])                             
           (name exp ...))]        
      [(_ ([pat exp] ...) . body)
       #'((match-lambda* ((list pat ...) . body)) exp ...)]))
  
  (define-syntax (match-let* stx)
    (syntax-case stx ()
      [(_ (clauses ...)) (match:syntax-err stx "bad syntax (empty body)")]
      ((_ () body ...)
       #'(let* () body ...))
      ((_ ([pat exp] rest ...) body ...)
       (if (pattern-var? (syntax pat))
           #'(let ([pat exp])
                 (match-let* (rest ...) body ...))
           #'(match exp [pat (match-let* (rest ...) body ...)]))
       )))
  
  (define-syntax (match-letrec stx)
      (syntax-case stx ()
        [(_ (clauses ...)) (match:syntax-err stx "bad syntax (empty body)")]
        [(_ ([pat exp] ...) . body)
         (andmap pattern-var?
                 (syntax->list #'(pat ...)))
         #'(letrec ([pat exp] ...) . body)]
        [(_ ([pat exp] ...) . body)         
           (let* ((**match-bound-vars** '())
                  (compiled-match 
                   (gen-match #'the-exp
                              '()
                              #'(((list pat ...) never-used))
                              stx
                              (lambda (sf bv)
                                (set! **match-bound-vars** bv)
                                #`(begin
                                    #,@(map (lambda (x) #`(set! #,(car x) #,(cdr x)))
                                          (reverse bv))
                                    . body )))))
             #`(letrec (#,@(map
                            (lambda (x) #`(#,(car x) #f))
                            (reverse **match-bound-vars**))
                           (the-exp (list exp ...)))
                 #,compiled-match))]))
  
  (define-syntax (match-define stx)
    (syntax-case stx ()
        [(_ pat exp)
         (identifier? #'pat)
         #'(define pat exp)]
        [(_ pat exp)
         (let* ((**match-bound-vars** '())
                  (compiled-match
                   (gen-match #'the-exp
                              '()
                              #'((pat never-used))
                              stx
                              (lambda (sf bv)
                                (set! **match-bound-vars** bv)
                                #`(begin
                                    #,@(map (lambda (x)
                                              #`(set! #,(car x) #,(cdr x)))
                                            (reverse bv)))))))
             #`(begin #,@(map
                          (lambda (x) #`(define #,(car x) #f))
                          (reverse **match-bound-vars**))
                      (let ((the-exp exp))
                        #,compiled-match)))]))    
  )