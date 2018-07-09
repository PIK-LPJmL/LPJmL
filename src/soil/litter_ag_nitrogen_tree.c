/***************************************************************************/
/**                                                                       **/
/**             l i t t e r _ a g _ n i t r o g e n _ t r e e . c         **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function computes sum of all above-ground litter pools for trees  **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2014-06-23 09:39:33 +0200 (Mon, 23 Jun 201#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

Real litter_ag_nitrogen_tree(const Litter *litter,int fuel)
{
  int l;
  Real sum;
  sum=0;
  if(fuel==0)
  {
    for(l=0;l<litter->n;l++)
      if(litter->ag[l].pft->type==TREE)
        sum+=litter->ag[l].trait.leaf.nitrogen+litter->ag[l].trait.wood[0].nitrogen;
  }
  else
  {
    for(l=0;l<litter->n;l++)
      if(litter->ag[l].pft->type==TREE)
        sum+=litter->ag[l].trait.wood[fuel].nitrogen;
  }
  return sum;
} /* of litter_ag_nitrogen_tree */
