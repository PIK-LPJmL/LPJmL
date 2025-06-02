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
#include "bstruct_readbeginstruct.h"
#include "bstruct_writeendstruct.h"
#include "bstruct_readendstruct.h"
#include "bstruct_finish.h"
#include "bstruct_writebeginstruct.h"
#include "bstruct_fprintnamestack.h"
#include "bstruct_readtoken.h"
#include "bstruct_isdefined.h"

void test_isdefined(void)
{
  Bool rc;
  struct
  {
    int a;
    float b;
    struct
    {
      float c;
      float d;
    } s;
  } data1={1,2.0,{3.0,4.0}},data2={};
  Bstruct bstr;
  bstr=bstruct_create("test.lpj");
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_writeint(bstr,"a",data1.a);
  bstruct_writefloat(bstr,"b",data1.b);
  bstruct_writebeginstruct(bstr,"s");
  bstruct_writefloat(bstr,"c",data1.s.c);
  bstruct_writefloat(bstr,"d",data1.s.d);
  bstruct_writeendstruct(bstr);
  bstruct_finish(bstr);
  bstruct_open("test.lpj",TRUE);
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_readint(bstr,"a",&data2.a);
  TEST_ASSERT_EQUAL_INT(data1.a,data2.a);
  bstruct_readbeginstruct(bstr,"s");
  bstruct_readfloat(bstr,"c",&data2.s.c);
  TEST_ASSERT_EQUAL_FLOAT(data1.s.c,data2.s.c);
  bstruct_readfloat(bstr,"d",&data2.s.d);
  TEST_ASSERT_EQUAL_FLOAT(data1.s.d,data2.s.d);
  rc=bstruct_isdefined(bstr,"c");
  TEST_ASSERT_EQUAL_INT(TRUE,rc);
  bstruct_readendstruct(bstr,"s");
  bstruct_readfloat(bstr,"b",&data2.b);
  TEST_ASSERT_EQUAL_FLOAT(data1.b,data2.b);
  rc=bstruct_isdefined(bstr,"d");
  TEST_ASSERT_EQUAL_INT(FALSE,rc);
  bstruct_finish(bstr);
  unlink("test.lpj");
}
