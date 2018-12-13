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
  for(l=0;l<NSOILLAYER;l++)
    cell->output.mswc[l]*=ndaymonth1[month];
  cell->output.mrootmoist*=ndaymonth1[month];
  cell->output.mfiredi*=ndaymonth1[month];
  cell->output.mfapar*=ndaymonth1[month];
  cell->output.malbedo*=ndaymonth1[month];
  cell->output.mphen_tmin*=ndaymonth1[month];
  cell->output.mphen_tmax*=ndaymonth1[month];
  cell->output.mphen_light*=ndaymonth1[month];
  cell->output.mphen_water*=ndaymonth1[month];
  cell->output.mwscal*=ndaymonth1[month];
  cell->output.atransp+=cell->output.mtransp;
  cell->output.aevap+=cell->output.mevap;
  cell->output.ainterc+=cell->output.minterc;
  cell->output.airrig+=cell->output.mirrig;
  cell->output.aevap_lake+=cell->output.mevap_lake;
  cell->output.aevap_res+=cell->output.mevap_res;
  cell->output.mswe*=ndaymonth1[month];
  cell->output.aconv_loss_evap+=cell->output.mconv_loss_evap;
  cell->output.aconv_loss_drain+=cell->output.mconv_loss_drain;
  cell->output.aburntarea+=cell->output.mburntarea;
#ifdef IMAGE
  cell->ml.image_data->anpp+=cell->output.mnpp;
  cell->ml.image_data->arh+=cell->output.mrh;
#endif
  /* for carbon balance check  */
  cell->balance.nep+=cell->output.mnpp-cell->output.mrh;

  /* for water balance check */
  cell->balance.awater_flux+=cell->output.mrunoff+cell->output.mtransp+
    cell->output.mevap+cell->output.minterc+cell->output.mevap_lake+
    ((cell->discharge.mfout-cell->discharge.mfin)/cell->coord.area)-cell->output.mirrig;
  if(cell->ml.dam)
    cell->balance.awater_flux+=cell->output.mevap_res;

} /* of 'monthly_update' */
