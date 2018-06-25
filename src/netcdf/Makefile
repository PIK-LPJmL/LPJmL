#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of NetCDF part of LPJmL                     ##
##   creates library ../../lib/libcdf.a                                        ##
##   Needs NetCDF and Udunits library to be installed                          ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = create_netcdf.$O close_netcdf.$O write_float_netcdf.$O\
          createcoord.$O mpi_write_netcdf.$O write_short_netcdf.$O\
          create_pft_netcdf.$O mpi_write_pft_netcdf.$O getlatlon_netcdf.$O\
          write_pft_float_netcdf.$O createcoord_all.$O freecoordarray.$O\
          write_pft_short_netcdf.$O openclimate_netcdf.$O createindex.$O\
          closeclimate_netcdf.$O readclimate_netcdf.$O coord_netcdf.$O\
          opendata_netcdf.$O readdata_netcdf.$O input_netcdf.$O\
          getvar_netcdf.$O readintdata_netcdf.$O readshortdata_netcdf.$O\
          write_int_netcdf.$O mpi_openclimate_netcdf.$O open_netcdf.$O\
          create1_netcdf.$O create1_pft_netcdf.$O openfile_netcdf.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libcdf.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h\
          $(INC)/climate.h $(INC)/date.h $(INC)/swap.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h $(INC)/pftlist.h\
          $(INC)/errmsg.h $(INC)/numeric.h $(INC)/channel.h $(INC)/stand.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/reservoir.h\
          $(INC)/list.h $(INC)/cell.h $(INC)/units.h $(INC)/climbuf.h\
          $(INC)/config.h $(INC)/queue.h $(INC)/output.h $(INC)/spitfire.h\
          $(INC)/cdf.h $(INC)/soil.h $(INC)/landuse.h $(INC)/discharge.h


$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) $(NETCDF_INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS) 
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
