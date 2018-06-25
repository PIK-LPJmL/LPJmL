/**************************************************************************************/
/**                                                                                \n**/
/**     o  u  t  f  l  o  w  _  r  e  s  e  r  v  o  i  r  .  c                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real outflow_reservoir(Resdata *resdata, /**< pointer to reservoir data */
                       int month
                       )                 /** \return outflow from reservoir (dm3) */
{
  Real dfresout=0;
  
  if(resdata->c>=0.5)
    dfresout=max(0,resdata->k_rls*resdata->target_release_month[month]);
  else if (resdata->c>=0 && resdata->c<0.5)
    dfresout=max(0,pow(resdata->c/0.5,2)*resdata->k_rls*resdata->target_release_month[month]+(1-pow(resdata->c/0.5,2))*resdata->mean_inflow_month[month]/ndaymonth[month]);
  else 
    fail(OUTFLOW_RESERVOIR_ERR,TRUE,"outflow_reservoir ERROR: c<0!");

  if(dfresout>resdata->dmass)
    dfresout=resdata->dmass;

  resdata->dmass-=dfresout;
  
  return dfresout;
} /* of 'outflow_reservoir' */
