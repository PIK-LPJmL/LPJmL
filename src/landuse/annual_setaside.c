/**************************************************************************************/
/**                                                                                \n**/
/**               a  n  n  u  a  l  _  s  e  t  a  s  i  d  e  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for setaside stand                                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"
#include "agriculture.h"

Bool annual_setaside(Stand *stand,         /**< Pointer to stand */
                     int npft,             /**< number of natural PFTs */
                     int UNUSED(ncft),     /**< number of crop PFTs */
                     int year,             /**< simulation year */
                     Bool isdaily,         /**< daily temperature data? */
                     Bool intercrop,       /**< enable intercropping (TRUE/FALSE) */
                     const Config *config  /**< LPJmL configuration */
                    )                      /** \return stand has to be killed (TRUE/FALSE) */
{
  int p;
  Pft *pft;
  Real fpc_inc;
  Real *fpc_type;
#ifndef DAILY_ESTABLISHMENT
  Real fpc_total;
  Bool *present;
  Stocks flux_estab,stocks;
  int n_est=0;
  present=newvec(Bool,npft);
  check(present);
  flux_estab.carbon=flux_estab.nitrogen=0;
  for(p=0;p<npft;p++)
    present[p]=FALSE;
#endif
  fpc_type=newvec(Real,config->ntypes);
  check(fpc_type);

  foreachpft(pft,p,&stand->pftlist)
  {
#ifdef DEBUG2
    printf("PFT:%s fpc=%g\n",pft->par->name,pft->fpc);
    printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
           pft->bm_inc.carbon,vegc_sum(pft),soilcarbon(&stand->soil));
#endif

#ifndef DAILY_ESTABLISHMENT
    present[pft->par->id]=TRUE;
#endif
    if(annual_grass(stand,pft,&fpc_inc,isdaily,config))
    {
      /* PFT killed, delete from list of established PFTs */
      litter_update_grass(&stand->soil.litter,pft,pft->nind,config);
      delpft(&stand->pftlist,p);
      p--; /* adjust loop variable */ 
    }
        
  } /* of foreachpft */

  /* separate calculation of grass FPC after all grass PFTs have been updated */
  foreachpft(pft,p,&stand->pftlist)
    fpc_grass(pft);

#ifdef DEBUG2
  printf("Number of updated pft: %d\n",stand->pftlist.n);
#endif

#ifndef DAILY_ESTABLISHMENT
  if(intercrop)
  {
    for(p=0;p<npft;p++)
    {
      if(config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE 
         && establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf))
      {
        if(!present[p])
         addpft(stand,config->pftpar+p,year,0,config);
        n_est++;
      }
    }
    fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
    foreachpft(pft,p,&stand->pftlist)
      if(establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf))
      {
        stocks=establishment_grass(pft,fpc_total,fpc_type[pft->par->type],n_est);
        flux_estab.carbon+=stocks.carbon;
        flux_estab.nitrogen+=stocks.nitrogen;
      }

    getoutput(&stand->cell->output,FLUX_ESTABC,config)+=flux_estab.carbon*stand->frac;
    getoutput(&stand->cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen*stand->frac;
    getoutput(&stand->cell->output,FLUX_ESTABN_MG,config)+=flux_estab.nitrogen*stand->frac;
    stand->cell->balance.flux_estab.carbon+=flux_estab.carbon*stand->frac;
    stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
    stand->cell->output.dcflux-=flux_estab.carbon*stand->frac;
  }
#endif

#ifndef DAILY_ESTABLISHMENT
  free(present);
#endif
  free(fpc_type);
  return FALSE;
} /* of 'annual_setaside' */
