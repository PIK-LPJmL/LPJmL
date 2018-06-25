/**************************************************************************************/
/**                                                                                \n**/
/**     i  n  i  t  _ c  r  o  p  d  a  t  e  s  .  c                              \n**/
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

Cropdates *init_cropdates(const Pftpar par[], /**< CFT parameter array */
                          int ncft,           /**< number of crop PFTs */
                          Real lat            /**< latitude (degrees) */
                         )                    /** \return allocates crop dates array */
{

  const Pftcroppar *croppar;
  Cropdates *cropdates;
  int cft;
  cropdates=newvec(Cropdates,ncft);
  if(cropdates==NULL)
    return NULL;
  for(cft=0;cft<ncft;cft++)
  {

    croppar=par[cft].data;
  
    cropdates[cft].last_update_fall=cropdates[cft].last_update_spring=cropdates[cft].last_update_vern=0;  
    cropdates[cft].fall_sdate20=cropdates[cft].spring_sdate20=cropdates[cft].vern_date20=0;
    cropdates[cft].fallow=cropdates[cft].fallow_irrig=0;

    if(croppar->calcmethod_sdate==NO_CALC_SDATE || croppar->calcmethod_sdate==MULTICROP)
    {
      cropdates[cft].last_update_fall=cropdates[cft].last_update_spring=NODATA;
      cropdates[cft].fall_sdate20=(lat>=0) ? croppar->initdate.sdatenh  : croppar->initdate.sdatesh;
    }
  }
  return cropdates;
} /* of 'init_cropdates' */

/*
- called in newgrid()
- initialisation of the 3 cropdates + related counters and fallow to zero
- for group NO_CALC_SDATE & MULTICROP the counters last_update_fall &
  last_update_spring are set to -999 and the cropdate fall_sdate20 is set to the 
  initialised sowing date
*/
