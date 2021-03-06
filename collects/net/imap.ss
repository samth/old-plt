
(module imap mzscheme
  (require (lib "unitsig.ss")
           (lib "contract.ss"))
  
  (require "imap-sig.ss"
           "imap-unit.ss")
  
  (define-values/invoke-unit/sig net:imap^
                                 net:imap@)
  
  (provide/contract
   [imap-get-hierarchy-delimiter (imap-connection? . -> . bytes?)]
   [imap-list-child-mailboxes
    (case->
     (imap-connection? (union false/c bytes?) . -> . (listof (list/c (listof symbol?) bytes?)))
     (imap-connection? (union false/c bytes?) (union false/c bytes?)
                       . -> .
                       (listof (list/c (listof symbol?) bytes?))))])
   
  (provide
   imap-connection?
   imap-connect imap-connect*
   imap-disconnect
   imap-force-disconnect
   imap-reselect
   imap-examine
   imap-noop
   imap-poll
   imap-status
   
   imap-port-number ; a parameter

   imap-new?
   imap-messages
   imap-recent
   imap-uidnext
   imap-uidvalidity
   imap-unseen
   imap-reset-new!
   
   imap-get-expunges
   imap-pending-expunges?
   imap-get-updates
   imap-pending-updates?
   
   imap-get-messages
   imap-copy imap-append
   imap-store imap-flag->symbol symbol->imap-flag
   imap-expunge
   
   imap-mailbox-exists?
   imap-create-mailbox
   
   imap-mailbox-flags))