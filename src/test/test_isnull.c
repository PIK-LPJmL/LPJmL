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
#include "bstruct_readendstruct.h"
#include "bstruct_finish.h"
#include "bstruct_fprintnamestack.h"
#include "bstruct_readtoken.h"
#include "bstruct_writenull.h"
#include "bstruct_isnull.h"

void test_isnull(void)
{
  int i,size;
  int a1=1,a2=0;
  Bool rc;
  float b1=2,b2=0;
  Bstruct bstr;
  bstr=bstruct_create("test.lpj");
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_writeint(bstr,"a",a1);
  bstruct_writenull(bstr,"c");
  bstruct_writefloat(bstr,"b",b1);
  bstruct_finish(bstr);
  bstr=bstruct_open("test.lpj",0);
  TEST_ASSERT_NOT_NULL(bstr);
  rc=bstruct_isnull(bstr,"a");
  TEST_ASSERT_EQUAL_INT(rc,FALSE);
  bstruct_readint(bstr,"a",&a2);
  TEST_ASSERT_EQUAL_INT(a1,a2);
  bstruct_readfloat(bstr,"b",&b2);
  TEST_ASSERT_EQUAL_FLOAT(b1,b2);
  rc=bstruct_isnull(bstr,"c");
  TEST_ASSERT_EQUAL_INT(TRUE,rc);
  bstruct_finish(bstr);
  unlink("test.lpj");
}
