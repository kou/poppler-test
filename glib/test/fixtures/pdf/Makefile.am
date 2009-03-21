EXTRA_DIST =		\
	simple-page.odt	\
	simple-page.pdf	\
	multi-pages.odt	\
	multi-pages.pdf	\
	form.odt	\
	form.pdf	\
	attachment.pdf	\
	empty.sla	\
	empty.pdf	\
	hello.txt	\
	hello.html	\
	layers.sla	\
	layers.pdf	\
	shape.pdf	\
	shape.svg	\
	slide.odp	\
	slide.pdf

attachment.pdf: empty.pdf hello.txt hello.html
	pdftk $(srcdir)/empty.pdf				\
	  attach_files $(srcdir)/hello.txt $(srcdir)/hello.html	\
	  output $@
