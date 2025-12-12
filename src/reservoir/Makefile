#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of reservoir part of LPJmL                  ##
##   creates library ../../lib/libreservoir.a                                  ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    =  initreservoir.$O allocate_reservoir.$O irrig_amount_reservoir.$O\
           readreservoir.$O landusechange_for_reservoir.$O\
           check_stand_fracs_for_reservoir.$O outflow_reservoir.$O\
           update_reservoir_monthly.$O reservoir_surface_storage.$O\
           update_reservoir_annual.$O initresdata.$O\
           fwriteresdata.$O freadresdata.$O fprintresdata.$O\
           update_reservoir_daily.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libreservoir.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/soil.h\
          $(INC)/climate.h $(INC)/date.h $(INC)/swap.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h\
          $(INC)/errmsg.h $(INC)/numeric.h $(INC)/channel.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h\
          $(INC)/list.h $(INC)/cell.h $(INC)/units.h\
          $(INC)/config.h $(INC)/queue.h $(INC)/output.h\
          $(INC)/reservoir.h $(INC)/pnet.h $(INC)/intlist.h\
          $(INC)/discharge.h $(INC)/landuse.h $(INC)/timing.h\
          $(INC)/stand.h $(INC)/natural.h $(INC)/cdf.h $(INC)/spitfire.h\
          $(INC)/pftlist.h $(INC)/climbuf.h $(INC)/hydrotope.h $(INC)/bstruct.h


$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS) 
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
