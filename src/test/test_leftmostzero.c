#include <stdlib.h>
#include <stdio.h>
#include "lpj.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "support_fail_stub.h"
#include "bisect.h"
#include "leftmostzero.h"

static Real fcn(Real x,void *data)
{
  return (x-1)*(x+1)*x;
} 

void test_leftmostzero(void)
{
  Real yzero;
  yzero=leftmostzero(fcn,-10,10,NULL,1e-5,1e-5,1000);
  TEST_ASSERT_EQUAL_FLOAT(-1,yzero);
}
