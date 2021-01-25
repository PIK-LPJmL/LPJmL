/**************************************************************************************/
/**                                                                                \n**/
/**            a  n  n  u  a  l  _  g  r  a  s  s  l  a  n  d  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for grassland stand                                                   \n**/
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
#include "grassland.h"

Bool annual_grassland(Stand *stand,         /**< Pointer to stand */
                      int npft,             /**< number of natural pfts */
                      int ncft,             /**< number of crop PFTs */
                      Real UNUSED(popdens), /**< population density (capita/km2) */
                      int year,             /**< simulation year (AD) */
                      Bool isdaily,         /**< daily temperature data? */
                      Bool UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
                      const Config *config  /**< LPJ configuration */
                     )                      /** \return stand has to be killed */
{
  int p,nirrig,nnat;
  Bool *present;
  Pft *pft;
  Pftgrass *grass;
  Real fpc_inc;
  Stocks flux_estab,stocks;
  int n_est=0;
  Real fpc_total,*fpc_type;
  Grassland *grassland;

  fpc_type=newvec(Real,config->ntypes);
  check(fpc_type);
  present=newvec(Bool,npft);
  check(present);
  flux_estab.carbon=flux_estab.nitrogen=0;
  for(p=0;p<npft;p++)
    present[p]=FALSE;

  grassland=stand->data;

  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);

  foreachpft(pft,p,&stand->pftlist)
  {
#ifdef DEBUG2
    printf("PFT:%s fpc=%g\n",pft->par->name,pft->fpc);
    printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
           pft->bm_inc.carbon,vegc_sum(pft),soilcarbon(&stand->soil));
#endif

    present[pft->par->id]=TRUE;
    if(annual_grass(stand,pft,&fpc_inc,config->new_phenology,config->with_nitrogen,isdaily))
    {
      /* PFT killed, delete from list of established PFTs */
      litter_update_grass(&stand->soil.litter,pft,pft->nind);
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

  for(p=0;p<npft;p++)
  {
    if(config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE &&
       establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf))
    {
      if(!present[p])
       addpft(stand,config->pftpar+p,year,0,config->with_nitrogen,config->double_harvest);
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
  stand->cell->output.flux_estab.carbon+=flux_estab.carbon*stand->frac;
  stand->cell->output.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
  stand->cell->balance.flux_estab.carbon+=flux_estab.carbon*stand->frac;
  stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
  stand->cell->output.dcflux-=flux_estab.carbon*stand->frac;

  stand->cell->balance.soil_storage+=(grassland->irrigation.irrig_stor+grassland->irrigation.irrig_amount)*stand->frac*stand->cell->coord.area;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    if(isannual(FPC_BFT,config))
      stand->cell->output.fpc_bft[getpftpar(pft, id)-(nnat-config->ngrass)+grassland->irrigation.irrigation*(config->nbiomass+2*config->ngrass)]+=pft->fpc;
    if(isannual(PFT_VEGC,config))
    {
      stand->cell->output.pft_veg[nnat+rothers(ncft)+grassland->irrigation.irrigation*nirrig].carbon+=vegc_sum(pft);
      stand->cell->output.pft_veg[nnat+rmgrass(ncft)+grassland->irrigation.irrigation*nirrig].carbon+=vegc_sum(pft);
    }
    if(isannual(PFT_VEGN,config))
    {
      stand->cell->output.pft_veg[nnat+rothers(ncft)+grassland->irrigation.irrigation*nirrig].nitrogen+=vegn_sum(pft);
      stand->cell->output.pft_veg[nnat+rmgrass(ncft)+grassland->irrigation.irrigation*nirrig].nitrogen+=vegn_sum(pft);
    }
    if(isannual(PFT_CLEAF,config))
    {
      stand->cell->output.pft_leaf[nnat+rothers(ncft)+grassland->irrigation.irrigation*nirrig].carbon+=grass->ind.leaf.carbon;
      stand->cell->output.pft_leaf[nnat+rmgrass(ncft)+grassland->irrigation.irrigation*nirrig].carbon+=grass->ind.leaf.carbon;
    } 
    if(isannual(PFT_NLEAF,config))
    {
      stand->cell->output.pft_root[nnat+rothers(ncft)+grassland->irrigation.irrigation*nirrig].nitrogen+=grass->ind.root.nitrogen;
      stand->cell->output.pft_root[nnat+rmgrass(ncft)+grassland->irrigation.irrigation*nirrig].nitrogen+=grass->ind.root.nitrogen;
    } 
    if(isannual(PFT_CROOT,config))
    {
      stand->cell->output.pft_root[nnat+rothers(ncft)+grassland->irrigation.irrigation*nirrig].carbon+=grass->ind.leaf.carbon;
      stand->cell->output.pft_root[nnat+rmgrass(ncft)+grassland->irrigation.irrigation*nirrig].carbon+=grass->ind.root.carbon;
    } 
    if(isannual(PFT_NROOT,config))
    {
      stand->cell->output.pft_leaf[nnat+rothers(ncft)+grassland->irrigation.irrigation*nirrig].nitrogen+=grass->ind.leaf.nitrogen;
      stand->cell->output.pft_leaf[nnat+rmgrass(ncft)+grassland->irrigation.irrigation*nirrig].nitrogen+=grass->ind.leaf.nitrogen;
    } 
  }

  free(present);
  free(fpc_type);
  return FALSE;
} /* of 'annual_grassland' */
