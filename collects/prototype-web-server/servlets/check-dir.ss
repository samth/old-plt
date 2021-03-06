(module check-dir "../web-interaction.ss"
  (require (lib "url.ss" "net"))
  
  (define (directory-page n)
    (let ([throw-away
           (send/suspend
            (lambda (k-url)
              `(html (head (title ,(format "Page ~a" n)))
                     (body
                      (h1 ,(format "Page ~a" n))
                      (h2 ,(format "The current directory: ~a" (current-directory)))
                      (p "Click " (a ([href ,(url->string k-url)]) "here") " to continue.")))))])
      (directory-page (add1 n))))
  
  (let ([req0 (start-servlet)])
    (directory-page 1)))