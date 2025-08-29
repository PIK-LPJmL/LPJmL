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
#include "bstruct_finish.h"
#include "bstruct_fprintnamestack.h"
#include "bstruct_readid.h"
#include "bstruct_readtoken.h"

#define N USHRT_MAX

void test_writename(void)
{
  String s;
  int i,value;
  Bool rc;
  Bstruct bstr;
  bstr=bstruct_create("test.lpj");
  TEST_ASSERT_NOT_NULL(bstr);
  for(i=0;i<=N;i++)
  {
    snprintf(s,STRING_LEN,"%d",i);
    rc=bstruct_writeint(bstr,s,i);
    TEST_ASSERT_EQUAL_INT(FALSE,rc);
  }
  rc=bstruct_finish(bstr);
  TEST_ASSERT_EQUAL_INT(FALSE,rc);
  bstr=bstruct_open("test.lpj",TRUE);
  TEST_ASSERT_NOT_NULL(bstr);
  for(i=0;i<=N;i++)
  {
    snprintf(s,STRING_LEN,"%d",i);
    rc=bstruct_readint(bstr,s,&value);
    TEST_ASSERT_EQUAL_INT(FALSE,rc);
    TEST_ASSERT_EQUAL_INT(i,value);
  }
  bstruct_finish(bstr);
  unlink("test.lpj");
}
