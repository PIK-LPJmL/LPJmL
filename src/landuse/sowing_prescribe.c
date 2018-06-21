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
#include "crop.h"
#include "agriculture.h"

Real sowing_prescribe(Cell *cell,          /**< pointer to cell */
                      int day,             /**< day of year (1..365) */
                      int npft,            /**< number of natural PFTs */
                      int ncft,            /**< number of crop PFTs */
                      int year,            /**< simulation year (AD) */
                      const Config *config /**< LPJmL configuration */
                     )                     /** \return establishment flux (gC/m2) */
{
  Real flux_estab=0;
  Stand *setasidestand;
  Bool alloc_today_rf=FALSE, alloc_today_ir=FALSE,istimber;
  const Pftcroppar *croppar;
  int cft,s,s2;
  int earliest_sdate;
  Bool wtype=FALSE;

#ifdef IMAGE
  istimber=(config->start_imagecoupling!=INT_MAX);
#else
  istimber=FALSE;
#endif
  s=findlandusetype(cell->standlist,SETASIDE_RF);
  s2=findlandusetype(cell->standlist,SETASIDE_IR);
  if(s!=NOT_FOUND || s2!=NOT_FOUND)
  {

    for(cft=0;cft<ncft;cft++)
    {
      croppar=config->pftpar[npft+cft].data;
      earliest_sdate=(cell->coord.lat>=0) ? croppar->initdate.sdatenh : croppar->initdate.sdatesh;

      /*rainfed crops*/
      s=findlandusetype(cell->standlist,SETASIDE_RF);
      if(s!=NOT_FOUND)
      {
        setasidestand=getstand(cell->standlist,s);

        if(day==cell->ml.sdate_fixed[cft])
        {
          wtype = (croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE && day>earliest_sdate) ? TRUE : FALSE;
          if(check_lu(cell->standlist,cell->ml.landfrac[0].crop[cft],npft+cft,FALSE))
          {
            if(!alloc_today_rf)
            {
              allocation_today(setasidestand, config->ntypes);
              alloc_today_rf=TRUE;
            }
            flux_estab+=cultivate(cell,config->pftpar+npft+cft,
                                  cell->ml.cropdates[cft].vern_date20,
                                  cell->ml.landfrac[0].crop[cft],FALSE,day,wtype,
                                  setasidestand,istimber,config->irrig_scenario,
                                  npft+ncft,cft,year);
#ifndef DOUBLE_HARVEST
            cell->output.sdate[cft]=day;
#endif
          }
        }/*of rainfed CFTs*/
      }
      s=findlandusetype(cell->standlist,SETASIDE_IR);
      if(s!=NOT_FOUND)
      {
        setasidestand=getstand(cell->standlist,s);


        /*irrigated crops*/
        if(day==cell->ml.sdate_fixed[cft+ncft])
        {
          wtype = (croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE && day>earliest_sdate) ? TRUE : FALSE;
          if(check_lu(cell->standlist,cell->ml.landfrac[1].crop[cft],npft+cft,TRUE))
          {
            if(!alloc_today_ir)
            {
              allocation_today(setasidestand, config->ntypes);
              alloc_today_ir=TRUE;
            }
            flux_estab+=cultivate(cell,config->pftpar+npft+cft,
                                  cell->ml.cropdates[cft].vern_date20,
                                  cell->ml.landfrac[1].crop[cft],TRUE,day,wtype,
                                  setasidestand,istimber,config->irrig_scenario,
                                  npft+ncft,cft,year);
#ifndef DOUBLE_HARVEST
            cell->output.sdate[cft+ncft]=day;
#endif
          }
        }/*of irrigated CFTs*/
      }
    }
  }
  return flux_estab;
} /* of 'sowing_prescribe' */
