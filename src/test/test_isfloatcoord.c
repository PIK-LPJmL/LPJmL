#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lpj.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "support_fail_stub.h"
#include "types.h"
#include "list.h"
#include "hash.h"
#include "swap.h"
#include "coord.h"
#include "freadheader.h"
#include "openmetafile.h"
#include "getfilesizep.h"
#include "closeconfig.h"
#include "parse_json.h"
#include "fscanfcns.h"
#include "fscanfloat.h"
#include "fscanreal.h"
#include "fscanbool.h"
#include "fscankeywords.h"
#include "fscanstruct.h"
#include "fscantimestep.h"
#include "fscanmap.h"
#include "fscanattrs.h"
#include "fscanint.h"
#include "fscansize.h"
#include "frepeatch.h"
#include "fputprintable.h"
#include "iskeydefined.h"
#include "getpath.h"
#include "addpath.h"
#include "isabspath.h"
#include "findstr.h"
#include "isstring.h"
#include "bigendian.h"
#include "readfilename.h"
#include "headersize.h"
#include "freadtopheader.h"
#include "fwritetopheader.h"
#include "fputprintable.h"
#include "bstruct_intern.h"
#include "bstruct_skipdata.h"
#include "bstruct_findobject.h"
#include "bstruct_wopen.h"
#include "bstruct_open.h"
#include "bstruct_writeint.h"
#include "bstruct_writename.h"
#include "bstruct_readint.h"
#include "bstruct_writereal.h"
#include "bstruct_readreal.h"
#include "bstruct_readbeginstruct.h"
#include "bstruct_writeendstruct.h"
#include "bstruct_readendstruct.h"
#include "bstruct_writebeginarray.h"
#include "bstruct_readbeginarray.h"
#include "bstruct_writeendarray.h"
#include "bstruct_readendarray.h"
#include "bstruct_finish.h"
#include "bstruct_writebeginstruct.h"
#include "bstruct_fprintnamestack.h"
#include "bstruct_readid.h"
#include "bstruct_readtoken.h"

void test_isfloatcoord(void)
{
  Real coord,scale;
  Bool rc;
  coord=0.5;
  scale=0.01;
  rc=isfloatcoord(coord*0.5,scale);
  TEST_ASSERT_EQUAL_INT(FALSE,rc);
  coord=0.25;
  scale=0.01;
  rc=isfloatcoord(coord*0.5,scale);
  TEST_ASSERT_EQUAL_INT(TRUE,rc);
  coord=0.1;
  scale=0.01;
  rc=isfloatcoord(coord*0.5,scale);
  TEST_ASSERT_EQUAL_INT(FALSE,rc);
  coord=0.0833;
  scale=0.01;
  rc=isfloatcoord(coord*0.5,scale);
  TEST_ASSERT_EQUAL_INT(TRUE,rc);
}
