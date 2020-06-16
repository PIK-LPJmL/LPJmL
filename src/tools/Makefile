#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of tools part of LPJmL                      ##
##   creates library ../../lib/libttools.a                                     ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = swap.$O freadheader.$O coord.$O openinputfile.$O\
          fscanreal.$O fail.$O list.$O headersize.$O\
          bigendian.$O fprintheader.$O sysname.$O queue.$O\
          openrestart.$O fbanner.$O getdir.$O freadanyheader.$O\
          getpath.$O getuser.$O gethost.$O failonerror.$O\
          addpath.$O frepeatch.$O isabspath.$O mpi_write.$O\
          printflags.$O getfilesize.$O enablefpe.$O\
          strippath.$O diskfree.$O fprintintf.$O freadrestartheader.$O\
          fwriteheader.$O getcounts.$O getfiledate.$O fscanint.$O\
          iserror.$O mpi_write_txt.$O mkfilename.$O stripsuffix.$O\
          hassuffix.$O checkfmt.$O findstr.$O fputstring.$O fscanfloat.$O\
          fprinttime.$O newmat.$O freemat.$O readrealvec.$O readfilename.$O\
          fscanuint.$O readintvec.$O readfloatvec.$O readuintvec.$O\
          catstrvec.$O strdate.$O openmetafile.$O fscansize.$O fscanfcns.$O\
          fscaninteof.$O fputprintable.$O fscanrealarray.$O fscanstruct.$O\
          fscanarray.$O fscanarrayindex.$O fscanbool.$O iskeydefined.$O\
          isboolean.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libtools.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h\
          $(INC)/climate.h $(INC)/date.h $(INC)/swap.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h\
          $(INC)/errmsg.h $(INC)/numeric.h $(INC)/channel.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h\
          $(INC)/list.h $(INC)/cell.h $(INC)/units.h\
          $(INC)/config.h $(INC)/queue.h $(INC)/output.h


$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

printflags.$O: printflags.c
	$(CC) $(CFLAGS) -DCFLAGS='"$(CFLAGS)"' -I$(INC) -c $*.c

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) $(JSON_INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJS) 
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
