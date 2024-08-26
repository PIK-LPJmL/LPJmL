/**************************************************************************************/
/**                                                                                \n**/
/**        a n n u a l _ a g r i c u l t u r e _ g r a s s . c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for agriculture grass stand                                           \n**/
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
#include "agriculture_grass.h"

Bool annual_agriculture_grass(Stand* stand,         /**< Pointer to stand */
                              int npft,             /**< number of natural pfts */
                              int ncft,             /**< number of crop PFTs */
                              int year,
                              Bool isdaily,         /**< daily temperature data? */
                              Bool UNUSED(intercrop),
                              const Config* config  /**< LPJmL configuration */
                             )
{
  int p,nnat,index;
  Bool* present;
  Pft* pft;
  Real fpc_inc,excess;
  Stocks estab_store={0,0};
  Stocks flux_estab={0,0};
  Stocks stocks;
  int n_est = 0;
  Real fpc_total, * fpc_type;
  Irrigation* irrigation;
  Pftgrasspar* grasspar;
  Pftgrass *grass;
  irrigation = stand->data;
  fpc_type = newvec(Real, config->ntypes);
  check(fpc_type);
  present = newvec(Bool, npft);
  check(present);
  for (p = 0; p < npft; p++)
    present[p] = FALSE;
  nnat=getnnat(npft,config);
  index=agtree(ncft,config->nwptype)+irrigation->pft_id-npft+config->nagtree+irrigation->irrigation*getnirrig(ncft,config);
  foreachpft(pft, p, &stand->pftlist)
  {
#ifdef DEBUG3
    printf("PFT:%s fpc=%g\n", pft->par->name, pft->fpc);
    printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n", pft->par->name,
           pft->bm_inc.carbon, vegc_sum(pft), soilcarbon(&stand->soil));
#endif

    present[pft->par->id] = TRUE;
    grasspar = pft->par->data;
    if (stand->cell->balance.estab_storage_grass[irrigation->irrigation].carbon < grasspar->sapling_C * 0.01)
    {
      estab_store.carbon = pft->bm_inc.carbon * 0.001;
      estab_store.nitrogen = pft->bm_inc.nitrogen * 0.001;
      pft->bm_inc.carbon -= estab_store.carbon;
      pft->bm_inc.nitrogen -= estab_store.nitrogen;
      stand->cell->balance.estab_storage_grass[irrigation->irrigation].carbon += estab_store.carbon * stand->frac;
      stand->cell->balance.estab_storage_grass[irrigation->irrigation].nitrogen += estab_store.nitrogen * stand->frac;
    }
    if (annual_grass(stand, pft, &fpc_inc,isdaily,config))
    {
      /* PFT killed, delete from list of established PFTs */
      litter_update_grass(&stand->soil.litter, pft, pft->nind,config);
      delpft(&stand->pftlist, p);
      p--; /* adjust loop variable */
    }

  } /* of foreachpft */

  /* separate calculation of grass FPC after all grass PFTs have been updated */
  foreachpft(pft, p, &stand->pftlist)
    fpc_grass(pft);

  fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if(fpc_type[GRASS]>1.0)
    foreachpft(pft,p,&stand->pftlist)
    {
      excess=(fpc_type[GRASS]-1.0)*(pft->fpc/fpc_type[GRASS]);
      light_grass(&stand->soil.litter,pft,excess);
    }

  if(establish(stand->cell->gdd[irrigation->pft_id],config->pftpar+irrigation->pft_id,&stand->cell->climbuf,stand->type->landusetype==WETLAND || stand->type->landusetype==SETASIDE_WETLAND))
  {
    if(!present[p])
     addpft(stand,config->pftpar+irrigation->pft_id,year,0,config);
    n_est++;
  }

  fpc_total = fpc_sum(fpc_type, config->ntypes, &stand->pftlist);
  foreachpft(pft, p, &stand->pftlist)
    if (establish(stand->cell->gdd[pft->par->id], pft->par, &stand->cell->climbuf,stand->type->landusetype==WETLAND || stand->type->landusetype==SETASIDE_WETLAND))
    {
      stocks=establishment_grass(pft, fpc_total, fpc_type[pft->par->type], n_est);
      flux_estab.carbon+=stocks.carbon;
      flux_estab.nitrogen+=stocks.nitrogen;
    }

  stand->cell->balance.estab_storage_grass[irrigation->irrigation].carbon -= flux_estab.carbon * stand->frac;
  stand->cell->balance.estab_storage_grass[irrigation->irrigation].nitrogen -= flux_estab.nitrogen * stand->frac;
  stand->cell->balance.soil_storage += (irrigation->irrig_stor + irrigation->irrig_amount) * stand->frac * stand->cell->coord.area;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    getoutputindex(&stand->cell->output,PFT_VEGC,nnat+index,config)+=vegc_sum(pft);
    getoutputindex(&stand->cell->output,PFT_VEGN,nnat+index,config)+=vegn_sum(pft)+pft->bm_inc.nitrogen;
    getoutputindex(&stand->cell->output,PFT_CROOT,nnat+index,config)+=grass->ind.root.carbon;
    getoutputindex(&stand->cell->output,PFT_NROOT,nnat+index,config)+=grass->ind.root.nitrogen;
    getoutputindex(&stand->cell->output,PFT_CLEAF,nnat+index,config)+=grass->ind.leaf.carbon;
    getoutputindex(&stand->cell->output,PFT_NLEAF,nnat+index,config)+=grass->ind.leaf.nitrogen;
  }

  free(fpc_type);
  free(present);
  return FALSE;
} /* of 'annual_agriculture_grass' */
