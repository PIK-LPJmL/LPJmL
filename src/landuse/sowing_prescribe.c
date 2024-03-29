/**************************************************************************************/
/**                                                                                \n**/
/**            s  o  w  i  n  g  _  p  r  e  s  c  r  i  b  e  .  c                \n**/
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
#include "agriculture.h"
#include "crop.h"

Stocks sowing_prescribe(Cell *cell,          /**< pointer to cell */
                        int day,             /**< day of year (1..365) */
                        int npft,            /**< number of natural PFTs */
                        int ncft,            /**< number of crop PFTs */
                        int year,            /**< simulation year (AD) */
                        const Config *config /**< LPJmL configuration */
                       )                     /** \return establishment flux (gC/m2, gN/m2) */
{
  Stocks flux_estab={0,0};
  Bool alloc_today[2]={FALSE,FALSE};
  const Pftcroppar *croppar;
  int cft,s,s2;
  int earliest_sdate;
  int cft_other;
  Bool wtype;

  s=findlandusetype(cell->standlist,SETASIDE_RF);
  s2=findlandusetype(cell->standlist,SETASIDE_IR);
  if (s!=NOT_FOUND||s2!=NOT_FOUND)
  {
    if(config->others_to_crop)
       cft_other=(fabs(cell->coord.lat)>30) ? config->cft_temp : config->cft_tropic;
    else
       cft_other=-1;
    for (cft=0; cft<ncft; cft++)
    {
      croppar=config->pftpar[npft+cft].data;
      earliest_sdate=(cell->coord.lat>=0)?croppar->initdate.sdatenh:croppar->initdate.sdatesh;
      wtype = (croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE && day>earliest_sdate);
      if(day==cell->ml.sdate_fixed[cft])
      {
        sowingcft(&flux_estab,alloc_today,cell,FALSE,wtype,TRUE,npft,ncft,cft,year,day,FALSE,config);
        if(cft==cft_other)
          sowingcft(&flux_estab,alloc_today,cell,FALSE,wtype,TRUE,npft,ncft,cft,year,day,TRUE,config);
      }
      if(day==cell->ml.sdate_fixed[cft+ncft])
      {
        sowingcft(&flux_estab,alloc_today+1,cell,TRUE,wtype,TRUE,npft,ncft,cft,year,day,FALSE,config);
        if(cft==cft_other)
          sowingcft(&flux_estab,alloc_today+1,cell,TRUE,wtype,TRUE,npft,ncft,cft,year,day,TRUE,config);
      }
    }
  }
  return flux_estab;
} /* of 'sowing_prescribe' */
