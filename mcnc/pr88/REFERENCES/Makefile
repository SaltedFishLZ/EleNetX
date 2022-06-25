.SUFFIXES: .bib .lpr .laser 

.bib.laser :
	sortbib $*.bib | refer -a1 -B | troff -mbib 

.bib.lpr :
	sortbib $*.bib | refer -a1 -B | nroff -mbib > $*.lpr
