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
                    Real frac_agr,       /**< agricultural fraction (0..1) */
                    const Config *config /**< LPJmL configuration */
                   )
{
  Stand *stand;
  int s,l;
  foreachstand(stand,s,cell->standlist)
    if(isagriculture(stand->type->landusetype))
      foreachsoillayer(l)
      {
        getoutputindex(&cell->output,WHC_AGR,l,config)+=stand->soil.whc[l]*stand->frac/frac_agr;
        getoutputindex(&cell->output,KS_AGR,l,config)+=stand->soil.Ks[l]*stand->frac/frac_agr;
      }
    else
      switch(stand->type->landusetype)
      {
        case NATURAL:
          foreachsoillayer(l)
          {
            getoutputindex(&cell->output,WHC_NAT,l,config)+=stand->soil.whc[l];
            getoutputindex(&cell->output,KS_NAT,l,config)+=stand->soil.Ks[l];
          }
          break;
        case GRASSLAND:
          foreachsoillayer(l)
          {
            getoutputindex(&cell->output,WHC_GRASS,l,config)+=stand->soil.whc[l];
            getoutputindex(&cell->output,KS_GRASS,l,config)+=stand->soil.Ks[l];
          }
          break;
    }
} /* of 'soilpar_output' */
