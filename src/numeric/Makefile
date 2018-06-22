#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of numeric part of LPJmL                    ##
##   creates library ../../lib/libnum.a                                        ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = leftmostzero.$O bisect.$O linreg.$O date.$O interpolate.$O\
          buffer.$O rand.$O petpar.$O\
          ivec_sum.$O int2date.$O petpar2.$O\
          petpar3.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libnum.$A

HDRS    = $(INC)/types.h $(INC)/numeric.h $(INC)/date.h\
          $(INC)/list.h $(INC)/buffer.h $(INC)/errmsg.h 

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
