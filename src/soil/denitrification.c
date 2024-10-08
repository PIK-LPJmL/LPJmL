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
#include "crop.h"
#include "agriculture.h"

void denitrification(Stand *stand,        /**< pointer to stand */
                     int npft,            /**< number of natural PFTs */
                     int ncft,            /**< number of crop PFTs */
                     const Config *config /**< LPJmL configuration */
                    )
{
  /* determines NO2 and N2 from nitrate NO3 */
  Real N_denit=0; /* amount of nitrate lost to denitrification */
  Real N2O_denit, denit_t;
  Real FT=0,FW=0,TCDF=0;
  Real Corg;
  Soil *soil;
  int l,p;
  Pft *pft;
  Pftcrop *crop;
  Irrigation *data;
  soil=&stand->soil;
#ifdef DEBUG_N
  printf("NBEFORE");
  for(l=0;l<NSOILLAYER-1;l++)
    printf("%g ",soil->NO3[l]);
  printf("\n");
#endif
  
  forrootsoillayer(l)
  {
    //wscaler=(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l])) : 0;
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
    printf("w=(%g + %g + %g  + %g + %g )/ %g\n",soil->wpwps[l],soil->w[l]*soil->whcs[l],soil->ice_depth[l],
           soil->w_fw[l],soil->ice_fw[l],soil->wsats[l]);
#endif
    denit_t = (soil->wpwps[l]+soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+
      soil->w_fw[l]+soil->ice_fw[l])/soil->wsats[l]; /* denitrification threshold dependent on water filled pore space */

    /* Version without threshold*/
    N_denit = 0.0;
    N2O_denit = 0.0;
    if(soil->temp[l]<=45.9)
    {
      FW = min(1.0,6.664096e-10*exp(21.12912*denit_t)); /* newly fitted parameters on curve with threshold */
      TCDF = 1-exp(-CDN*FT*Corg);
      N_denit = FW*TCDF*soil->NO3[l];
    }
#ifdef SAFE
    if((FW*TCDF)>1.0 && N_denit>(soil->NO3[l]+epsilon*10))
    {
      fprintf(stderr,"Too large denitrification in layer %d: N_denit %g FW %g TCDF %g NO3 %g FT %g Corg %g\n",l,N_denit,FW,TCDF,soil->NO3[l],FT,Corg);
      fflush(stderr);
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

    getoutput(&stand->cell->output,N2O_DENIT,config)+=N2O_denit*stand->frac;
    getoutput(&stand->cell->output,N2_EMIS,config)+=N_denit*stand->frac;
    stand->cell->balance.n_outflux+=(N_denit+N2O_denit)*stand->frac;
    if(isagriculture(stand->type->landusetype))
    {
      getoutput(&stand->cell->output,N2O_DENIT_AGR,config)+=N2O_denit*stand->frac;
      getoutput(&stand->cell->output,N2_AGR,config)+=N_denit*stand->frac;
    }
    if(stand->type->landusetype==GRASSLAND)
    {
      getoutput(&stand->cell->output,N2O_DENIT_MGRASS,config)+=N2O_denit*stand->frac;
      getoutput(&stand->cell->output,N2_MGRASS,config)+=N_denit*stand->frac;
    }
    if(stand->type->landusetype==AGRICULTURE)
    {
      data=stand->data;
      foreachpft(pft,p,&stand->pftlist)
      {
        crop=pft->data;
        if(crop->sh!=NULL)
        {
          crop->sh->n2o_denitsum+=N2O_denit;
          crop->sh->n2_emissum+=N_denit;
        }
        else
        {
          getoutputindex(&stand->cell->output,CFT_N2O_DENIT,pft->par->id-npft+data->irrigation*ncft,config)+=N2O_denit;
          getoutputindex(&stand->cell->output,CFT_N2_EMIS,pft->par->id-npft+data->irrigation*ncft,config)+=N_denit;
        }
      }
    }
  }
#ifdef DEBUG_N
  printf("NAFTER");
  for(l=0;l<NSOILLAYER-1;l++)
    printf("%g ",soil->NO3[l]);
  printf("\n");
#endif
} /* of 'denitrification' */
