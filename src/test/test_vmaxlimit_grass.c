#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lpj.h"
#include "grass.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "ndemand_grass.h"
#include "vmaxlimit_grass.h"

#define T 22.5

Param param;

void test_vmaxlimit_grass(void)
{
  Pft pft;
  Pftpar par;
  Pftgrasspar grasspar;
  Pftgrass grass;
  Real ndemand_leaf,ndemand,vmax,vmax2;
  param.p=25;      /* Haxeltine & Prentice regression coefficient */
  param.n0=7.15;   /* Haxeltine & Prentice regression coefficient */
  param.k=0.0693;  /* factor of temperature dependence of nitrogen demand for Rubisco activity */
  pft.par=&par;
  par.data=&grasspar;
  pft.data=&grass;
  par.ncleaf.low=7;
  par.ncleaf.high=20;
  grasspar.ratio=1.1;
  pft.bm_inc.carbon=20;
  pft.vmax=1000;
  grass.falloc.leaf=0.5;
  grass.falloc.root=0.5;
  pft.nind=1.0;
  grass.turn_litt.leaf.carbon=1.0;
  grass.turn_litt.leaf.nitrogen=0.1;
  grass.ind.leaf.carbon=10;
  grass.ind.leaf.nitrogen=1;
  grass.ind.root.carbon=10;
  grass.ind.root.nitrogen=1;
  grass.turn.leaf.carbon=2;
  for(vmax=1;vmax<10;vmax++)
  {
    ndemand=ndemand_grass(&pft,&pft.nleaf,vmax,T);
    vmax2=vmaxlimit_grass(&pft,T);
    TEST_ASSERT_EQUAL_FLOAT(vmax,vmax2);
  }
}
