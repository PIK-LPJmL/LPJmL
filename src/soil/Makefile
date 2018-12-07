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
          fopensoilcode.$O litter_ag_sum.$O littersum.$O fprintlitter.$O\
          freadlitter.$O fwritelitter.$O newsoil.$O freesoil.$O copysoil.$O\
          findlitter.$O addlitter.$O freelitter.$O fprintsoilpar.$O\
          soilwater.$O soilcarbon.$O litter_ag_tree.$O litter_ag_grass.$O\
          moistfactor.$O litter_ag_sum_quick.$O freesoilpar.$O moisture2soilice.$O\
          soilconduct.$O soilheatcap.$O soilice2moisture.$O getlag.$O convert_water.$O\
          getstate.$O initsoiltemp.$O infil_perc_rain.$O infil_perc_irr.$O\
          fscanpool.$O

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
          $(INC)/climbuf.h $(INC)/reservoir.h $(INC)/agriculture.h

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
