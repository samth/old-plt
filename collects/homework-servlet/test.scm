WXME0106 ## 'wxtext wxtab wxmedia wximage  $(lib "comment-snip.ss" "framework")  +(lib "collapsed-snipclass.ss" "framework")   drscheme:sexp-snip   drscheme:syntax-snipclass%  drscheme:number  ,(lib "number-snip.ss" "drscheme" "private")   drscheme:bindings-snipclass%  drscheme:lambda-snip%  drscheme:define-snip%  gb:core  
gb:canvas  gb:editor-canvas  
gb:slider  	gb:gauge  gb:listbox  gb:radiobox  
gb:choice  gb:text  gb:message  
gb:button  gb:checkbox  gb:vertical-panel  	gb:panel  gb:horizontal-panel  !(lib "readable.ss" "guibuilder")  "drscheme:vertical-separator-snip%  wxbad   drscheme:test-suite:helper%  case%  def%  drscheme:xml-snip  (lib "xml-snipclass.ss" "xml")  drscheme:scheme-snip  "(lib "scheme-snipclass.ss" "xml")  HTML Bullet   wxloc     >   K         Z��Z����                                                       �������� 	Standard  KCourier New         Z��Z����                                                       ��������  �� ?�       \����������                              ?�      ?�      ?�      "��"   ����Matching Parenthesis Style  �� ?�       \����������                              ?�      ?�      ?�      "��"   ����  �� ?�       ������������                              ?�      ?�      ?�      (   ����drscheme:check-syntax:keyword  �� ?�       ������������                              ?�      ?�      ?�      (   ����  �� ?�       ������������                              ?�      ?�      ?�      ��@   ����'drscheme:check-syntax:unbound-variable  �� ?�       ������������                              ?�      ?�      ?�      ��@   ����  �� ?�       ������������                              ?�      ?�      ?�      $$��   ����%drscheme:check-syntax:bound-variable  �� ?�       ������������                              ?�      ?�      ?�      $$��   ���� drscheme:check-syntax:primitive  �� ?�       ������������                              ?�      ?�      ?�      $$��   ����  �� ?�       ������������                              ?�      ?�      ?�      3��'   ����drscheme:check-syntax:constant  �� ?�       ������������                              ?�      ?�      ?�      3��'   ����  �� ?�       ������������                              ?�      ?�      ?�      ����    ���� drscheme:check-syntax:tail-call  �� ?�       ������������                              ?�      ?�      ?�      ����    ����  �� ?�       ������������                              ?�      ?�      ?�      ��<$   ����drscheme:check-syntax:base  �� ?�       ������������                              ?�      ?�      ?�      ��<$   ����  F ?�       ������������      ?�      ?�      ?�      ?�      ?�      ?�            ����XML  F ?�       ������������      ?�      ?�      ?�      ?�      ?�      ?�            ����  G ?�       ������������      ?�      ?�      ?�      ?�      ?�      ?�            ����  �� ?�       ������������                             ?�      ?�      ?�      PP��   ����  G ?�       ������������                             ?�      ?�      ?�      PP��   ����  G ?�       ������������                              ?�      ?�      ?�       d    ����  F @        \����������      ?�      ?�      ?�      ?�      ?�      ?�            ����  F ?�       \����������      ?�      ?�      ?�      ?�      ?�      ?�            ����  F ?�       \����������                             ?�      ?�      ?�        ��   ����  F ?�      ��������������      ?�      ?�      ?�      ?�      ?�      ?�            ����  F ?�      ��������������                             ?�      ?�      ?�        ��   ����  F ?�      ��������������                              ?�      ?�      ?�      ��@   ����  F ?�       \����������                             ?�      ?�      ?�      2��2   ����  F ?�       ����]������      ?�      ?�      ?�      ?�      ?�      ?�            ����  F ?�      ������������      ?�      ?�      ?�      ?�      ?�      ?�            ����  F ?�      ������������                              ?�      ?�      ?�      PP��   ����  F ?�      \����������                              ?�      ?�      ?�      PP��   ����  F ?�       ������������                             ?�      ?�      ?�        ��   ����  K ?�       ������������                             ?�      ?�      ?�        ��   ����  K ?�       \����������                             ?�      ?�      ?�        ��   ����  F ?�       \����������      ?�      ?�      ?�      ?�      ?�      ?�            ����  F ?�33@    \����������      ?�      ?�      ?�      ?�      ?�      ?�            ����  �� ?�       ����^������                              ?�      ?�      ?�      ��     ����  �� ?�       ������������                              ?�      ?�      ?�      ��     ����   KCourier New         Z��Z����                                                       ��������  �� ?�       ������������                              ?�      ?�      ?�        ��   ����  F ?�       ����]������                              ?�      ?�      ?�      ��@   ����  F ?�       ����]������                             ?�      ?�      ?�        ��   ����  F @        \����������                             ?�      ?�      ?�        ��   ����  K ?�       ������������                              ?�      ?�      ?�      ��<$   ����  K ?�       \����������                              ?�      ?�      ?�      ��     ����  F ?陙�    ������������                             ?�      ?�      ?�        ��    ��  F ?�33@    ������������                             ?�      ?�      ?�        ��    ��  F ?�       \����������                             ?�      ?�      ?�        ��   ����  K ?�       \����������                             ?�      ?�      ?�      ��<$   ����  K ?�       \����������                             ?�      ?�      ?�      ��     ����  K ?�       ������������                              ?�      ?�      ?�      $$��   ����  K ?�       ������������      ?�      ?�      ?�      ?�      ?�      ?�            ����  K ?�       ������������                              ?�      ?�      ?�      "��"   ����  F ?�33@    \����������                             ?�      ?�      ?�        ��   ����  K ?�       ������������                              ?�      ?�      ?�      D@l   ����  K ?�       \����������      ?�      ?�      ?�      ?�      ?�      ?�            ����  K ?陙�    ������������                              ?�      ?�      ?�      ��<$    ��  �� ?�       \����������                                                        ���� ����            �6  N        % (define-syntax (test-macro stx)  
    (syntax-case stx ()  
      [(_ sig-name (ids ...))  
  &     (with-syntax ([(new-ids ...) (map  
  .                                  (lambda (id)  
  3                                    (string->symbol  
  Q                                     (string-append "get-" (symbol->string id))))  
  G                                  (syntax-object->datum #'(ids ...)))])  
  6       #'(define-signature sig-name (new-ids ...)))]))  
  
  (test-macro foo^ (a b c d e))  
  
  (unit/sig foo^  
  
  (import)  
      
    (define get-a (lambda x 'a))  
    (define get-b (lambda x 'b))  
    (define get-c (lambda x 'c))  
    (define get-d (lambda x 'd))  
    (define get-e (lambda x 'e)))       
  
   -        < (define-syntax (test-macro stx)  
    (syntax-case stx (struct)  
  7    [(_ sig-name (struct prefix (field-names ...)) ...)  
  !     (with-syntax ([(new-ids ...)  
                      (map  
  !                     (lambda (id)  
                         (cond  
  &                         [(symbol? id)  
  )                          (string->symbol  
  G                           (string-append "get-" (symbol->string id)))]  
                           [else  
  )                          (string->symbol  
  O                           (string-append "get-" (symbol->string (car id))))]))  
  =                     (syntax-object->datum #'(prefix ...)))])  
  6       #'(define-signature sig-name (new-ids ...)))]))  
  
  (test-macro foo^  
  !            (struct a (f1 f2 f3))  
               (struct (e a) (g h))  
              (struct b (f4 f5))  
              (struct c (f6 f7)))  
  
  (define-values/invoke-unit/sig  
   foo^  
   (unit/sig foo^  
     (import)  
       
     (define get-a (lambda x 'a))  
     (define get-b (lambda x 'b))  
     (define get-c (lambda x 'c))  
  !   (define get-e (lambda x 'e))))  
       
  
   �        = (define-syntax (test-macro stx)  
    (syntax-case stx (struct)  
  7    [(_ sig-name (struct prefix (field-names ...)) ...)  
  !     (with-syntax ([(new-ids ...)  
                      (map  
  !                     (lambda (id)  
  )                       (syntax-case id ()  
  &                         [(pref super)  
  ?                          (with-syntax ([new-id (string->symbol  
  y                                                 (string-append "get-" (symbol->string (syntax-object->datum #'pref))))])  
  &                            #'new-id)]  
                           [else  
  m                          (with-syntax ([new-id (string->symbol (string-append "get-" (symbol->string id)))])  
  (                            #'new-id)]))  
  <                    (syntax-object->datum #'(prefix ...)))])  
  6       #'(define-signature sig-name (new-ids ...)))]))  
  
  (test-macro foo^  
  !            (struct a (f1 f2 f3))  
               (struct (e a) (g h))  
              (struct b (f4 f5))  
              (struct c (f6 f7)))  
  
  (define-values/invoke-unit/sig  
   foo^  
   (unit/sig foo^  
     (import)  
       
     (define get-a (lambda x 'a))  
     (define get-b (lambda x 'b))  
     (define get-c (lambda x 'c))  
  !   (define get-e (lambda x 'e))))       
  
  
  
   x        { $(require-for-syntax (lib "list.ss"))  
  
  (define-syntax (test-macro stx)  
    (syntax-case stx ()  
  "    [(_ sig-name struct-exprs ...)  
  "     (with-syntax ([(sig-elts ...)  
                      (foldr  
  /                     (lambda (struct-expr rest)  
  <                       (syntax-case struct-expr (struct/sig)  
  K                         [(struct/sig (prefix super) (fields ...) more ...)  
  ?                          (with-syntax ([new-id (string->symbol  
  {                                                 (string-append "get-" (symbol->string (syntax-object->datum #'prefix))))])  
  2                            (cons #'new-id rest))]  
  C                         [(struct/sig prefix (fields ...) more ...)  
  ?                          (with-syntax ([new-id (string->symbol  
  {                                                 (string-append "get-" (symbol->string (syntax-object->datum #'prefix))))])  
  2                            (cons #'new-id rest))]  
  &                         [else rest]))  
                       '()  
  B                     (syntax-object->datum #'(struct-exprs ...)))]  
  %                   [(struct-defs ...)  
                      (map  
  *                     (lambda (struct-expr)  
  <                       (syntax-case struct-expr (struct/sig)  
  G                         [(struct (prefix super) (fields ...) more ...)  
  Q                          #'(define-struct (prefix super) (fields ...) more ...)]  
  K                         [(struct/sig (prefix super) (fields ...) more ...)  
  Q                          #'(define-struct (prefix super) (fields ...) more ...)]  
  ?                         [(struct prefix (fields ...) more ...)  
  I                          #'(define-struct prefix (fields ...) more ...)]  
  C                         [(struct/sig prefix (fields ...) more ...)  
  K                          #'(define-struct prefix (fields ...) more ...)]))  
  B                     (syntax-object->datum #'(struct-exprs ...)))]  
  )                   [(struct-provides ...)  
                      (map  
  *                     (lambda (struct-expr)  
  C                       (syntax-case struct-expr (struct struct/sig)  
  G                         [(struct (prefix super) (fields ...) more ...)  
  9                          #'(struct prefix (fields ...))]  
  K                         [(struct/sig (prefix super) (fields ...) more ...)  
  9                          #'(struct prefix (fields ...))]  
  ?                         [(struct prefix (fields ...) more ...)  
  9                          #'(struct prefix (fields ...))]  
  C                         [(struct/sig prefix (fields ...) more ...)  
  ;                          #'(struct prefix (fields ...))]))  
  C                     (syntax-object->datum #'(struct-exprs ...)))])  
         #'(begin  
  5           (define-signature sig-name (sig-elts ...))  
  '           (provide-signature sig-name)  
  (           (provide struct-provides ...)  
             struct-defs ...  
             ))]))  
  
  (test-macro foo^  
              (struct a (g1 g2))  
              (struct b (g2 g3)))  
                               
  (define-values/invoke-unit/sig  
   foo^  
   (unit/sig foo^  
     (import)  
     (define get-a (lambda x 'a))  
  !   (define get-b (lambda x 'b))))       
                              
                               
   H         (define-syntax (test-macro stx)  
    (syntax-case stx (struct)  
  0    [(_ sig-name (struct name (fields ...)) ...)  
       #'(begin  
  2         (define-struct name (fields ...)) ...)]))  
  
  (test-macro foo^  
   (struct a (f1 f2))  
   (struct b (g1 g2)))       
  
   �         (define-syntax (helper stx)  
    (syntax-case stx (struct)  
  #    [(_ (struct name (fields ...)))  
  +     #'(define-struct name (fields ...))]))  
  
  (define-syntax (test-macro stx)  
    (syntax-case stx ()  
      [(_ sig-name struct-expr)  
  C     (let ([def ((syntax-local-value #'helper) #'(_ struct-expr))])  
         #`(begin #,def))]))  
  
  $(test-macro foo^ (struct a (f1 f2)))       
  
  
  (  module   m mzscheme  
    (  require     (lib "unitsig.ss")  )  
    (  provide     
test-macro  )  
      
    (  define-syntax   (  make-struct-defs     stx  )  
      (  syntax-case     stx   (struct struct/sig)  
        [(_ )   #'  ()  ]  
        [(_ (  struct  / (name super) (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-defs  )   #'  (_ rest ...))])  
  	           #`  (  3(define-struct (name super) (fields ...) stuff ...)  
              #,@  more  ))]  
        [(_ (  
struct/sig  / (name super) (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-defs  )   #'  (_ rest ...))])  
  	           #`  (  3(define-struct (name super) (fields ...) stuff ...)  
              #,@  more  ))]  
        [(_ (  struct  ' name (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-defs  )   #'  (_ rest ...))])  
  	           #`  (  +(define-struct name (fields ...) stuff ...)  
              #,@  more  ))]  
        [(_ (  
struct/sig  ' name (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-defs  )   #'(_ rest ...))])  
  	           #`  (  +(define-struct name (fields ...) stuff ...)  
              #,@  more  ))]))  
      
    (  define-syntax   (  make-struct-provides     stx  )  
      (  syntax-case     stx   (struct struct/sig)  
        [(_ )   #'  ()  ]  
        [(_ (  struct  / (name super) (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-provides  )   #'  (_ rest ...))])  
  	           #`  (  $(provide (struct name (fields ...)))  
              #,@  more  ))]  
        [(_ (  
struct/sig  / (name super) (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-provides  )   #'  (_ rest ...))])  
  	           #`  (  $(provide (struct name (fields ...)))  
              #,@  more  ))]  
        [(_ (  struct  ' name (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-provides  )   #'  (_ rest ...))])  
  	           #`  (  $(provide (struct name (fields ...)))  
              #,@  more  ))]  
        [(_ (  
struct/sig  ' name (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-struct-provides  )   #'  (_ rest ...))])  
  	           #`  (  $(provide (struct name (fields ...)))  
              #,@  more  ))]))  
      
    (  define-syntax   (  make-sig-elts     stx  )  
      (  syntax-case     stx   (struct struct/sig)  
        [(_ )   #'  ()  ]  
        [(_ (  struct  / (name super) (fields ...) stuff ...) rest ...)  
  J       (let ([more ((syntax-local-value #'make-sig-elts) #'(_ rest ...))])  
           more)]  
        [(_ (  
struct/sig  / (name super) (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-sig-elts  )   #'  (_ rest ...))])  
  
         (  with-syntax   ([new-name (  string->symbol  
  #                                  (  string-append     "get-"   (  symbol->string   (  syntax-object->datum     #'  
name))))])  
               #`  (new-name #,@  more  )))]  
        [(_ (  -struct name (fields ...) stuff ...) rest ...)  
  J       (let ([more ((syntax-local-value #'make-sig-elts) #'(_ rest ...))])  
           more)]  
        [(_ (  
struct/sig  ' name (fields ...) stuff ...) rest ...)  
         (  let   ([  more   ((  syntax-local-value     #'  make-sig-elts  )   #'  (_ rest ...))])  
  
         (  with-syntax   ([new-name (  string->symbol  
  #                                  (  string-append     "get-"   (  symbol->string   (  syntax-object->datum     #'  
name))))])  
               #`  (new-name #,@  more  )))]))  
      
      
    (  define-syntax   (  
test-macro     stx  )  
      (  syntax-case     stx   ()  
  $      [(_ sig-name struct-exprs ...)  
         (  let   ([  struct-defs   ((  syntax-local-value     #'  make-struct-defs  )   #'  (_ struct-exprs ...))]  
               [  struct-provides   ((  syntax-local-value     #'  make-struct-provides  )   #'  (_ struct-exprs ...))]  
               [  sig-elts   ((  syntax-local-value     #'  make-sig-elts  )   #'  (_ struct-exprs ...))])  
  	           #`  (  begin     (  define-signature   sig-name #,  sig-elts  )  
  $                  (provide sig-name)  
                    #,@  struct-defs  
                    #,@  struct-provides  ))]))  
    )  
  
  (  module   test   mzscheme  
    (  require     m  )  
      
    (  
test-macro   foo^  
                (struct/sig   a   (  f1     f2  ) (make-inspector))  
                (struct   b (g1     g2  ))  
                (struct/sig (  c     a  ) (  h1     h2  ) (make-inspector))  
                (struct (d     c  ) (  i     j  ))  
                (struct/sig   e   (  t     y  ) (make-inspector))))  
  
  (require test)  
  
  (define-values/invoke-unit/sig  
   foo^  
   (unit/sig foo^  
     (import)  
     (define get-a  
  !     (lambda x (make-a 'a1 'a2)))  
     (define get-c  
  )     (lambda x (make-c 'c1 'c2 'c3 'c4)))  
     (define get-e  
  #     (lambda x (make-e 'e1 'e2)))))  
        