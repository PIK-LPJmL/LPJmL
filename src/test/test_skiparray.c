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
#include "getfilesizep.h"
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
#include "bstruct_writedouble.h"
#include "bstruct_readfloat.h"
#include "bstruct_readdouble.h"
#include "bstruct_readbeginstruct.h"
#include "bstruct_writeendstruct.h"
#include "bstruct_readendstruct.h"
#include "bstruct_writebeginarray.h"
#include "bstruct_readbeginarray.h"
#include "bstruct_writeendarray.h"
#include "bstruct_readendarray.h"
#include "bstruct_skiparray.h"
#include "bstruct_finish.h"
#include "bstruct_writebeginstruct.h"
#include "bstruct_fprintnamestack.h"
#include "bstruct_readid.h"
#include "bstruct_readtoken.h"

void test_restart(void)
{
  char *filename;
  int i,size,value;
  Bool rc;
  float vec[10],vec2[5];
  Bstruct bstr;
  filename=tmpnam(NULL);
  bstr=bstruct_create(filename);
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_writeint(bstr,"a",1);
  bstruct_writebeginarray(bstr,"vec",10);
  for(i=0;i<10;i++)
  {
    vec[i]=i;
    bstruct_writefloat(bstr,NULL,vec[i]);
  }
  bstruct_writeendarray(bstr);
  bstruct_writeint(bstr,"b",2);
  bstruct_finish(bstr);
  bstr=bstruct_open(filename,TRUE);
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_readint(bstr,"a",&value);
  TEST_ASSERT_EQUAL_INT(1,value);
  rc=bstruct_readbeginarray(bstr,"vec",&size);
  TEST_ASSERT_EQUAL_INT(FALSE,rc);
  TEST_ASSERT_EQUAL_INT(10,size);
  /* reading the first 5 elements in array */
  for(i=0;i<5;i++)
  {
    rc=bstruct_readfloat(bstr,NULL,vec2+i);
    TEST_ASSERT_EQUAL_INT(FALSE,rc);
  }
  TEST_ASSERT_EQUAL_FLOAT_ARRAY(vec,vec2,5);
  rc=bstruct_skiparray(bstr);
  TEST_ASSERT_EQUAL_INT(FALSE,rc);
  rc=bstruct_readint(bstr,"b",&value);
  TEST_ASSERT_EQUAL_INT(FALSE,rc);
  TEST_ASSERT_EQUAL_INT(2,value);
  bstruct_finish(bstr);
  unlink(filename);
}
