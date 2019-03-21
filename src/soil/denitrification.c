/*************************************************************************************/
/**                                                                                \n**/
/**         d  e  n  i  t  r  i  f  i  c  a  t  i  o  n  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void denitrification(Stand *stand  /**< pointer to stand */
                    )
{
  /* determines NO2 and N2 from nitrate NO3 */
  Real N_denit=0; /* amount of nitrate lost to denitrification */
  Real N2O_denit, denit_t;
  Real FT,FW,TCDF;
  Real Corg;
  Soil *soil;
  int l;
  soil=&stand->soil;
#ifdef DEBUG_N
  printf("NBEFORE");
  for(l=0;l<NSOILLAYER-1;l++)
    printf("%g ",soil->NO3[l]);
  printf("\n");
#endif
  
  forrootsoillayer(l)
  {
    Corg = soil->pool[l].fast.carbon+soil->pool[l].slow.carbon;
    if(Corg<0)
      Corg=0;
    if(soil->temp[l]>epsilon)
      FT = 0.0326+0.00351*pow(soil->temp[l],1.652)-pow((soil->temp[l]/41.748),7.19);
      /* Equation C5 from Smith et al 2014 but only for positive temp */
    else if (soil->temp[l] > 45.9) /* otherwise FT is negative */
      FT=0.0;
    else
      FT=0.0326;
#ifdef DEBUG_N
    printf("w=(%g + %g + %g  + %g + %g )/ %g\n",soil->par->wpwps[l],soil->w[l]*soil->par->whcs[l],soil->ice_depth[l],
           soil->w_fw[l],soil->ice_fw[l],soil->par->wsats[l]);
#endif
    denit_t = (soil->par->wpwps[l]+soil->w[l]*soil->par->whcs[l]+soil->ice_depth[l]+
      soil->w_fw[l]+soil->ice_fw[l])/soil->par->wsats[l]; /* denitrification threshold dependent on water filled pore space */

    /* Version without threshold*/
    N_denit = 0.0;
    N2O_denit = 0.0;
    if(soil->temp[l]<=45.9)
    {
      FW = 6.664096e-10*exp(21.12912*denit_t); /* newly fitted parameters on curve with threshold */
      TCDF = 1-exp(-CDN*FT*Corg);
      N_denit = FW*TCDF*soil->NO3[l];
    }
#ifdef SAFE
    if((FW*TCDF)>1.0 && N_denit>(soil->NO3[l]-epsilon*10))
    {
      fprintf(stdout,"too large denitrification in layer %d: N_denit %g FW %g TCDF %g NO3 %g FT %g Corg %g\n",l,N_denit,FW,TCDF,soil->NO3[l],FT,Corg);
      fflush(stdout);

      N_denit=soil->NO3[l];
    }
#endif
    if(N_denit>soil->NO3[l])
      N_denit=soil->NO3[l];
    soil->NO3[l]-=N_denit;
#ifdef SAFE
    if(soil->NO3[l]<-epsilon)
      fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"Negative soil NO3=%g in layer %d",soil->NO3[l],l);
#endif
    /* Calculation of N2O from denitrification after Bessou 2010 */
    N2O_denit = 0.11 * N_denit;
    N_denit -= N2O_denit;

    stand->cell->output.daily.n2_denit  += N_denit;
    stand->cell->output.daily.n2o_denit += N2O_denit;
    stand->cell->output.mn2o_denit+=N2O_denit*stand->frac;
    stand->cell->output.mn2_emissions+=N_denit*stand->frac;
  }
#ifdef DEBUG_N
  printf("NAFTER");
  for(l=0;l<NSOILLAYER-1;l++)
    printf("%g ",soil->NO3[l]);
  printf("\n");
#endif
} /* of 'denitrification' */
