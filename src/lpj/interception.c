/**************************************************************************************/
/**                                                                                \n**/
/**                  i  n  t  e  r  c  e  p  t  i  o  n  .  c                      \n**/
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

Real interception(Real *wet,      /**< relative wetness */
                  const Pft *pft, /**< PFT variables */
                  Real eeq,       /**< equilibrium evapotranspiration (mm) */
                  Real rain       /**< precipitation (mm) */
                 )                /** \return interception (mm) */ 
{
  Real int_store;
  if(eeq<0.0001 || pft->fpc==0)
  {
    *wet=0;
    return 0; 
  }
  int_store=pft->par->intc*actual_lai(pft);                                //*pft->nind/pft->fpc;
  if(int_store>0.9999)
    int_store=0.9999;
  *wet=int_store*rain/(eeq*PRIESTLEY_TAYLOR);
  if (*wet>0.9999) 
    *wet=0.9999;
#ifdef SAFE
  if(rain<eeq*PRIESTLEY_TAYLOR*(*wet)*pft->fpc)
  {
    printf("par_intc: %f, lai: %f, phen: %f pet: %f rain: %f pft: %s\n",
            pft->par->intc,actual_lai(pft),pft->phen,eeq*PRIESTLEY_TAYLOR,rain,pft->par->name);
    fflush(stdout);
  }
#endif
  
  return eeq*PRIESTLEY_TAYLOR*(*wet)*pft->fpc;
} /* of interception */
