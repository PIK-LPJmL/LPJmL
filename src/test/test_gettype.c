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
#include "bstruct_skiparrayelements.h"
#include "bstruct_findobject.h"
#include "bstruct_wopen.h"
#include "bstruct_open.h"
#include "bstruct_writeint.h"
#include "bstruct_writename.h"
#include "bstruct_writestring.h"
#include "bstruct_readint.h"
#include "bstruct_writefloat.h"
#include "bstruct_readfloat.h"
#include "bstruct_readbeginstruct.h"
#include "bstruct_writeendstruct.h"
#include "bstruct_readendstruct.h"
#include "bstruct_finish.h"
#include "bstruct_writebeginstruct.h"
#include "bstruct_fprintnamestack.h"
#include "bstruct_readid.h"
#include "bstruct_readtoken.h"
#include "bstruct_gettype.h"

void test_gettype(void)
{
  char *filename;
  struct
  {
    int a;
    float b;
    char *name;
    struct
    {
      float c;
      float d;
    } s;
  } data1={1,2.0,"name",{3.0,4.0}},data2={};
  Bstruct bstr;
  int type;
  /* creating test bstruct file */
  filename=tmpnam(NULL);
  bstr=bstruct_create(filename);
  TEST_ASSERT_NOT_NULL(bstr);
  bstruct_writeint(bstr,"a",data1.a);
  bstruct_writefloat(bstr,"b",data1.b);
  bstruct_writestring(bstr,"name",data1.name);
  bstruct_writebeginstruct(bstr,"s");
  bstruct_writefloat(bstr,"c",data1.s.c);
  bstruct_writefloat(bstr,"d",data1.s.d);
  bstruct_writeendstruct(bstr);
  bstruct_finish(bstr);
  /* opening test bstruct file for reading */
  bstr=bstruct_open(filename,TRUE);
  TEST_ASSERT_NOT_NULL(bstr);
  /* check for type of struct "s" */
  type=bstruct_gettype(bstr,"s");
  TEST_ASSERT_EQUAL_INT(BSTRUCT_STRUCT,type);
  /* check for type of string "name" */
  type=bstruct_gettype(bstr,"name");
  TEST_ASSERT_EQUAL_INT(BSTRUCT_STRING,type);
  bstruct_readbeginstruct(bstr,"s");
  /* check for type of float "c" */
  type=bstruct_gettype(bstr,"c");
  TEST_ASSERT_EQUAL_INT(BSTRUCT_FLOAT,type);
  bstruct_readendstruct(bstr,"s");
  /* check for type of non-existing object "d" */
  type=bstruct_gettype(bstr,"d");
  TEST_ASSERT_EQUAL_INT(BSTRUCT_NOTFOUND,type);
  bstruct_finish(bstr);
  unlink(filename);
}
