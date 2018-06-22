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
                    Bool UNUSED(intercrop), /**< intercroping enabled (TRUE/FALSE) */
                    const Config *config  /**< LPJ configuration */
                   )                      /** \return stand has to be killed (TRUE/FALSE) */
{
  int p,pft_len;
  Pft *pft;
  Real *fpc_inc;
  Real fire_frac;
  Real firec;
  Real fpc_obs_cor;
#ifndef DAILY_ESTABLISHMENT
  Real acflux_estab;
#endif
  Real firewood=0;

  pft_len=getnpft(&stand->pftlist); /* get number of established PFTs */
  if(pft_len>0)
  {
    fpc_inc=newvec(Real,pft_len);
    check(fpc_inc);
    
    foreachpft(pft,p,&stand->pftlist)
    {
      if (stand->prescribe_landcover == LANDCOVERFPC && stand->type->landusetype==NATURAL)
      {
        pft->prescribe_fpc = TRUE;
      }
      else
        pft->prescribe_fpc = FALSE;

#ifdef DEBUG2
      printf("PFT:%s fpc_inc=%g fpc=%g\n",pft->par->name,fpc_inc[p],pft->fpc);
      printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
             pft->bm_inc,vegc_sum(pft),soilcarbon(&stand->soil));
#endif
      
      if(annualpft(stand,pft,fpc_inc+p,isdaily))
      {
        /* PFT killed, delete from list of established PFTs */
        fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1];
        litter_update(&stand->soil.litter,pft,pft->nind);
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
      stand->cell->output.flux_firewood+=firewood*stand->frac;
      foreachpft(pft,p,&stand->pftlist)
        if(pft->nind<epsilon)
        {
          fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1];
          litter_update(&stand->soil.litter,pft,pft->nind);
          delpft(&stand->pftlist,p);
          p--;  
        }
    }
    light(stand,config->ntypes,fpc_inc);
    free(fpc_inc);
  }
  if(config->fire==FIRE)
  {  
    fire_frac=fire_prob(&stand->soil.litter,stand->fire_sum);
    stand->cell->output.firef+=1.0/fire_frac;
    firec=firepft(&stand->soil.litter,
                  &stand->pftlist,fire_frac)*stand->frac;
    stand->cell->output.firec+=firec;
    stand->cell->output.dcflux+=firec;
  }
#ifndef DAILY_ESTABLISHMENT
  acflux_estab=establishmentpft(stand,config->pftpar,npft,config->ntypes,stand->cell->balance.aprec,year);
  stand->cell->output.flux_estab+=acflux_estab*stand->frac;
  stand->cell->output.dcflux-=acflux_estab*stand->frac;
#endif
  foreachpft(pft,p,&stand->pftlist)
  {
  
    /* if land cover is prescribed: reduce FPC and Nind of PFT if observed value is exceeded */
    if (pft->prescribe_fpc)
    {
      /* correct prescribed observed FPC value by fraction of natural vegetation stand to reach prescribed value */
      fpc_obs_cor = pft->fpc_obs + (1 - stand->frac) * pft->fpc_obs;;
      if (fpc_obs_cor > 0.99)
        fpc_obs_cor = 0.99;
      if (pft->fpc > fpc_obs_cor)
           pft->fpc = fpc_obs_cor;
     }
    stand->cell->output.fpc[getpftpar(pft,id)+1]=pft->fpc;
  }
  return FALSE;
} /* of 'annual_natural' */
