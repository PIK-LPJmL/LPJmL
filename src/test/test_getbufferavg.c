#include <stdlib.h>
#include <stdio.h>
#include "lpj.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "support_fail_stub.h"
#include "buffer.h"
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
#include "bstruct_writereal.h"
#include "bstruct_writerealarray.h"
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

#define BUFFERSIZE 10

void test_getbufferavg(void)
{
  Buffer buffer;
  Real avg;
  int i;
  buffer=newbuffer(BUFFERSIZE);
  TEST_ASSERT_NOT_NULL(buffer);
  for(i=0;i<BUFFERSIZE;i++)
    updatebuffer(buffer,10.0);
  for(i=0;i<BUFFERSIZE/2;i++)
    updatebuffer(buffer,20.0);
  avg=getbufferavg(buffer);
  TEST_ASSERT_EQUAL_FLOAT((10+20)*0.5,avg);
  freebuffer(buffer);
}
