/**************************************************************************************/
/**                                                                                \n**/
/**              u  p  d  a  t  e  _  m  o  n  t  h  l  y  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     month                                                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void update_monthly(Cell *cell,  /**< Pointer to cell */
                    Real mtemp,  /**< monthly average temperature (deg C) */
                    Real mprec,  /**< monthly average precipitation (mm) */
                    int month    /**< month (0..11) */
                   )
{
  int p;
  Pft *pft;
  int s,l;
  Stand *stand;

  monthly_climbuf(&cell->climbuf,mtemp,mprec,cell->output.mpet,month);
  if(cell->ml.dam) /* to store the monthly inflow and demand */
    update_reservoir_monthly(cell,month);
  foreachstand(stand,s,cell->standlist)
  {
    getlag(&stand->soil,month);
    foreachpft(pft,p,&stand->pftlist)
      turnover_monthly(&stand->soil.litter,pft);
  } /* of foreachstand */
#ifdef IMAGE
  cell->ml.image_data->anpp+=cell->output.npp;
  cell->ml.image_data->arh+=cell->output.mrh;
#endif
  /* for nitrogen balance check */
  /* for water balance check */
  cell->balance.awater_flux+=((cell->discharge.mfout-cell->discharge.mfin)/cell->coord.area);

} /* of 'monthly_update' */
