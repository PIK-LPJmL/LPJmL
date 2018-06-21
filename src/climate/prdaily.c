/**************************************************************************************/
/**                                                                                \n**/
/**                   p  r  d  a  i  l  y  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**     Function calculates daily precipitation from monthly raninfall             \n**/
/**     and number of wet days                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/**
 * PRDAILY
 * Distribution of monthly precipitation totals to quasi-daily values
 **/

#define c1 1.0 /** normalising coefficient for exponential distribution */
#define c2 1.2 /** power for exponential distribution */

void prdaily(Real dval_prec[],  /**< Daily precipitation array (mm)  */
             int ndaymonth,     /**< number of days in month */
             Real mval,         /**< total rainfall (mm) for month */
             Real mval_wet      /**< expected number of rain days for month */
             )                  /** \return void */
{
  int d;
  Real prob_rain; /** daily probability of rain for this month */
  Real mprec;     /** average rainfall per rain day for this month */
  Real mprec_sum; /** cumulative sum of rainfall for this month */
  Real prob;
#ifdef SAFE
  int count=0;
#endif

  if (mval<1.0) 
    for (d=1;d<=ndaymonth;d++) 
      dval_prec[d]=0.0;
  else 
  {
    mprec_sum=0.0;
    if (mval_wet<1.0)
      mval_wet=1.0;
    prob_rain=mval_wet/ndaymonth;
    mprec=mval/mval_wet;

    while (mprec_sum<1.0)
    {
#ifdef SAFE
      if(++count%25==0)
      {
        fprintf(stderr,"WARNING021: already %d iterations in prdaily()\n",count);
        fflush(stderr);
      }
#endif
      for (d=1;d<=ndaymonth;d++) 
      {

        /**
        * ----------Transitional probabilities (Geng et al 1986)--------
        **/

        prob=(dval_prec[d-1]<0.1) ? 0.75*prob_rain : 
          0.25+(0.75*prob_rain);

        /**
        * ---------Determine wet days randomly and use Krysanova/Cramer estimates of
        * parameter values (c1,c2) for an exponential distribution---------
        **/

        if (drand48()>prob) 
          dval_prec[d]=0.0;
        else 
        {
          dval_prec[d]=pow(-log(drand48()),c2)*mprec*c1;
          if (dval_prec[d]<0.1) 
            dval_prec[d]=0.0;
          mprec_sum+=dval_prec[d];
        }
      } /** of 'for(d=1;...)' */
    } 

    /**
    * -------- Normalise generated precipitation by prescribed monthly totals----
    **/

    if (mprec_sum>1.0) 
    {
      for (d=1;d<=ndaymonth;d++) 
      {
        dval_prec[d]*=mval/mprec_sum;
        if (dval_prec[d]<0.1) 
          dval_prec[d]=0.0;
      }
    }
  }
  dval_prec[0]=dval_prec[ndaymonth];
} /* of 'prdaily' */
