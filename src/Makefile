#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of LPJmL                                    ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../Makefile.inc

OBJ    = lpjml.$O  

SRC    = lpjml.c

INC     = ../include

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/manage.h $(INC)/climate.h $(INC)/date.h\
          $(INC)/pft.h $(INC)/pftpar.h $(INC)/types.h $(INC)/outfile.h\
          $(INC)/header.h $(INC)/landuse.h $(INC)/crop.h $(INC)/tree.h $(INC)/grass.h\
          $(INC)/errmsg.h $(INC)/numeric.h $(INC)/conf.h $(INC)/swap.h\
          $(INC)/soilpar.h $(INC)/managepar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/output.h\
          $(INC)/config.h $(INC)/intlist.h $(INC)/queue.h $(INC)/pnet.h\
          $(INC)/discharge.h $(INC)/channel.h $(INC)/biomes.h\
          $(INC)/image.h $(INC)/input.h $(INC)/natural.h $(INC)/grassland.h\
          $(INC)/biomass_tree.h $(INC)/biomass_grass.h $(INC)/cdf.h\
          $(INC)/agriculture.h $(INC)/reservoir.h $(INC)/spitfire.h\
          $(INC)/cpl.h

LIBDIR  = ../lib
BINDIR  = ../bin

LPJLIBS = $(LIBDIR)/liblpj.$A $(LIBDIR)/libgrass.$A $(LIBDIR)/liblanduse.$A $(LIBDIR)/libtree.$A $(LIBDIR)/libimage.$A\
          $(LIBDIR)/libspitfire.$A $(LIBDIR)/libsoil.$A\
          $(LIBDIR)/libclimate.$A $(LIBDIR)/libnum.$A $(LIBDIR)/libtools.$A\
          $(LIBDIR)/libcrop.$A $(LIBDIR)/libreservoir.$A $(LIBDIR)/libpnet.$A\
          $(LIBDIR)/libsocket.$A $(LIBDIR)/liblpj.$A $(LIBDIR)/libcdf.$A $(LIBDIR)/liblpj.$A\
          $(LIBDIR)/libtools.$A $(LIBDIR)/libcpl.$A $(LIBDIR)/libsoil.$A

EXE     = $(BINDIR)/lpjml$E

bin: 
	$(MAKE) libs
	$(MAKE) $(EXE)

libs:
	(cd climate && $(MAKE))
	(cd numeric && $(MAKE))
	(cd lpj && $(MAKE))
	(cd soil && $(MAKE))
	(cd tree && $(MAKE))
	(cd grass && $(MAKE))
	(cd crop && $(MAKE))
	(cd tools && $(MAKE))
	(cd landuse && $(MAKE))
	(cd pnet && $(MAKE))
	(cd image && $(MAKE))
	(cd socket && $(MAKE))
	(cd reservoir && $(MAKE))
	(cd spitfire && $(MAKE))
	(cd netcdf && $(MAKE))
	(cd cpl && $(MAKE))

clean:
	(cd climate && $(MAKE) clean)
	(cd lpj && $(MAKE) clean)
	(cd numeric && $(MAKE) clean)
	(cd soil && $(MAKE) clean)
	(cd tree && $(MAKE) clean)
	(cd grass && $(MAKE) clean)
	(cd crop && $(MAKE) clean)
	(cd tools && $(MAKE) clean)
	(cd landuse && $(MAKE) clean)
	(cd pnet && $(MAKE) clean)
	(cd utils && $(MAKE) clean)
	(cd image && $(MAKE) clean)
	(cd socket && $(MAKE) clean)
	(cd reservoir && $(MAKE) clean)
	(cd spitfire && $(MAKE) clean)
	(cd netcdf && $(MAKE) clean)
	(cd cpl && $(MAKE) clean)
	$(RM) $(RMFLAGS) $(OBJ) getbuild.$O $(EXE)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c


$(EXE): $(LPJLIBS) $(OBJ)
	$(CC) $(CFLAGS) -I$(INC) -c getbuild.c
	$(LINKMAIN) $(LNOPTS)$(EXE) $(OBJ) $(LPJLIBS) $(LIBS) getbuild.$O 
