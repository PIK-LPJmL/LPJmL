/**************************************************************************************/
/**                                                                                \n**/
/**     u  p  d  a  t  e  l  i  t  t  e  r  p  r  o  p  e  r  t  i  e  s  .  c     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**************************************************************************************/

#include "lpj.h"

void updatelitterproperties(Stand *stand)       /* Stand pointer */
{
  int l;
  Real dm_sum=0;
  for(l=0;l<stand->soil.litter.n;l++)
    dm_sum+=stand->soil.litter.item[l].ag.leaf.carbon/0.42; /* Accounting that C content in plant dry matter is 42% (Brady and Weil 2008, p.504)*/
  if(dm_sum<0)
    dm_sum=0;

  stand->soil.litter.agtop_cover=1-exp(-6e-3*dm_sum);
  stand->soil.litter.agtop_wcap=2e-3*dm_sum;
  if((stand->soil.litter.agtop_moist-stand->soil.litter.agtop_wcap)>epsilon)
  {
    stand->cell->balance.totw-=(stand->soil.litter.agtop_moist-stand->soil.litter.agtop_wcap)*stand->frac;
    stand->cell->balance.soil_storage-=(stand->soil.litter.agtop_moist-stand->soil.litter.agtop_wcap)*stand->frac*stand->cell->coord.area;
    stand->soil.litter.agtop_moist=stand->soil.litter.agtop_wcap;
  }
} /* of 'updatelitterproperties' */

/*Brady, N.C., Weil, R.R., The nature and properties of soil, 2008, 14th edition, ISBN 0-13-227938-X*/
