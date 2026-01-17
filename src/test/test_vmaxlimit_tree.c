#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lpj.h"
#include "tree.h"
#include "unity.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */

#include "ndemand_tree.h"
#include "vmaxlimit_tree.h"

#define T 22.5

Param param;

void test_vmaxlimit_tree(void)
{
  Pft pft;
  Pftpar par;
  Pfttreepar treepar;
  Pfttree tree;
  Real ndemand_leaf,ndemand,vmax,vmax2;
  param.p=25;      /* Haxeltine & Prentice regression coefficient */
  param.n0=7.15;   /* Haxeltine & Prentice regression coefficient */
  param.k=0.0693;  /* factor of temperature dependence of nitrogen demand for Rubisco activity */
  pft.par=&par;
  par.data=&treepar;
  pft.data=&tree;
  par.ncleaf.low=7;
  par.ncleaf.high=20;
  treepar.ratio.root=1.1;
  treepar.ratio.sapwood=1.16;
  pft.bm_inc.carbon=20;
  pft.vmax=1000;
  tree.falloc.leaf=0.5;
  tree.falloc.root=0.3;
  tree.falloc.sapwood=0.2;
  pft.nind=0.2;
  tree.excess_carbon=0;
  tree.turn_litt.leaf.carbon=1.0;
  tree.turn_litt.leaf.nitrogen=0.1;
  tree.ind.leaf.carbon=10;
  tree.ind.leaf.nitrogen=1;
  tree.ind.root.carbon=10;
  tree.ind.root.nitrogen=1;
  tree.ind.sapwood.carbon=10;
  tree.ind.sapwood.nitrogen=1;
  tree.turn.leaf.carbon=2;
  for(vmax=1;vmax<10;vmax++)
  {
    ndemand=ndemand_tree(&pft,&pft.nleaf,vmax,T);
    vmax2=vmaxlimit_tree(&pft,T);
    TEST_ASSERT_EQUAL_FLOAT(vmax,vmax2);
  }
}
