#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lpj.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "support_fail_stub.h"
#include "list.h"
#include "hash.h"
#include "swap.h"
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
#include "bstruct_writefloat.h"
#include "bstruct_readfloat.h"
#include "bstruct_writebeginarray.h"
#include "bstruct_readbeginarray.h"
#include "bstruct_writeendarray.h"
#include "bstruct_readendarray.h"
#include "bstruct_finish.h"
#include "bstruct_fprintnamestack.h"
#include "bstruct_readtoken.h"
#include "bstruct_writebeginindexarray.h"
#include "bstruct_getarrayindex.h"
#include "bstruct_seekindexarray.h"
#include "bstruct_writearrayindex.h"

#define N 10

void test_restart(void)
{
  Bstruct bstr;
  long long pos[N],filepos;
  float vec[N],vec5=0;
  int i,size;
  bstr=bstruct_create("test.lpj");
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_writebeginindexarray(bstr,"vec",&filepos,N); 
  for(i=0;i<N;i++)
  {
    vec[i]=i;
    pos[i]=bstruct_getarrayindex(bstr);
    bstruct_writefloat(bstr,NULL,vec[i]);
  }
  bstruct_writearrayindex(bstr,filepos,pos,0,N);
  bstruct_writeendarray(bstr);
  bstruct_finish(bstr);
  bstr=bstruct_open("test.lpj",TRUE);
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_readbeginarray(bstr,"vec",&size);
  TEST_ASSERT_EQUAL_INT(N,size);
  bstruct_seekindexarray(bstr,5,N); 
  bstruct_readfloat(bstr,NULL,&vec5);
  TEST_ASSERT_EQUAL_FLOAT(vec[5],vec5);
  bstruct_finish(bstr);
  unlink("test.lpj");
}
