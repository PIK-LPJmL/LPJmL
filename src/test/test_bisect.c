#include <stdlib.h>
#include <stdio.h>
#include "lpj.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "support_fail_stub.h"
#include "bisect.h"

static Real fcn(Real x,void *data)
{
  return x*x-*((Real *)data);
} /* of 'fcn' */

void test_bisect(void)
{
  Real yzero,data;
  int it;
  data=2;
  yzero=bisect(fcn,0,10,&data,1e-5,1e-5,1000,&it);
  TEST_ASSERT_EQUAL_FLOAT(sqrt(data),yzero);
}
