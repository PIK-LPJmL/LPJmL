#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lpj.h"
#include "crop.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "ndemand_crop.h"
#include "vmaxlimit_crop.h"

#define T 22.5

Param param;

void test_vmaxlimit_crop(void)
{
  Pft pft;
  Pftpar par;
  Pftcroppar croppar;
  Pftcrop crop;
  Real ndemand_leaf,ndemand,vmax,vmax2;
  param.p=25;      /* Haxeltine & Prentice regression coefficient */
  param.n0=7.15;   /* Haxeltine & Prentice regression coefficient */
  param.k=0.0693;  /* factor of temperature dependence of nitrogen demand for Rubisco activity */
  pft.par=&par;
  par.data=&croppar;
  pft.data=&crop;
  par.ncleaf.low=7;
  par.ncleaf.high=20;
  croppar.ratio.root=1.1;
  croppar.ratio.so=0.83;
  croppar.ratio.pool=3;
  pft.bm_inc.carbon=20;
  pft.vmax=1000;
  pft.nind=1.0;
  crop.ind.leaf.carbon=10;
  crop.ind.leaf.nitrogen=1;
  crop.ind.root.carbon=10;
  crop.ind.root.nitrogen=1;
  crop.ind.so.carbon=10;
  crop.ind.so.nitrogen=1;
  crop.ind.pool.carbon=10;
  crop.ind.pool.nitrogen=1;
  for(vmax=1;vmax<10;vmax++)
  {
    ndemand=ndemand_crop(&pft,&crop.ind.leaf.nitrogen,vmax,T);
    vmax2=vmaxlimit_crop(&pft,T);
    TEST_ASSERT_EQUAL_FLOAT(vmax,vmax2);
  }
}
