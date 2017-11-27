########################################################################
##                                                                    ##
##               M  a  k  e  f  i  l  e                               ##
##                                                                    ##
##   Makefile to create HTML files from man pages                     ##
##                                                                    ##
##   written by Werner von Bloh                                       ##
##   Potsdam Institute for Climate Impact Research                    ##
##   P.O. Box 60 12 03                                                ##
##   14412 Potsdam/Germany                                            ##
##                                                                    ##
##   Last change: 05.01.2009                                          ##
##                                                                    ##
########################################################################


SRC	= clm.5 lpj.conf.5 lpjml.conf.5 lpjml_image.conf.5

HTMLDIR	= ../../html
HTML	= $(SRC:%.5=$(HTMLDIR)/%.html)

html: $(HTML)

$(HTMLDIR)/%.html: %.5
	nroff -man $*.5 |man2html -cgiurl '$$title.html' >$@
clean:
	rm $(HTML)
