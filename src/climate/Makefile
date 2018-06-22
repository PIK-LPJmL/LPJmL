#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of climate part of LPJmL                    ##
##   creates library ../../lib/libclimate.a                                    ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = initclimate.$O getclimate.$O freeclimate.$O avgtemp.$O\
          prdaily.$O getco2.$O storeclimate.$O dailyclimate.$O\
          getmtemp.$O initclimate_monthly.$O openclimate.$O\
          getmprec.$O checkvalidclimate.$O readco2.$O\
          closeclimate.$O radiation.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libclimate.$A

HDRS    = $(INC)/buffer.h $(INC)/lpj.h\
          $(INC)/climate.h $(INC)/date.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h\
          $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h\
          $(INC)/config.h $(INC)/param.h $(INC)/cdf.h


$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS)  $(LIB))
