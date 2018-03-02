#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of distributed networks of LPJmL            ##
##   creates library ../../lib/libpnet.a                                       ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://gitlab.pik-potsdam.de/lpjml                                ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = pnet_init.$O  pnet_setup.$O pnet_addconnect.$O intlist.$O\
          pnet_reverse.$O pnet_free.$O pnet_strerror.$O pnet_dup.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libpnet.$A

HDRS    = $(INC)/types.h $(INC)/pnet.h $(INC)/intlist.h\
          $(INC)/errmsg.h


$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) $(JSON_INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
