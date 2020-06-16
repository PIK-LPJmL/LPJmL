#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of tree part of LPJmL                       ##
##   creates library ../../lib/libtree.a                                       ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = lai_tree.$O turnover_tree.$O npp_tree.$O phenology_tree.$O\
          mortality_tree.$O init_tree.$O firemortality_tree.$O\
          allocation_tree.$O isneg_tree.$O light_tree.$O fpar_tree.$O\
          litter_update_tree.$O fpc_tree.$O allometry_tree.$O\
          new_tree.$O fwrite_tree.$O fscanpft_tree.$O  fprint_tree.$O \
          fread_tree.$O establishment_tree.$O fire_tree.$O free_tree.$O\
          vegc_sum_tree.$O adjust_tree.$O mix_veg_tree.$O fprintpar_tree.$O\
          alphaa_tree.$O livefuel_consum_tree.$O reduce_tree.$O\
          annual_tree.$O coppice_tree.$O turnover_monthly_tree.$O\
          albedo_tree.$O turnover_daily_tree.$O agb_tree.$O litter_update_fire_tree.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libtree.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/tree.h\
          $(INC)/errmsg.h $(INC)/numeric.h $(INC)/param.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h\
          $(INC)/config.h $(INC)/landuse.h $(INC)/spitfire.h\
          $(INC)/climbuf.h $(INC)/discharge.h $(INC)/biomass_tree.h

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean:
	$(RM) $(RMFLAGS) $(OBJS) 
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
