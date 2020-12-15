/**************************************************************************************/
/**                                                                                \n**/
/**     a  n  n  u  a  l  _  b  i  o  m  a  s  s  _  g  r  a  s  s  .  c           \n**/
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
#include "biomass_grass.h"

Bool annual_biomass_grass(Stand *stand,         /* Pointer to stand */
                          int npft,             /**< number of natural pfts */
                          int ncft,             /**< number of crop PFTs */
                          Real UNUSED(popdens), /**< population density (capita/km2) */
                          int year,             /**< simulation year (AD) */
                          Bool isdaily,         /**< daily temperature data? */
                          Bool UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
                          const Config *config  /**< LPJ configuration */
                         )                      /** \return stand has to be killed */
{
  int p;
  Bool *present;
  Pft *pft;
  Real fpc_inc;
  Stocks estab_store;
  Stocks flux_estab,stocks;
  int n_est=0;
  Real fpc_total,*fpc_type;
  Irrigation *irrigation;
  Pftgrasspar *grasspar;

  irrigation=stand->data;
  fpc_type=newvec(Real,config->ntypes);
  check(fpc_type);
  present=newvec(Bool,npft);
  check(present);
  flux_estab.carbon=estab_store.carbon=flux_estab.nitrogen=estab_store.nitrogen=0;
  for(p=0;p<npft;p++)
    present[p]=FALSE;

  foreachpft(pft,p,&stand->pftlist)
  {
#ifdef DEBUG2
    printf("PFT:%s fpc=%g\n",pft->par->name,pft->fpc);
    printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
           pft->bm_inc.carbon,vegc_sum(pft),soilcarbon(&stand->soil));
#endif

    present[pft->par->id]=TRUE;
    grasspar=pft->par->data;
    if (stand->cell->balance.estab_storage_grass[irrigation->irrigation].carbon<grasspar->sapling_C*0.01)
    {
      estab_store.carbon=pft->bm_inc.carbon*0.001;
      estab_store.nitrogen=pft->bm_inc.nitrogen*0.001;
      pft->bm_inc.carbon-=estab_store.carbon;
      pft->bm_inc.nitrogen-=estab_store.nitrogen;
      stand->cell->balance.estab_storage_grass[irrigation->irrigation].carbon+=estab_store.carbon*stand->frac;
      stand->cell->balance.estab_storage_grass[irrigation->irrigation].nitrogen+=estab_store.nitrogen*stand->frac;
    }
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

  for(p=0;p<npft;p++)
  {
    if(config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==BIOMASS
       && establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf))
    {
      if(!present[p])
       addpft(stand,config->pftpar+p,year,0);
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

  stand->cell->balance.estab_storage_grass[irrigation->irrigation].carbon-=flux_estab.carbon*stand->frac;
  stand->cell->balance.estab_storage_grass[irrigation->irrigation].nitrogen-=flux_estab.nitrogen*stand->frac;

  stand->cell->balance.soil_storage+=(irrigation->irrig_stor+irrigation->irrig_amount)*stand->frac*stand->cell->coord.area;
  foreachpft(pft,p,&stand->pftlist)
  {
    stand->cell->output.fpc_bft[getpftpar(pft, id)-npft+config->nbiomass+config->nwft+2*config->ngrass+irrigation->irrigation*(config->nbiomass+2*config->ngrass)]=pft->fpc;
    stand->cell->output.pft_veg[npft-config->nbiomass-config->nwft+rbgrass(ncft)+irrigation->irrigation*(ncft+NGRASS*NBIOMASSTYPE+NWPTYPE)].carbon+=vegc_sum(pft);
    stand->cell->output.pft_veg[npft-config->nbiomass-config->nwft+rbgrass(ncft)+irrigation->irrigation*(ncft+NGRASS*NBIOMASSTYPE)].nitrogen+=vegn_sum(pft);
  }

  free(fpc_type);
  free(present);
  return FALSE;
} /* of 'annual_biomass_grass' */
