/**************************************************************************************/
/**                                                                                \n**/
/**             p  h  e  n  _  v  a  r  i  e  t  y  .  c                           \n**/
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

#define KEYDAY_NHEMISPHERE 365 /* last day of driest month (Dec) of northern hemisphere */
#define KEYDAY_SHEMISPHERE 181 /* last day of driest month (Jun) of southern hemisphere */

void phen_variety(Pft *pft,      /**< PFT variables */
                  int vern_date20,
                  Real lat,   /**< latitude (deg) */
                  int sdate,  /**< sowing date (1..365) */
                  Bool wtype  /**< winter type (TRUE/FALSE) */
                 )
{
  int keyday,keyday1;
  Pftcrop *crop;
  const Pftcroppar *croppar;
  crop=pft->data;
  croppar=pft->par->data;
  crop->wtype=wtype;
  if(lat>=0)
  {
    keyday=KEYDAY_NHEMISPHERE;
    keyday1=0;
  }
  else
    keyday=keyday1=KEYDAY_SHEMISPHERE;

  if(sdate>365)
    sdate-=365;

  if(croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
  {
    if(wtype)
    {
      crop->pvd=max(0,min(60,vern_date20-sdate-croppar->pvd));
      crop->phu=max(croppar->phuw.low,-0.1081*pow((sdate-keyday),2)
                +3.1633*(sdate-keyday)+croppar->phuw.high);
    }
    else
    {
      crop->pvd=0;/*max(0,min(60,vern_date20-sdate));*/
      /* quick fix for too long growing periods in the high latitudes */
      crop->phu=min(croppar->phus.high,max(croppar->phus.low,
              max(croppar->basetemp.low,pft->stand->cell->climbuf.atemp_mean20_fix)*200));
    }
  }
  else
  {
    crop->pvd=0;
    crop->phu=min(croppar->phus.high,max(croppar->phus.low,
          max(croppar->basetemp.low,pft->stand->cell->climbuf.atemp_mean20_fix)*167));
  }

  crop->basetemp=min(croppar->basetemp.high,max(croppar->basetemp.low,pft->stand->cell->climbuf.atemp_mean20_fix-3.0));
} /* of 'phen_variety' */

/*
- called in cultivate()
- this function adjusts parameters (pvd,phu,basetemp) for certain cfts (pvd, phu
  for temperate cereals, rapeseed, sunflower; basetemp only for maize) for a
  variety of the phenology between colder and warmer regions
- the parameters pvd, phu.low/high, phu_par, basetemp.low/high are set in pft.par,
  so that only for the above mentioned cfts a variety can be determined
- sets the keydays dependent on n/shemisphere
- brings the local variable sdate (sowing date) to the interval [1,365]
- 3 different determinations of phu (phenological heat units) and pvd
  (vernalization days) dependent of variables vern_date20 and wtype
  -> hints: the parameters phu.low/high are equal for all cfts except for
            temperate cereals,rapeseed,sunflower
            the parameter phu_par is set to 9999 for all cfts except for sunflower
- checks if vernalization days exist (vern_date20>0)
  -> only for temperate cereals, rapeseed possible
  -> if TRUE: checks if cft is a winter type (wtype)

- determination of basetemp (base temperature) dependent on sdate
  -> hint: parameters bastemp.low/high are equal for all cfts except for
           sunflower, so that the basetemp of these cfts is equal to the
           parameters basetemp.low/high (see pft.par)
*/
