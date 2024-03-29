#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile coupling LPJmL to COPAN                                          ##
##   creates library ../../lib/libcoupler.a                                    ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = receive_real_coupler.$O open_coupler.$O close_coupler.$O receive_coupler.$O\
          receive_real_scalar_coupler.$O send_real_scalar_coupler.$O send_token_coupler.$O\
          check_coupler.$O openinput_coupler.$O openoutput_coupler.$O\
          send_output_coupler.$O connect_coupler.$O receive_token_coupler.$O\
          receive_scalar_coupler.$O send_scalar_coupler.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libcoupler.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/manage.h $(INC)/climate.h $(INC)/date.h\
          $(INC)/pft.h $(INC)/climbuf.h $(INC)/coupler.h $(INC)/biomes.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/crop.h $(INC)/errmsg.h\
          $(INC)/numeric.h $(INC)/header.h $(INC)/landuse.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/managepar.h\
          $(INC)/stand.h $(INC)/discharge.h $(INC)/queue.h $(INC)/intlist.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/output.h\
          $(INC)/config.h $(INC)/pnet.h $(INC)/channel.h $(INC)/reservoir.h\
          $(INC)/spitfire.h

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O:
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean:
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
