#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of LPJmL                                    ##
##   The call of ./configure.sh copies os-specific makefile.* in               ##
##   directory config to Makefile.inc                                          ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include Makefile.inc

TARFILE = lpjml-$(shell cat VERSION).tar

ZIPFILE = lpjml-$(shell cat VERSION).zip

INC     = include

HDRS    = $(INC)/buffer.h $(INC)/cell.h $(INC)/climate.h $(INC)/conf.h\
          $(INC)/config.h $(INC)/coord.h $(INC)/crop.h $(INC)/cropdates.h\
          $(INC)/date.h $(INC)/discharge.h $(INC)/param.h $(INC)/input.h\
          $(INC)/errmsg.h $(INC)/grass.h $(INC)/header.h $(INC)/landuse.h\
          $(INC)/list.h $(INC)/lpj.h $(INC)/manage.h $(INC)/managepar.h\
          $(INC)/numeric.h $(INC)/tree.h $(INC)/image.h $(INC)/biomes.h\
          $(INC)/output.h $(INC)/pft.h $(INC)/pftlist.h $(INC)/pftpar.h\
          $(INC)/soil.h $(INC)/soilpar.h $(INC)/stand.h $(INC)/swap.h\
          $(INC)/types.h $(INC)/units.h $(INC)/climbuf.h $(INC)/intlist.h\
          $(INC)/queue.h $(INC)/pnet.h $(INC)/channel.h $(INC)/woodplantation.h\
          $(INC)/natural.h $(INC)/grassland.h $(INC)/agriculture.h\
          $(INC)/reservoir.h $(INC)/spitfire.h $(INC)/biomass_tree.h\
          $(INC)/biomass_grass.h $(INC)/cdf.h $(INC)/outfile.h $(INC)/cpl.h\
          $(INC)/agriculture_tree.h $(INC)/agriculture_grass.h $(INC)/coupler.h\
          $(INC)/couplerpar.h

DATA    = par/*.cjson

JSON	= lpjml_config.cjson input.cjson input_netcdf.cjson

SCRIPTS	= configure.bat configure.sh\
          bin/output_bsq bin/lpjsubmit_aix bin/lpjsubmit_intel\
          bin/lpjsubmit_mpich bin/lpjrun bin/backtrace\
          bin/regridlpj bin/lpjsubmit_slurm

FILES	= Makefile config/* README AUTHORS INSTALL VERSION LICENSE STYLESHEET.md\
          $(JSON) $(DATA) $(HDRS) $(SCRIPTS)

main:
	$(MKDIR) lib
	(cd src && $(MAKE))

lpjcheck:
	$(MKDIR) lib
	(cd src && $(MAKE) libs)
	(cd src/utils && $(MAKE) ../../bin/lpjcheck)

utils:
	(cd src && $(MAKE) libs)
	(cd src/utils && $(MAKE) all)

all: main utils

install: all
	$(MKDIR) $(LPJROOT)/bin
	$(MKDIR) $(LPJROOT)/include
	$(MKDIR) $(LPJROOT)/par
	$(MKDIR) $(LPJROOT)/man/man1
	$(MKDIR) $(LPJROOT)/man/man5
	$(MKDIR) $(LPJROOT)/man/man3
	chmod 755 $(LPJROOT)
	chmod 755 $(LPJROOT)/bin
	chmod 755 $(LPJROOT)/include
	chmod 755 $(LPJROOT)/par
	chmod 755 $(LPJROOT)/man
	chmod 755 $(LPJROOT)/man/man1
	chmod 755 $(LPJROOT)/man/man5
	chmod 755 $(LPJROOT)/man/man3
	install bin/* $(LPJROOT)/bin
	install -m 644 $(HDRS) $(LPJROOT)/include
	install -m 644 $(DATA) $(LPJROOT)/par
	install -m 644 README INSTALL VERSION AUTHORS LICENSE COPYRIGHT CHANGELOG.md $(JSON) $(LPJROOT)
	install -m 644 man/whatis $(LPJROOT)/man
	install -m 644 man/man1/*.1 $(LPJROOT)/man/man1
	install -m 644 man/man5/*.5 $(LPJROOT)/man/man5
	install -m 644 man/man3/*.3 $(LPJROOT)/man/man3

test: main
	$(MKDIR) output
	$(MKDIR) restart

clean:
	(cd src  && $(MAKE) clean)

tar:
	tar -cf $(TARFILE) $(FILES) src/Makefile src/*.c\
	    src/climate/Makefile src/climate/*.c\
            man/man1/*.1 man/man3/*.3 man/man5/*.5 man/whatis\
            man/man1/Makefile man/man3/Makefile man/man5/Makefile man/Makefile\
	    src/crop/*.c src/crop/Makefile src/grass/*.c src/grass/Makefile\
	    src/image/Makefile src/image/*.c\
	    src/landuse/*.c src/landuse/Makefile src/lpj/*.c src/lpj/Makefile\
	    src/numeric/*.c src/numeric/Makefile src/soil/*.c src/soil/Makefile\
	    src/tools/*.c src/tools/Makefile src/tree/*.c src/tree/Makefile\
            src/lpj/FILES src/pnet/*.c src/pnet/FILES src/socket/Makefile\
            src/socket/*.c src/reservoir/Makefile\
            src/image/Makefile src/image/*.c src/reservoir/*.c\
            src/pnet/Makefile REFERENCES COPYRIGHT src/utils/*.c src/utils/Makefile\
            src/spitfire/Makefile src/spitfire/*.c src/netcdf/Makefile src/netcdf/*.c\
            src/cpl/Makefile src/cpl/*.c src/coupler/Makefile src/coupler/*.c
	    gzip -f $(TARFILE)

zipfile:
	zip -l $(ZIPFILE) $(FILES) src/Makefile src/*.c\
	    src/climate/Makefile src/climate/*.c config/* man/* man/man1/*.1\
            man/man3/*.3 man/man5/*.5\
	    src/crop/*.c src/crop/Makefile src/grass/*.c src/grass/Makefile\
	    src/image/Makefile src/image/*.c\
	    src/landuse/*.c src/landuse/Makefile src/lpj/*.c src/lpj/Makefile\
	    src/numeric/*.c src/numeric/Makefile src/soil/*.c src/soil/Makefile\
	    src/tools/*.c src/tools/Makefile src/tree/*.c src/tree/Makefile\
            src/lpj/FILES src/pnet/*.c src/pnet/FILES src/socket/Makefile\
            src/socket/*.c src/reservoir/Makefile\
            src/image/*.c src/image/Makefile src/reservoir/*.c\
            src/pnet/Makefile REFERENCES COPYRIGHT src/utils/*.c src/utils/Makefile\
            src/spitfire/Makefile src/spitfire/*.c src/netcdf/Makefile src/netcdf/*.c\
            src/cpl/Makefile src/cpl/*.c src/coupler/Makefile src/coupler/*.c
