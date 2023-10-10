#include "lpj.h"
#include "realdata.h"

Real getrealdata(const Realdata realdata,int cell)
{
  return realdata->data[cell];
}  /* of 'getrealdata' */
