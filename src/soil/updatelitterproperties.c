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
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void updatelitterproperties(Stand *stand,  /**< Stand pointer */
                            Real standfrac /**< stand fraction (0..1) */
                           )
{
  Real dm_sum = calc_litter_dm_sum(&stand->soil);
  stand->soil.litter.agtop_cover=1-exp(-6e-3*dm_sum);
  stand->soil.litter.agtop_wcap=2e-3*dm_sum;
  if((stand->soil.litter.agtop_moist-stand->soil.litter.agtop_wcap)>epsilon*1e-3)
  {
    //stand->cell->balance.excess_water+=(stand->soil.litter.agtop_moist-stand->soil.litter.agtop_wcap)*standfrac;
    stand->soil.w_fw[0]+=(stand->soil.litter.agtop_moist-stand->soil.litter.agtop_wcap);
    stand->soil.litter.agtop_moist=stand->soil.litter.agtop_wcap;
  }
} /* of 'updatelitterproperties' */


Real calc_litter_dm_sum(Soil *soil) {
  int l;
  Real dm_sum=0;
  for(l=0;l<soil->litter.n;l++){
    dm_sum+=soil->litter.item[l].agtop.leaf.carbon/0.42; /* Accounting that C content in plant dry matter is 42% (Brady and Weil 2008, p.504)*/
  }
  if(dm_sum<0)
    dm_sum=0;
  return(dm_sum);
} /* of 'calc_litter_dm_sum' */

/*Brady, N.C., Weil, R.R., The nature and properties of soil, 2008, 14th edition, ISBN 0-13-227938-X*/
