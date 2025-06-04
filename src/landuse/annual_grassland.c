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
//#define DEBUG2

Bool annual_grassland(Stand *stand,         /**< Pointer to stand */
                      int npft,             /**< number of natural pfts */
                      int ncft,             /**< number of crop PFTs */
                      int year,             /**< simulation year (AD) */
                      Bool isdaily,         /**< daily temperature data? */
                      Bool UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
                      const Config *config  /**< LPJ configuration */
                     )                      /** \return stand has to be killed */
{
  int p,nirrig,nnat,index;
  Bool *present;
  Pft *pft;
  Pftgrass *grass;
  Bool peatland=FALSE;
  Real fpc_inc;
  Stocks flux_estab,stocks;
  int n_est=0;
  Real fpc_total,*fpc_type;
  Grassland *grassland;
#ifdef CHECK_BALANCE
  Stocks start = {0,0};
  Real end = 0;
  Stocks bm_inc={0,0};
  start.carbon = standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4;
  start.nitrogen = standstocks(stand).nitrogen;
  foreachpft(pft,p,&stand->pftlist)
   bm_inc.nitrogen+=pft->bm_inc.nitrogen;
#endif

  fpc_type=newvec(Real,config->ntypes);
  check(fpc_type);
  present=newvec(Bool,npft);
  check(present);
  flux_estab.carbon=flux_estab.nitrogen=0;
  for(p=0;p<npft;p++)
    present[p]=FALSE;

  grassland=stand->data;

  nnat=getnnat(npft,config);
  index=(stand->type->landusetype==GRASSLAND) ? rmgrass(ncft) : rothers(ncft);
  nirrig=getnirrig(ncft,config);
  foreachpft(pft,p,&stand->pftlist)
  {
#ifdef DEBUG3
    printf("PFT:%s fpc=%g\n",pft->par->name,pft->fpc);
    printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
           pft->bm_inc.carbon,vegc_sum(pft),soilcarbon(&stand->soil));
#endif

    present[pft->par->id]=TRUE;
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

#ifdef DEBUG3
  printf("4 Number of updated pft: %d\n",stand->pftlist.n);
#endif

  for(p=0;p<npft;p++)
  {
    if(config->pftpar[p].type==GRASS && p!=Sphagnum_moss && config->pftpar[p].cultivation_type==NONE &&
       establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf,peatland))
    {
      if(!present[p])
       addpft(stand,config->pftpar+p,year,0,config);
      n_est++;
    }
  }
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  foreachpft(pft,p,&stand->pftlist)
    if(establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf, stand->type->landusetype == WETLAND || stand->type->landusetype==SETASIDE_WETLAND))
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

  stand->cell->balance.soil_storage+=(grassland->irrigation.irrig_stor+grassland->irrigation.irrig_amount)*stand->frac*stand->cell->coord.area;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    if(stand->type->landusetype==GRASSLAND)
    {
      if(isannual(FPC_BFT,config))
        getoutputindex(&stand->cell->output,FPC_BFT,getpftpar(pft, id)-(nnat-config->ngrass)+grassland->irrigation.irrigation*(config->nbiomass+2*config->ngrass),config)+=pft->fpc;
    }
    if(isannual(PFT_VEGC,config))
    {
      getoutputindex(&stand->cell->output,PFT_VEGC,nnat+index+grassland->irrigation.irrigation*nirrig,config)+=vegc_sum(pft);
    }
    if(isannual(PFT_VEGN,config))
    {
      getoutputindex(&stand->cell->output,PFT_VEGN,nnat+index+grassland->irrigation.irrigation*nirrig,config)+=vegn_sum(pft)+pft->bm_inc.nitrogen;
    }
    if(isannual(PFT_CROOT,config))
    {
      getoutputindex(&stand->cell->output,PFT_CROOT,nnat+index+grassland->irrigation.irrigation*nirrig,config)+=grass->ind.root.carbon;
    } 
    if(isannual(PFT_NROOT,config))
    {
      getoutputindex(&stand->cell->output,PFT_NROOT,nnat+index+grassland->irrigation.irrigation*nirrig,config)+=grass->ind.root.nitrogen;
    } 
    if(isannual(PFT_CLEAF,config))
    {
      getoutputindex(&stand->cell->output,PFT_CLEAF,nnat+index+grassland->irrigation.irrigation*nirrig,config)+=grass->ind.leaf.carbon;
    } 
    if(isannual(PFT_NLEAF,config))
    {
      getoutputindex(&stand->cell->output,PFT_NLEAF,nnat+index+grassland->irrigation.irrigation*nirrig,config)+=grass->ind.leaf.nitrogen;
    } 
  }
#ifdef CHECK_BALANCE
    end = standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4;
    if (fabs(end-start.carbon-flux_estab.carbon)>0.001)
      fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid carbon balance in %s: %g start:%g  end:%g flux_estab.carbon: %g  type:%s\n",
           __FUNCTION__,end-start.carbon, start.carbon, end, flux_estab.carbon, stand->type->name);
    end = standstocks(stand).nitrogen;
    if (fabs(end-start.nitrogen-flux_estab.nitrogen)>0.0001)
      fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid nitrogen balance in %s: %g start:%g  end:%g flux_estab.carbon: %g  type:%s\n",
           __FUNCTION__,end-start.nitrogen, start.nitrogen, end, flux_estab.nitrogen, stand->type->name);
#endif
  free(present);
  free(fpc_type);
  return FALSE;
} /* of 'annual_grassland' */
