/**************************************************************************************/
/**                                                                                \n**/
/**              s  o  i  l  p  a  r  _  o  u  t  p  u  t  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function updates soil parameter outputs                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void soilpar_output(Cell *cell,          /**< pointer to cell */
                    const Config *config /**< LPJmL configuration */
                   )
{
  Stand *stand;
  int s,l;
  Real whc[NSOILLAYER];
  Real frac_agr;
  frac_agr=0;
  foreachsoillayer(l)
    whc[l]=0; 
  foreachstand(stand,s,cell->standlist)
    switch(stand->type->landusetype)
    {
      case NATURAL:
        foreachsoillayer(l)
          getoutputindex(&cell->output,WHC_NAT,l,config)+=stand->soil.whc[l];
        break;
      case GRASSLAND:
        foreachsoillayer(l)
          getoutputindex(&cell->output,WHC_GRASS,l,config)+=stand->soil.whc[l];
        break;
      case AGRICULTURE:
        foreachsoillayer(l)
          whc[l]+=stand->soil.whc[l]*stand->frac;
        frac_agr+=stand->frac;
        break;
    }
  if(frac_agr>0)
     foreachsoillayer(l)
        getoutputindex(&cell->output,WHC_AGR,l,config)+=whc[l]/frac_agr;
} /* of 'soilpar_output' */
