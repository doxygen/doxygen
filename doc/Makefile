include ../Makefile.config

DOXYDIR=../bin
DOXYGEN_DOCDIR=..

all: FORCE
	DOXYGEN_DOCDIR=$(DOXYGEN_DOCDIR); \
	export DOXYGEN_DOCDIR; \
        VERSION=$(VERSION) ; \
	export VERSION; \
	$(DOXYDIR)/doxygen
	@rm -f ../latex/refman.tex
	@cp doxygen_logo*.gif ../html
	@cp Makefile.latex ../latex/Makefile
	@sed -e "s/\$$VERSION/$(VERSION)/g" doxygen_manual.tex >../latex/doxygen_manual.tex
	@sed -e "s/\$$VERSION/$(VERSION)/g" doxygen.sty >../latex/doxygen.sty 
	@cp doxygen_logo.eps ../latex

clean: 
	rm -rf ../html ../latex

FORCE:
