#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of landuse part of LPJmL                    ##
##   creates library ../../lib/liblanduse.a                                    ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = cultivate.$O landuse.$O landusechange.$O\
          sowing_season.$O sowing_prescribe.$O\
          check_lu.$O agriculture.$O reclaim_land.$O allocation_today.$O\
          initmanage.$O fscancountrypar.$O\
          findlandusetype.$O findstand.$O setaside.$O harvest_stand.$O\
          opencountrycode.$O readcountrycode.$O grassland.$O\
          biomass_tree.$O biomass_grass.$O\
          createcountrycode.$O freecountrypar.$O\
          writecountrycode.$O freemanage.$O\
          timber_burn.$O timber_harvest.$O annual_grassland.$O\
          annual_agriculture.$O annual_biomass_grass.$O annual_setaside.$O\
          annual_woodplantation.$O daily_woodplantation.$O\
          daily_grassland.$O daily_biomass_grass.$O annual_biomass_tree.$O\
          daily_agriculture.$O daily_biomass_tree.$O output_gbw.$O\
          woodplantation.$O\
          output_gbw_agriculture.$O calc_nir.$O irrig_amount.$O killstand.$O\
          landfrac.$O cutpfts.$O crop_sum_frac.$O\
          new_agriculture.$O free_agriculture.$O fwrite_agriculture.$O\
          fread_agriculture.$O fprint_agriculture.$O\
          fprintcountrypar.$O wateruse.$O withdrawal_demand.$O\
          distribute_water.$O sowing.$O rw_irrigation.$O\
          fwrite_irrigation.$O fread_irrigation.$O fprint_irrigation.$O\
          new_grassland.$O fwrite_grassland.$O fread_grassland.$O\
          fprint_grassland.$O new_biomass_tree.$O fread_biomass_tree.$O\
          fwrite_biomass_tree.$O fprint_biomass_tree.$O init_irrigation.$O\
          scancftmap.$O\
          fscanmowingdays.$O getnsoil_agr.$O agriculture_tree.$O agriculture_grass.$O\
          daily_agriculture_tree.$O daily_agriculture_grass.$O fscantreedens.$O\
          findstandpft.$O isirrigevent.$O\
          annual_agriculture_tree.$O annual_agriculture_grass.$O readcottondays.$O\
          update_irrig.$O update_separate_harvests.$O fscanagtreemap.$O sowingcft.$O\
          fertday_biomass.$O fertilize_tree.$O setotherstocrop.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = liblanduse.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/biomes.h\
          $(INC)/landuse.h $(INC)/date.h $(INC)/image.h $(INC)/crop.h $(INC)/tree.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h $(INC)/natural.h\
          $(INC)/errmsg.h $(INC)/numeric.h $(INC)/manage.h $(INC)/grassland.h\
          $(INC)/biomass_tree.h $(INC)/biomass_grass.h $(INC)/discharge.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/agriculture.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/output.h\
          $(INC)/config.h $(INC)/stand.h $(INC)/pft.h $(INC)/pftlist.h\
          $(INC)/reservoir.h $(INC)/soil.h $(INC)/cropdates.h $(INC)/grass.h\
          $(INC)/tree.h $(INC)/outfile.h $(INC)/cdf.h $(INC)/param.h\
          $(INC)/spitfire.h $(INC)/climbuf.h $(INC)/woodplantation.h\
          $(INC)/agriculture_tree.h $(INC)/agriculture_grass.h $(INC)/coupler.h

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
