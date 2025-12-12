#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of soil part of LPJmL                       ##
##   creates library ../../lib/libsoil.a                                       ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = initsoil.$O waterbalance.$O equilsoil.$O littersom.$O\
          getlag.$O temp_response.$O fscansoilpar.$O soiltemp.$O laketemp.$O snow.$O\
          fire_sum.$O fire_prob.$O freadsoil.$O fwritesoil.$O fprintsoil.$O\
          freesoilpar.$O seeksoilcode.$O freadsoilcode.$O getnsoilcode.$O\
          fopensoilcode.$O litter_agtop_sum.$O littercarbon.$O fprintlitter.$O\
          hydrotopes.$O inithydro.$O update_wetland.$O\
          freadhydrotope.$O fscanhydropar.$O fwritehydrotope.$O \
          freadlitter.$O fwritelitter.$O newsoil.$O freesoil.$O copysoil.$O\
          findlitter.$O addlitter.$O freelitter.$O fprintsoilpar.$O\
          soilwater.$O soilcarbon.$O litter_agtop_tree.$O litter_agtop_grass.$O\
          moistfactor.$O litter_agtop_sum_quick.$O freesoilpar.$O moisture2soilice.$O\
          soilconduct.$O soilheatcap.$O soilice2moisture.$O getlag.$O convert_water.$O\
          gasdiffusion.$O getstate.$O initsoiltemp.$O infil_perc.$O\
          ebullition.$O fprinthydrotope.$O fprinthydropar.$O soilmethane.$O\
          fscanpoolpar.$O denitrification.$O biologicalNfixation.$O\
          litter_agtop_nitrogen_tree.$O soilstocks.$O litterstocks.$O fscansoilmap.$O\
          checklitter.$O getrootdist.$O nuptake_temp_fcn.$O volatilization.$O\
          getwr.$O updatelitterproperties.$O pedotransfer.$O tillage.$O\
          soilpar_output.$O getsoilmap.$O defaultsoilmap.$O cmpsoilmap.$O\
          calc_soil_thermal_props.$O apply_heatconduction_of_a_day.$O compute_mean_layer_temps_from_enth.$O\
          apply_enth_of_untracked_mass_shifts.$O freezefrac2soil.$O enth2freezefrac.$O\
          apply_perc_enthalpy.$O update_soil_thermal_state.$O freadpool.$O fwritepool.$O\
          soil_status.$O daily_littersom.$O littersom_nomethane.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libsoil.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h $(INC)/landuse.h\
          $(INC)/crop.h $(INC)/errmsg.h $(INC)/numeric.h $(INC)/spitfire.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/output.h\
          $(INC)/config.h $(INC)/param.h $(INC)/cdf.h $(INC)/discharge.h\
          $(INC)/climbuf.h $(INC)/reservoir.h $(INC)/agriculture.h\
          $(INC)/wetland.h $(INC)/hydrotope.h $(INC)/bstruct.h $(INC)/timing.h

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
