
(letrec* ([d (let ([wd (current-load-relative-directory)])
	       (if (file-exists? (build-path wd "demo.ss"))
		   wd
		   (let ([dd (build-path wd "demo")])
		     (if (file-exists? (build-path dd "demo.ss"))
			 dd
			 wd))))]
	  [f (make-object mred:simple-menu-frame%)]
	  [p2 (make-object mred:horizontal-panel% (send f get-top-panel))]
	  [p1 (make-object mred:horizontal-panel% (send f get-top-panel))]
	  [loader-button
	   (lambda (p name file instr go need-console?)
	     (make-object mred:button% p
			  (lambda (b e)
			    (load (build-path d file))
			    (send edit lock #f)
			    (send edit load-file (build-path d instr))
			    (send edit lock #t)
			    (when need-console?
				  (unless console
					  (set! console (make-object mred:console-frame%))))
			    (go))
			  name))]
	  [canvas (send f get-canvas)]
	  [edit (send f get-edit)]
	  [console (with-handlers ([void (lambda (exn) #f)])
		      (global-defined-value 'mred:console))])
  (send (send f get-top-panel) change-children reverse)
  (send p2 stretchable-in-y #f)
  (send p1 stretchable-in-y #f)
  (loader-button p1 "Phone Book" "phone.ss" "phone.mre" void #t)
  (loader-button p1 "NanoCAD" "ncad02x.ss" "ncad.mre" (lambda () (ncad:go)) #f)
  (loader-button p1 "Morphing" "morph.ss" "morph.mre" void #f)
  (loader-button p1 "Proof Systems" "toyproof.ss" "toyproof.mre" void #t)
  (loader-button p1 "Turtles" "turtles.ss" "turtles.mre" void #t)
  (loader-button p2 "Minesweeper" "mines.ss" "mines.mre" void #f)
  (loader-button p2 "Console Graph" "graph.ss" "graph.mre" void #t)
  (loader-button p2 "Simple Pasteboard" "draw.ss" "draw.mre" (lambda () (draw:go)) #f)
  (send edit set-auto-set-wrap #t)
  (send edit insert "Click on a button above.")
  (send edit lock #t)
  (send edit set-autowrap-bitmap null)
  (send canvas set-media edit)
  (send f show #t))


  