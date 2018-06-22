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

SRC	= pnet_addconnect.3 pnet_hi.3 pnet_input.3 pnet_outlen.3 pnet_strerror.3\
          pnet_dup.3 pnet_inindex.3 pnet_lo.3 pnet_output.3\
          pnet_exchg.3 pnet_init.3 pnet_nodes.3 pnet_reverse.3\
          pnet_free.3 pnet_inlen.3 pnet_outindex.3 pnet_setup.3\
          iterate.3 newgrid.3 fopenoutput.3 readconfig.3 initclimate.3\
          initwateruse.3 initlanduse.3 initdrain.3 getpopdens.3\
          initpopdens.3 initinput.3 writefloat_socket.3 close_image.3\
          close_socket.3 connect_socket.3 failonerror.3 fcloseoutput.3\
          flux_sum.3 fopenoutput.3 fprintconfig.3 fprintflux.3\
          freeclimate.3 freeconfig.3 freegrid.3 freeinput.3\
          freelanduse.3 freewateruse.3 fwriterestart.3 getclimate.3\
          getco2.3 getlanduse.3 getwateruse.3 initconfig.3 isopen.3\
          isroot.3 iterateyear.3 open_image.3 open_socket.3\
          printflags.3 readint_socket.3 writecoords.3 writecountrycode.3\
          writeregioncode.3 writeint_socket.3 initmpiconfig.3


HTMLDIR	= ../../html
HTML	= $(SRC:%.3=$(HTMLDIR)/%.html)

html: $(HTML)

$(HTMLDIR)/%.html: %.3
	nroff -man $*.3 |man2html -cgiurl '$$title.html' >$@
clean:
	rm $(HTML)
