/**************************************************************************************/
/**                                                                                \n**/
/**     a  n  n  u  a  l  _  b  i  o  m  a  s  s  _  t  r  e  e  .  c              \n**/
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
#include "tree.h"
#include "natural.h"
#include "agriculture.h"
#include "biomass_tree.h"

#define FPC_MAX 0.95


Bool annual_biomass_tree(Stand *stand,         /**< Pointer to stand */
                         int npft,             /**< number of natural pfts */
                         int ncft,             /**< number of crop PFTs */
                         Real UNUSED(popdens), /**< population density (capita/km2) */
                         int year,             /**< year (AD) */
                         Bool isdaily,         /**< daily temperature data? */
                         Bool intercrop,       /**< enable intercropping (TRUE/FALSE) */
                         const Config *config  /**< LPJmL configuration */
                        )                      /** \return stand has to be killed (TRUE/FALSE) */
{
  int p,pft_len,t;
  Bool *present,isdead;
  int *n_est;
  Pft *pft;
  Real *fpc_inc,*fpc_inc2,*fpc_type;
  Real fpc_total;
  Real acflux_estab=0;
  Real estab_store=0;
  Real yield=0.0;
  Pfttreepar *treepar;
  Irrigation *irrigation;

  irrigation=stand->data;

  isdead=FALSE;

  n_est=newvec(int,config->ntypes);
  check(n_est);
  fpc_type=newvec(Real,config->ntypes);
  check(fpc_type);
  present=newvec(Bool,npft);
  check(present);

  for(p=0;p<npft;p++)
    present[p]=FALSE;
  for(t=0;t<config->ntypes;t++)
    n_est[t]=0;

  pft_len=getnpft(&stand->pftlist); /* get number of established PFTs */
  if(pft_len>0)
  {
    fpc_inc=newvec(Real,pft_len);
    check(fpc_inc);

    foreachpft(pft,p,&stand->pftlist)
    {
      present[pft->par->id]=TRUE;
#ifdef DEBUG2
      printf("PFT:%s fpc_inc=%g fpc=%g\n",pft->par->name,fpc_inc[p],pft->fpc);
      printf("PFT:%s bm_inc=%g vegc=%g soil=%g\n",pft->par->name,
             pft->bm_inc,vegc_sum(pft),soilcarbon(&stand->soil));
#endif
      
      if(istree(pft))
      {
        treepar=pft->par->data;
        if (stand->cell->balance.estab_storage_tree[irrigation->irrigation]<treepar->sapling_C*stand->frac && pft->bm_inc>100)
        {
          estab_store=pft->bm_inc*0.03;
          pft->bm_inc-=estab_store;
          stand->cell->balance.estab_storage_tree[irrigation->irrigation]+=estab_store*stand->frac;
        }
      }

      if(annualpft(stand,pft,fpc_inc+p,isdaily))
      {
        /* PFT killed, delete from list of established PFTs */
        fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1];
        litter_update(&stand->soil.litter,pft,pft->nind);
        pft->nind=0;
        delpft(&stand->pftlist,p);
        p--; /* adjust loop variable */ 
      }
        
    } /* of foreachpft */
#ifdef DEBUG2
    printf("Number of updated pft: %d\n",stand->pftlist.n);
#endif

    light(stand,config->ntypes,fpc_inc);
    free(fpc_inc);
  } /* END if(pft_len>0) */
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);

  /*
  if(setting.fire==FIRE)
  {
    fire_frac=fire_prob(&stand->soil.litter,stand->fire_sum);
    stand->cell->output.firef+=1.0/fire_frac;
    stand->cell->output.firec+=firepft(&stand->soil.litter,
                                       &stand->pftlist,fire_frac)*stand->frac;
  }
  */
  foreachpft(pft,p,&stand->pftlist)
  {
    if(istree(pft))
    {
      treepar=pft->par->data;
      if(stand->growing_time>=treepar->rotation && stand->growing_time%treepar->rotation==0)
      {
        yield=coppice_tree(pft);
        if(stand->cell->balance.estab_storage_tree[irrigation->irrigation]<0)
        {
          yield+=stand->cell->balance.estab_storage_tree[irrigation->irrigation];
          stand->cell->balance.estab_storage_tree[irrigation->irrigation]-=stand->cell->balance.estab_storage_tree[irrigation->irrigation]*stand->frac;
        }
#ifdef IMAGE
        /* communicate bm tree yield every year as fraction of rotation length */
        stand->cell->ml.image_data->store_bmtree_yield=yield;
#endif
        stand->cell->balance.biomass_yield+=yield*stand->frac;
        if(config->pft_output_scaled)
          stand->cell->output.pft_harvest[rbtree(ncft)+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=yield*stand->frac;
        else
          stand->cell->output.pft_harvest[rbtree(ncft)+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE)].harvest+=yield;

        stand->growing_time=0;
      }
    }
#ifdef IMAGE
    /* communicate bm tree yield every year as fraction of rotation length */
    stand->cell->ml.image_data->biomass_yield_annual+=stand->cell->ml.image_data->store_bmtree_yield*stand->frac/treepar->rotation;
#endif

  } /* of foreachpft */

  for(p=0;p<npft;p++)
  {
    if(establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf) &&
       ((config->pftpar[p].type==TREE && config->pftpar[p].cultivation_type==BIOMASS) ||
        (config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE)))
    {
      if(!present[p] && (estab_store<epsilon || config->pftpar[p].type!=TREE) && (fpc_type[TREE]<0.7 || config->pftpar[p].type==GRASS)) {
        addpft(stand,config->pftpar+p,year,0);
        n_est[config->pftpar[p].type]++;
      }
      if(present[p])  n_est[config->pftpar[p].type]++;
    }

  }
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  pft_len=getnpft(&stand->pftlist);
  if(pft_len>0){
    fpc_inc2=newvec(Real,pft_len);
    check(fpc_inc2);
  }
  foreachpft(pft,p,&stand->pftlist) fpc_inc2[p]=0;

  foreachpft(pft,p,&stand->pftlist)
  {
    if(establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf))
      if (istree(pft))
      {
        treepar=pft->par->data;
        if(pft->nind<treepar->k_est && stand->age<treepar->max_rotation_length)
        {
          acflux_estab+=establishment(pft,fpc_total,fpc_type[pft->par->type],
                                        n_est[pft->par->type]);
          fpc_inc2[p]=pft->fpc+fpc_total-1;
        }
      }
      else
      {
        acflux_estab+=establishment(pft,fpc_total,fpc_type[pft->par->type],n_est[pft->par->type]);
      }
  }
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if(fpc_total>1.0) light(stand,config->ntypes,fpc_inc2);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);  

  if (fpc_total>1.0)
   foreachpft(pft,p,&stand->pftlist)
    adjust(&stand->soil.litter,pft,fpc_type[pft->par->type],FPC_MAX);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);

  if (fpc_total>1.0)
    foreachpft(pft,p,&stand->pftlist)
      reduce(&stand->soil.litter,pft,fpc_total);
  stand->cell->balance.estab_storage_tree[irrigation->irrigation]-=acflux_estab*stand->frac;
  acflux_estab=0;

  stand->cell->output.flux_estab+=acflux_estab*stand->frac;
  stand->cell->output.dcflux-=acflux_estab*stand->frac;

  foreachpft(pft,p,&stand->pftlist)
    if(istree(pft))
    {
      treepar=pft->par->data;
      if (stand->age>=treepar->max_rotation_length) 
        isdead=TRUE;
    }

  stand->cell->output.cftfrac[rbtree(ncft)+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=stand->cell->ml.landfrac[irrigation->irrigation].biomass_tree;

  free(present);
  free(fpc_type);
  free(n_est);
  if(pft_len>0) free(fpc_inc2);
  if(isdead)
  {
    cutpfts(stand);
    stand->age=stand->growing_time=0;
    if(irrigation->irrigation)
    {
      stand->cell->discharge.dmass_lake+=(irrigation->irrig_stor+irrigation->irrig_amount)*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(irrigation->irrig_stor+irrigation->irrig_amount)*stand->frac;
      /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
      stand->cell->discharge.dmass_lake+=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*irrigation->conv_evap*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*irrigation->conv_evap*stand->frac;
      stand->cell->output.mstor_return+=(irrigation->irrig_stor+irrigation->irrig_amount)*stand->frac;
      stand->cell->output.aconv_loss_evap-=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*irrigation->conv_evap*stand->frac;
      stand->cell->output.aconv_loss_drain-=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*(1-irrigation->conv_evap)*stand->frac;

      if(config->pft_output_scaled)
      {
        stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*irrigation->conv_evap*stand->cell->ml.landfrac[1].biomass_tree;
        stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*(1-irrigation->conv_evap)*stand->cell->ml.landfrac[1].biomass_tree;
      }
      else
      {
        stand->cell->output.cft_conv_loss_evap[rbtree(ncft)+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*irrigation->conv_evap;
        stand->cell->output.cft_conv_loss_drain[rbtree(ncft)+irrigation->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]-=(irrigation->irrig_stor+irrigation->irrig_amount)*(1/irrigation->ec-1)*(1-irrigation->conv_evap);
      }

      irrigation->irrig_stor=0;
      irrigation->irrig_amount=0;
    }
    if(setaside(stand->cell,stand,config->pftpar,intercrop,npft,irrigation->irrigation,year))
      return TRUE;
  }
  else
    stand->cell->output.soil_storage+=(irrigation->irrig_stor+irrigation->irrig_amount)*stand->frac*stand->cell->coord.area;
  stand->age++;
  stand->growing_time++;

  return FALSE;
} /* of 'annual_biomass_tree' */
