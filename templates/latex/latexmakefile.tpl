{% if config.USE_PDFLATEX %}
all: refman.pdf

pdf: refman.pdf

refman.pdf: clean refman.tex
	pdflatex refman
	{{ config.MAKEINDEX_CMD_NAME }} refman.idx
{# TODO: generateBib #}
	pdflatex refman
	latex_count=8 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      pdflatex refman ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done
	{{ config.MAKEINDEX_CMD_NAME }} refman.idx
	pdflatex refman
{% else %}
all: refman.dvi

ps: refman.ps

pdf: refman.pdf

ps_2on1: refman_2on1.ps

pdf_2on1: refman_2on1.pdf

refman.ps: refman.dvi
	dvips -o refman.ps refman.dvi

refman.pdf: refman.ps
	ps2pdf refman.ps refman.pdf

refman.dvi: clean refman.tex doxygen.sty
	echo "Running latex..."
	{{ config.LATEX_CMD_NAME }} refman.tex
	echo "Running makeindex..."
	{{ config.MAKEINDEX_CMD_NAME }} refman.idx
{# TODO: generateBib #}
	echo "Rerunning latex...."
	{{ config.LATEX_CMD_NAME }} refman.tex
	latex_count=8 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' refman.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      {{ config.LATEX_CMD_NAME }} refman.tex ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done
	{{ config.MAKEINDEX_CMD_NAME }} refman.idx
	{{ config.LATEX_CMD_NAME }} refman.tex

refman_2on1.ps: refman.ps
	psnup -2 refman.ps >refman_2on1.ps

refman_2on1.pdf: refman_2on1.ps
	ps2pdf refman_2on1.ps refman_2on1.pdf
{% endif %}

clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl refman.pdf

