/**************************************************************************************/
/**                                                                                \n**/
/**      e  s  t  a  b  l  i  s  h  m  e  n  t  p  f  t  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates establishment in population                            \n**/
/**     Simulates population increase through establishment each                   \n**/
/**     simulation year for trees and grasses and introduces new PFTs if           \n**/
/**     climate conditions become suitable.                                        \n**/
/**     This function assumes each Individual object represents the                \n**/
/**     average individual of a PFT population, and that there is (at              \n**/
/**     most) one individual object per PFT per modelled area (stand).             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "tree.h"
#include "grass.h"

Stocks establishmentpft(Stand *stand,        /**< Stand pointer  */
                        int npft,            /**< number of natural PFTs */
                        Real aprec,          /**< annual precipitation (mm) */
                        int year,            /**< simulation year (AD) */
                        const Config *config /**< LPJmL configuration */
                       )  /** \return establishment flux (gC/m2,gN/m2) */
{
  Stocks flux_est={0,0},stocks;
  Real fpc_total,*fpc_type, fpc_obs_cor, fpc_obs;
  int *n_est;
  Bool *present;
  int p,t;
  Pft *pft;
  present=newvec(Bool,npft);
  check(present);
  fpc_type=newvec(Real,config->ntypes);
  check(fpc_type);
  n_est=newvec(int,config->ntypes);
  check(n_est);
  for(p=0;p<npft;p++)
    present[p]=FALSE;
  foreachpft(pft,p,&stand->pftlist)
    present[pft->par->id]=TRUE;
#ifdef DEBUG2
  printf("ESTAB %s: %g %d\n",stand->type->name,stand->frac,stand->prescribe_landcover);
  printf("Number of pfts: %d\n",stand->pftlist.n);
  for(p=0;p<npft;p++)
    printf("%s ",bool2str(present[p]));
  printf("\n");
#endif
  for(t=0;t<config->ntypes;t++)
    n_est[t]=0;

  /* establish PFTs if observed landcover > 0 or bioclimatic limits are suitable in dynamic mode */
  for(p=0;p<npft;p++)
  {
    if ((stand->prescribe_landcover !=NO_LANDCOVER &&  config->pftpar[p].cultivation_type==NONE && stand->cell->landcover[p] > 0 && stand->type->landusetype==NATURAL) ||
        (stand->prescribe_landcover == NO_LANDCOVER && aprec>=config->pftpar[p].aprec_min && config->pftpar[p].cultivation_type==NONE &&
        (stand->soil.par->type != ICE && stand->soil.par->type != ROCK) &&
       establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf)))
    {
      if(!present[p])
        addpft(stand,config->pftpar+p,year,0,config);
      n_est[config->pftpar[p].type]++;
    }
  }

  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  foreachpft(pft,p,&stand->pftlist)
  {
    fpc_obs_cor = 1;
    if (stand->prescribe_landcover !=NO_LANDCOVER &&  pft->par->cultivation_type==NONE && stand->type->landusetype==NATURAL)
    {
      fpc_obs = stand->cell->landcover[pft->par->id];
      /* adjust observed FPC by stand fraction of natural vegetation */
      fpc_obs_cor = fpc_obs + (1 - stand->frac) * fpc_obs;
    }
    if ((stand->prescribe_landcover == LANDCOVERFPC && fpc_obs_cor > 0 && pft->fpc < fpc_obs_cor &&  pft->par->cultivation_type==NONE && stand->type->landusetype==NATURAL) ||
        (stand->prescribe_landcover == LANDCOVEREST && fpc_obs > 0 &&  pft->par->cultivation_type==NONE && stand->type->landusetype==NATURAL) ||
        (stand->prescribe_landcover == NO_LANDCOVER && aprec>=pft->par->aprec_min && pft->par->cultivation_type==NONE &&
        istree(pft) && (stand->soil.par->type != ICE && stand->soil.par->type != ROCK) &&
#ifdef DAILY_ESTABLISHMENT
        !pft->established &&
#endif
        establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf)))
    {
      stocks=establishment(pft,fpc_total,fpc_type[pft->par->type],
                           n_est[pft->par->type]);
      flux_est.carbon+=stocks.carbon;
      flux_est.nitrogen+=stocks.nitrogen;
#ifdef DAILY_ESTABLISHMENT
      pft->established=TRUE;
#endif
    }
  }
  fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  foreachpft(pft,p,&stand->pftlist)
    if(pft->par->type==TREE)
      adjust_tree(&stand->soil.litter,pft,fpc_type[pft->par->type], param.fpc_tree_max,config);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if (fpc_total>1.0)
    foreachpft(pft,p,&stand->pftlist)
      if(pft->par->type==GRASS)
        reduce(&stand->soil.litter,pft,fpc_type[GRASS]/(1+fpc_type[GRASS]-fpc_total),config);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  foreachpft(pft,p,&stand->pftlist)
  {
     if (stand->prescribe_landcover == NO_LANDCOVER && aprec>=pft->par->aprec_min && pft->par->cultivation_type==NONE &&
         !istree(pft) && (stand->soil.par->type != ICE && stand->soil.par->type != ROCK) &&
#ifdef DAILY_ESTABLISHMENT
        !pft->established &&
#endif
        establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf))
    {
      stocks=establishment(pft,fpc_total,fpc_type[pft->par->type],
                           n_est[pft->par->type]);
      flux_est.carbon+=stocks.carbon;
      flux_est.nitrogen+=stocks.nitrogen;
#ifdef DAILY_ESTABLISHMENT
      pft->established=TRUE;
#endif
    }
  }

#ifdef DAILY_ESTABLISHMENT
  if(flux_est.carbon>0) /* any establishment occured? */
  {
#endif
  /* separate calculation of grass FPC after all grass PFTs have been updated */
  foreachpft(pft,p,&stand->pftlist)
    if(pft->par->type==GRASS)
      fpc_grass(pft);
  fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  foreachpft(pft,p,&stand->pftlist)
    if(pft->par->type==TREE)
      adjust_tree(&stand->soil.litter,pft,fpc_type[pft->par->type], param.fpc_tree_max,config);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if (fpc_total>1.0)
    foreachpft(pft,p,&stand->pftlist)
      if(pft->par->type==GRASS)
        reduce(&stand->soil.litter,pft,fpc_type[GRASS]/(1+fpc_type[GRASS]-fpc_total),config);
#ifdef DAILY_ESTABLISHMENT
  }
#endif

#ifdef DEBUG2
  printf("new npft=%d\n",stand->pftlist.n);
#endif
  free(present);
  free(fpc_type);
  free(n_est);
  return flux_est;
} /* of 'establishmentpft' */
