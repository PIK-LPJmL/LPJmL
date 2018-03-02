#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of socket libary                            ##
##   creates library ../../lib/libsocket.a                                     ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://gitlab.pik-potsdam.de/lpjml                                ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

LIBDIR  = ../../lib
INC     = ../../include
LIB     = libsocket.$A

OBJS    = open_socket.$O connect_socket.$O read_socket.$O\
          readint_socket.$O readshort_socket.$O readfloat_socket.$O\
          write_socket.$O close_socket.$O readdouble_socket.$O\
          getclientname.$O writestring_socket.$O readstring_socket.$O\
          fopen_socket.$O fconnect_socket.$O freadint_socket.$O\
          freadshort_socket.$O fwriteint_socket.$O fwriteshort_socket.$O\
          fclose_socket.$O freaddouble_socket.$O fwritedouble_socket.$O\
          fwritefloat_socket.$O freadfloat_socket.$O opentdt_socket.$O\
          connecttdt_socket.$O fopentdt_socket.$O fconnecttdt_socket.$O\
          readlong_socket.$O freadlong_socket.$O fwritelong_socket.$O\
          fmpi_read_socket.$O\
          fmpi_write_socket.$O freadstring_socket.$O fwritestring_socket.$O\
          fgetclientname.$O mpi_read_socket.$O mpi_write_socket.$O

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(INC)/channel.h $(INC)/types.h $(INC)/swap.h

.c.$O:
	$(CC) -c $(CFLAGS) -I$(INC) $(JSON_INC) $*.c

clean:
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
