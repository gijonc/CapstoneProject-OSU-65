LATEX	= latex -shell-escape
DVIPS	= dvips
PS2PDF = ps2pdf
SRC	:= $(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
DVIF	= $(SRC:%.tex=%.dvi)
PSF	= $(SRC:%.tex=%.ps)
PDF	= $(SRC:%.tex=%.pdf)

all: pdf

dvi:
	$(LATEX) ${SRC}

ps: dvi
	$(DVIPS) ${DVIF}

pdf: ps
	$(PS2PDF) ${PSF}

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg

.PHONY: all show clean ps pdf showps
