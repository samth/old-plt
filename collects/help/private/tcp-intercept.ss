(module tcp-intercept mzscheme
  (provide tcp-intercept@ url-intercept@)
  
  (require (lib "unitsig.ss")
           (lib "etc.ss")
           (lib "sig.ss" "web-server")
           (lib "tcp-sig.ss" "net")
           (lib "url-sig.ss" "net")
           "internal-hp.ss")
  
  (define-syntax (redefine stx)
    (syntax-case stx ()
      [(_ names ...)
       (with-syntax ([(defs ...) (map (lambda (x)
                                        (with-syntax ([orig-name x]
                                                      [raw-name
                                                       (datum->syntax-object
                                                        x
                                                        (string->symbol
                                                         (string-append
                                                          "raw:"
                                                          (symbol->string (syntax-object->datum x)))))])
                                          (syntax (define orig-name raw-name))))
                                      (syntax->list (syntax (names ...))))])
         (syntax (begin defs ...)))]))
  
  (define url-intercept@
    (unit/sig net:url^
      (import (raw : net:url^))
      
      (define (url->string url)
        (cond
          [(and (equal? (url-port url) internal-port)
                (equal? (url-host url) internal-host))
           (let* ([long
                   (raw:url->string 
                    (raw:make-url ""
                                  (raw:url-user url)
                                  ""
                                  #f
                                  (raw:url-path url)
                                  (raw:url-query url)
                                  (raw:url-fragment url)))])
             (substring long 3 (string-length long)))]
          [else (raw:url->string url)]))
      
      (redefine make-url
                struct:url
                url-scheme set-url-scheme!
                url-user set-url-user!
                url-host set-url-host!
                url-port set-url-port!
                url-path set-url-path!
                url-query set-url-query!
                url-fragment set-url-fragment!
                url?
                
                struct:path/param
                make-path/param
                path/param-path set-path/param-path!
                path/param-param set-path/param-param!
                path/param?
                
                get-pure-port
                get-impure-port
                post-pure-port
                post-impure-port
                display-pure-port
                purify-port
                netscape/string->url
                string->url
                call/input-url
                combine-url/relative
                url-exception?
                current-proxy-servers)))
  
  (define raw:tcp-abandon-port tcp-abandon-port)
  (define raw:tcp-accept tcp-accept) 
  (define raw:tcp-accept-ready? tcp-accept-ready?)
  (define raw:tcp-addresses tcp-addresses)
  (define raw:tcp-close tcp-close)
  (define raw:tcp-connect tcp-connect)
  (define raw:tcp-connect/enable-break tcp-connect/enable-break)
  (define raw:tcp-listen tcp-listen)
  (define raw:tcp-listener? tcp-listener?)
  
  ; For tcp-listeners, we use an else branch in the conds since
  ; (instead of a contract) I want the same error message as the raw
  ; primitive for bad inputs.
  
  ; : (listof nat) -> (unit/sig () -> net:tcp^)
  (define tcp-intercept@
    (unit/sig net:tcp^
      (import web-server^)
      
      ; : port -> void
      (define (tcp-abandon-port tcp-port)
        (cond
          [(tcp-port? tcp-port)
           (raw:tcp-abandon-port tcp-port)]
          [(input-port? tcp-port)
           (close-input-port tcp-port)]
          [(output-port? tcp-port)
           (close-output-port tcp-port)]
          [else (void)]))
      
      ; : listener -> iport oport
      (define tcp-accept raw:tcp-accept)
      
      ; : tcp-listener -> iport oport
      (define tcp-accept-ready? raw:tcp-accept-ready?)
      
      ; : tcp-port -> str str
      (define (tcp-addresses tcp-port)
        (if (tcp-port? tcp-port)
            (raw:tcp-addresses tcp-port)
            (values "127.0.0.1" internal-host)))
      
      ; : port -> void
      (define tcp-close raw:tcp-close)
      
      ; : (str nat -> iport oport) -> str nat -> iport oport
      (define (gen-tcp-connect raw)
        (lambda (hostname-string port)
          (if (and (or (string=? internal-host hostname-string)
                       (string=? addon-host hostname-string))
                   (equal? internal-port port))
              (let-values ([(req-in req-out) (make-pipe)]
                           [(resp-in resp-out) (make-pipe)])
                (parameterize ([current-custodian (make-custodian)])
                  (serve-ports req-in resp-out))
                (values resp-in req-out))
              (raw hostname-string port))))
      
      ; : str nat -> iport oport
      (define tcp-connect (gen-tcp-connect raw:tcp-connect))
      
      ; : str nat -> iport oport
      (define tcp-connect/enable-break (gen-tcp-connect raw:tcp-connect/enable-break))
      
      ; FIX - support the reuse? flag.
      (define tcp-listen raw:tcp-listen)
      
      ; : tst -> bool
      (define tcp-listener? raw:tcp-listener?))))
