LATEX	= latex -shell-escape
DVIPS	= dvips
PS2PDF = ps2pdf
SRC	= 'problem-statement'	#name of input file

all: pdf

dvi:
	$(LATEX) ${SRC}.tex

ps: dvi
	$(DVIPS) problem-statement.dvi

pdf: ps
	$(PS2PDF) problem-statement.ps

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg

.PHONY: all show clean ps pdf showps
