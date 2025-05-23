#################################################################################
##                                                                             ##
##               M  a  k  e  f  i  l  e                                        ##
##                                                                             ##
##   Makefile for C implementation of reading/writing restart files in LPJmL   ##
##   creates library ../../lib/libbstruct.a                                    ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

include ../../Makefile.inc

OBJS    = bstruct_append.$O bstruct_finish.$O bstruct_create.$O bstruct_fprintdata.$O\
          bstruct_fprintnametable.$O bstruct_freedata.$O bstruct_freehash.$O\
          bstruct_getarrayindex.$O bstruct_getfile.$O bstruct_gethash.$O\
          bstruct_getmiss.$O bstruct_intern.$O bstruct_isdefined.$O bstruct_open.$O\
          bstruct_readbeginarray.$O bstruct_readbool.$O bstruct_readbyte.$O\
          bstruct_readdata.$O bstruct_readdouble.$O bstruct_readendarray.$O\
          bstruct_readendstruct.$O bstruct_readfloatarray.$O bstruct_readfloat.$O\
          bstruct_readindexarray.$O bstruct_readintarray.$O bstruct_readint.$O\
          bstruct_readrealarray.$O bstruct_readreal.$O bstruct_readshortarray.$O\
          bstruct_readshort.$O bstruct_readstring.$O bstruct_readbeginstruct.$O\
          bstruct_readushortarray.$O bstruct_readushort.$O bstruct_readvarrealarray.$O\
          bstruct_seekindexarray.$O bstruct_setout.$O bstruct_sync.$O bstruct_writebeginarray.$O\
          bstruct_writearrayindex.$O bstruct_writebool.$O bstruct_writebyte.$O\
          bstruct_writedouble.$O bstruct_writeendarray.$O bstruct_writeendstruct.$O\
          bstruct_writefloatarray.$O bstruct_writefloat.$O bstruct_writebeginindexarray.$O\
          bstruct_writeintarray.$O bstruct_writeint.$O bstruct_writerealarray.$O\
          bstruct_writereal.$O bstruct_writeshortarray.$O bstruct_writeshort.$O\
          bstruct_writestring.$O bstruct_writebeginstruct.$O bstruct_writeushortarray.$O\
          bstruct_writeushort.$O bstruct_writename.$O bstruct_readtoken.$O\
          bstruct_readdoublearray.$O bstruct_writedoublearray.$O bstruct_findobject.$O\
          bstruct_skipdata.$O bstruct_fprintnamestack.$O

INC     = ../../include
LIBDIR  = ../../lib
LIB     = libbstruct.$A

HDRS    = $(INC)/types.h $(INC)/swap.h $(INC)/header.h $(INC)/errmsg.h\
          $(INC)/list.h $(INC)/bstruct.h $(INC)/hash.h bstruct_intern.h

$(LIBDIR)/$(LIB): $(OBJS)
	$(AR) $(ARFLAGS)$(LIBDIR)/$(LIB) $(OBJS)

$(OBJS): $(HDRS)

.c.$O:
	$(CC) $(CFLAGS) -I$(INC) $(JSON_INC) -c $*.c

clean:
	$(RM) $(RMFLAGS) $(OBJS)
	(cd $(LIBDIR) && $(RM) $(RMFLAGS) $(LIB))
