#################################################################################
##                                                                             ##
##                        M  a  k  e  f  i  l  e                               ##
##                                                                             ##
##   Makefile to create HTML files from man pages                              ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://gitlab.pik-potsdam.de/lpjml                                ##
##                                                                             ##
#################################################################################


SRC	= clm.5 lpj.conf.5 lpjml.conf.5 lpjml_image.conf.5

HTMLDIR	= ../../html
HTML	= $(SRC:%.5=$(HTMLDIR)/%.html)

html: $(HTML)

$(HTMLDIR)/%.html: %.5
	nroff -man $*.5 |man2html -cgiurl '$$title.html' >$@
clean:
	rm $(HTML)
