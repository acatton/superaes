PDFLATEX=pdflatex
LFLAGS=

report.pdf: report.tex report.toc
	$(PDFLATEX) $(LFLAGS) report.tex

report.toc: report.tex
	$(PDFLATEX) $(LFLAGS) report.tex

all: report.pdf

clean:
	rm -f report.{aux,log,out,toc}

mrproper: clean
	rm -f report.pdf


.PHONY: all clean mrproper
