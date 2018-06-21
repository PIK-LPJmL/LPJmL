#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of grass part of LPJmL                      ##
##   creates library ../../lib/libgrass.a                                      ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = lai_grass.$O npp_grass.$O turnover_grass.$O\
          allocation_grass.$O fpc_grass.$O litter_update_grass.$O\
          phenology_grass.$O new_grass.$O light_grass.$O fpar_grass.$O\
          fwrite_grass.$O fscanpft_grass.$O fprint_grass.$O fread_grass.$O\
          establishment_grass.$O free_grass.$O fprintpar_grass.$O\
          vegc_sum_grass.$O init_grass.$O mix_veg_grass.$O fire_grass.$O\
          alphaa_grass.$O livefuel_consum_grass.$O reduce_grass.$O\
          annual_grass.$O turnover_monthly_grass.$O albedo_grass.$O adjust_grass.$O\
          turnover_daily_grass.$O agb_grass.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libgrass.$A


HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/param.h\
          $(INC)/grass.h $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/spitfire.h\
          $(INC)/config.h $(INC)/landuse.h $(INC)/climbuf.h $(INC)/discharge.h

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c
clean:
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
