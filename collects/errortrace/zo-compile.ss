
(module zo-compile mzscheme
  (require "errortrace-lib.ss")
  
  (provide zo-compile)

  (define zo-compile
    (let ([orig (current-compile)])
      (lambda (stx immediate-eval?)
	(if (null? (use-compiled-file-paths))
	    (orig stx immediate-eval?)
	    (orig (errortrace-annotate stx) immediate-eval?))))))



