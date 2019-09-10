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


SRC	= cat2bsq.1 lpjml.1 txt2grid.1\
          cft2clm.1 lpjcat.1 lpj_paths.csh.1 lpjsubmit.1\
          configure.sh.1 lpjcheck.1 lpj_paths.sh.1 txt2clm.1\
          cru2clm.1 lpjprint.1 output_bsq.1 asc2clm.1\
          grid2clm.1 lpjrun.1 cdf2bin.1 cdf2coord.1\
          backtrace.1 catclm.1 printclm.1 adddrain.1\
          printreservoir.1 printharvest.1 setclm.1 lpjfiles.1\
          regridlpj.1 cdf2clm.1 clm2cdf.1 soil2cdf.1 cdf2soil.1 bin2cdf.1\
          cutclm.1 cvrtclm.1 manage2js.1 headersize.1

HTMLDIR	= ../../html
HTML	= $(SRC:%.1=$(HTMLDIR)/%.html)

html: $(HTML)

$(HTMLDIR)/%.html: %.1
	nroff -man $*.1 |man2html -cgiurl '$$title.html' -title $*.1 >$@
clean:
	rm -f $(HTML)
