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
#include "bstruct_fprintnamestack.h"
#include "bstruct_wopen.h"
#include "bstruct_open.h"
#include "bstruct_writename.h"
#include "bstruct_writefloat.h"
#include "bstruct_readfloat.h"
#include "bstruct_finish.h"
#include "bstruct_readid.h"
#include "bstruct_readtoken.h"

void test_restart(void)
{
  float a1=1,b1=10,a2=0,b2=0;
  Bstruct bstr;
  bstr=bstruct_create("test.lpj");
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_writefloat(bstr,"a",a1);
  bstruct_finish(bstr);
  bstr=bstruct_append("test.lpj",TRUE);
  bstruct_writefloat(bstr,"b",b1);
  bstruct_finish(bstr);
  bstr=bstruct_open("test.lpj",TRUE);
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_readfloat(bstr,"a",&a2);
  bstruct_readfloat(bstr,"b",&b2);
  bstruct_finish(bstr);
  TEST_ASSERT_EQUAL_FLOAT(a1,a2);
  TEST_ASSERT_EQUAL_FLOAT(b1,b2);
  unlink("test.lpj");
}
