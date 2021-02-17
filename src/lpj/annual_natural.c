/**************************************************************************************/
/**                                                                                \n**/
/**            a  n  n  u  a  l  _  n  a  t  u  r  a  l  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for natural stand                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "natural.h"
#include "grass.h"

Bool annual_natural(Stand *stand,         /**< Pointer to stand */
                    int npft,             /**< number of natural pfts */
                    int UNUSED(ncft),     /**< number of crop PFTs */
                    Real popdens,         /**< population density (capita/km2) */
                    int year,             /**< year (AD) */
                    Bool isdaily,         /**< daily temperature data? */
                    Bool UNUSED(intercrop), /**< intercropping enabled (TRUE/FALSE) */
                    const Config *config  /**< LPJ configuration */
                   )                      /** \return stand has to be killed (TRUE/FALSE) */
{
  int p,pft_len;
  Pft *pft;
  Real *fpc_inc;
  Real fire_frac;
  Real fpc_obs,fpc_obs_cor;
  Stocks flux;
#ifndef DAILY_ESTABLISHMENT
  Stocks flux_estab;
#endif
  Stocks firewood={0,0};
  pft_len=getnpft(&stand->pftlist); /* get number of established PFTs */
  if(pft_len>0)
  {
    fpc_inc=newvec(Real,pft_len);
    check(fpc_inc);
    
    foreachpft(pft,p,&stand->pftlist)
    {

#ifdef DEBUG2
      printf("PFT:%s fpc_inc=%g fpc=%g\n",pft->par->name,fpc_inc[p],pft->fpc);
      printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
             pft->bm_inc.carbon,vegc_sum(pft),soilcarbon(&stand->soil));
#endif
      
      if(annualpft(stand,pft,fpc_inc+p,isdaily,config) || config->black_fallow)
      {
        /* PFT killed, delete from list of established PFTs */
        fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1];
        litter_update(&stand->soil.litter,pft,pft->nind,config);
        delpft(&stand->pftlist,p);
        p--; /* adjust loop variable */ 
      }      
    } /* of foreachpft */

    /* separate calculation of grass FPC after all grass PFTs have been updated */
    foreachpft(pft,p,&stand->pftlist)
      if(pft->par->type==GRASS)
        fpc_inc[p]=fpc_grass(pft);

#ifdef DEBUG2
    printf("Number of updated pft: %d\n",stand->pftlist.n);
#endif
    if(year>=config->firstyear && config->firewood==FIREWOOD)
    {
      firewood=woodconsum(stand,popdens);
      getoutput(&stand->cell->output,FLUX_FIREWOOD,config)+=firewood.carbon*stand->frac;
      stand->cell->balance.flux_firewood.carbon+=firewood.carbon*stand->frac;
      stand->cell->balance.flux_firewood.nitrogen+=firewood.nitrogen*stand->frac;
      foreachpft(pft,p,&stand->pftlist)
        if(pft->nind<epsilon)
        {
          fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1];
          litter_update(&stand->soil.litter,pft,pft->nind,config);
          delpft(&stand->pftlist,p);
          p--;  
        }
    }
    light(stand,fpc_inc,config);
    free(fpc_inc);
  }
  if(config->fire==FIRE)
  {  
    fire_frac=fire_prob(&stand->soil.litter,stand->fire_sum);
    getoutput(&stand->cell->output,FIREF,config)+=1.0/fire_frac;
    flux=firepft(&stand->soil.litter,&stand->pftlist,fire_frac,config);
    getoutput(&stand->cell->output,FIREC,config)+=flux.carbon*stand->frac;
    stand->cell->balance.fire.carbon+=flux.carbon*stand->frac;
    if(flux.nitrogen<0)
    {
      getoutput(&stand->cell->output,FIREN,config)+=flux.nitrogen*stand->frac;
      stand->cell->balance.fire.nitrogen+=flux.nitrogen*stand->frac;
    }
    else
    {
      stand->cell->balance.fire.nitrogen+=flux.nitrogen*stand->frac*(1-param.q_ash);
      getoutput(&stand->cell->output,FIREN,config)+=flux.nitrogen*stand->frac*(1-param.q_ash);
      stand->soil.NO3[0]+=flux.nitrogen*param.q_ash;
    }
    stand->cell->output.dcflux+=flux.carbon*stand->frac;
  }
#ifndef DAILY_ESTABLISHMENT
  if(!config->black_fallow)
  {
    flux_estab=establishmentpft(stand,npft,stand->cell->balance.aprec,year,config);
    getoutput(&stand->cell->output,FLUX_ESTABC,config)+=flux_estab.carbon*stand->frac;
    getoutput(&stand->cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen*stand->frac;
    stand->cell->balance.flux_estab.carbon+=flux_estab.carbon*stand->frac;
    stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
    stand->cell->output.dcflux-=flux_estab.carbon*stand->frac;
#if defined IMAGE && defined COUPLED
    if(stand->type->landusetype==NATURAL)
    {
      stand->cell->flux_estab_nat+=flux_estab.carbon*stand->frac;
    }
#endif
  }
#endif
  foreachpft(pft,p,&stand->pftlist)
  {
    /* if land cover is prescribed: reduce FPC and Nind of PFT if observed value is exceeded */
    if(stand->prescribe_landcover == LANDCOVERFPC && stand->type->landusetype==NATURAL)
    {
      fpc_obs = stand->cell->landcover[pft->par->id];
      /* correct prescribed observed FPC value by fraction of natural vegetation stand to reach prescribed value */
      fpc_obs_cor = fpc_obs + (1 - stand->frac) * fpc_obs;
      if (fpc_obs_cor > 0.99)
        fpc_obs_cor = 0.99;
      if (pft->fpc > fpc_obs_cor)
        pft->fpc = fpc_obs_cor;
    }
    getoutputindex(&stand->cell->output,FPC,getpftpar(pft,id)+1,config)+=pft->fpc;
#ifdef SAFE
    if(pft->fpc<0)
      fail(INVALID_FPC_ERR,TRUE,"FPC=%g for '%s' less than zero.\n",pft->fpc,pft->par->name);
#endif
  } /* of foreachpft */
  return FALSE;
} /* of 'annual_natural' */
