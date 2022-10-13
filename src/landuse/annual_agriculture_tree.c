/**************************************************************************************/
/**                                                                                \n**/
/**            a n n u a l _ a g r i c u l t u r e _ t r e e . c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for agriculture tree stand                                            \n**/
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
#include "agriculture_tree.h"

#define FPC_MAX 0.95

Bool annual_agriculture_tree(Stand *stand,         /**< Pointer to stand */
                             int npft,             /**< number of natural pfts */
                             int ncft,             /**< number of crop PFTs */
                             Real UNUSED(popdens), /**< population density (capita/km2) */
                             int year,             /**< year (AD) */
                             Bool isdaily,         /**< daily temperature data? */
                             Bool intercrop,       /**< enable (intercropping) (TRUE/FALSE) */
                             const Config *config  /**< LPJ configuration */
                            )                      /** \return stand has to be killed (TRUE/FALSE) */
{
  int p,pft_len,t,index;
  Bool *present,isdead,nnat;
  int *n_est;
  Pft *pft;
  Real *fpc_inc,*fpc_inc2,*fpc_type;
  Real fpc_total;
  Stocks flux_estab={0,0};
  Stocks flux_return;
  Stocks estab_store={0,0};
  Stocks yield={0.0,0};
  Pfttreepar *treepar;
  Pfttree *tree;
  Biomass_tree *data;
  Real k_est_thiscell,sapling_C_thiscell;
  data=stand->data;

  if(!strcmp(config->pftpar[data->irrigation.pft_id].name,"cotton"))
  {
    if(getnpft(&stand->pftlist)>0)
      stand->cell->balance.soil_storage+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->frac*stand->cell->coord.area;
    return FALSE;
  }
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
  nnat=getnnat(npft,config);
  index=agtree(ncft,config->nwptype)+data->irrigation.pft_id-npft+config->nagtree+data->irrigation.irrigation*getnirrig(ncft,config);
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
      //printf("PFT=%s, fpc=%g\n",pft->par->name,pft->fpc);
      if(istree(pft))
      {
        treepar=pft->par->data;

        //printf("PFT:%s height:%g lai %g fpc %g stemdiam %g leafC %g sapC %g heartC %g crownarea %g debt %g\n",pft->par->name, tree->height,
        //       lai_tree(pft), pft->fpc, pow(tree->height/20,1.0/4), tree->ind.leaf.carbon, tree->ind.sapwood.carbon, tree->ind.heartwood.carbon, tree->crownarea, tree->ind.debt.carbon);
        /*Allocating cell specified k_est and sapling_C*/
        if(pft->stand->cell->ml.manage.k_est[pft->par->id]>0)
          k_est_thiscell=pft->stand->cell->ml.manage.k_est[pft->par->id];
        else
          k_est_thiscell=treepar->k_est;
        sapling_C_thiscell=phys_sum_tree(treepar->sapl)*k_est_thiscell;

        if (stand->cell->balance.estab_storage_tree[data->irrigation.irrigation].carbon<sapling_C_thiscell*stand->frac && pft->bm_inc.carbon>100)
        {
          estab_store.carbon=pft->bm_inc.carbon*0.03;
          estab_store.nitrogen=pft->bm_inc.nitrogen*0.03;
          pft->bm_inc.carbon-=estab_store.carbon;
          pft->bm_inc.nitrogen-=estab_store.nitrogen;
          stand->cell->balance.estab_storage_tree[data->irrigation.irrigation].carbon+=estab_store.carbon*stand->frac;
          stand->cell->balance.estab_storage_tree[data->irrigation.irrigation].nitrogen+=estab_store.nitrogen*stand->frac;
        }
      }

      if(annualpft(stand,pft,fpc_inc+p,isdaily,config))
      {
        /* PFT killed, delete from list of established PFTs */
        fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1];
        litter_update(&stand->soil.litter,pft,pft->nind,config);
        delpft(&stand->pftlist,p);
        p--; /* adjust loop variable */
        continue;
      }
      if(istree(pft))
      {
        if(data->age>treepar->rotation)
        {
          yield=harvest_tree(pft);
          //printf("%s yield %s=%g t/ha, %g indiv/ha, wstress=%g, fpc=%g\n",(data->irrigation.irrigation) ? "irrigated" :"",pft->par->name,yield.carbon*1e4/1e6/0.45,pft->nind*1e4,pft->wscal_mean/365,pft->fpc);
          //printf("index=%d, yield=%g\n",index,yield);
          if(config->pft_output_scaled)
          {
#if defined IMAGE && defined COUPLED
            stand->cell->pft_harvest[index]+=yield.carbon*stand->frac;
#endif
            getoutputindex(&stand->cell->output,PFT_HARVESTC,index,config)+=yield.carbon*stand->frac;
            getoutputindex(&stand->cell->output,PFT_HARVESTN,index,config)+=yield.nitrogen*stand->frac;
          }
          else
          {
#if defined IMAGE && defined COUPLED
            stand->cell->pft_harvest[index]+=yield.carbon;
#endif
            getoutputindex(&stand->cell->output,PFT_HARVESTC,index,config)+=yield.carbon;
            getoutputindex(&stand->cell->output,PFT_HARVESTN,index,config)+=yield.nitrogen;
          }
          stand->cell->balance.biomass_yield.carbon+=yield.carbon*stand->frac;
          stand->cell->balance.biomass_yield.nitrogen+=yield.nitrogen*stand->frac;
          stand->cell->output.dcflux+=yield.carbon*stand->frac;
        }
      }

    } /* of foreachpft */
#ifdef DEBUG2
    printf("Number of updated pft: %d\n",stand->pftlist.n);
#endif

    light(stand,fpc_inc,config);
    free(fpc_inc);
  } /* END if(pft_len>0) */

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
    present[pft->par->id]=TRUE;
  treepar=config->pftpar[data->irrigation.pft_id].data;
  for(p=0;p<npft;p++)
  {
    if(establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf) &&
       (config->pftpar[p].id==data->irrigation.pft_id ||
       (treepar->with_grass && config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE)))
    {
      if(!present[p])
        addpft(stand,config->pftpar+p,year,0,config);
      n_est[config->pftpar[p].type]++;
    }
  }
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  pft_len=getnpft(&stand->pftlist);
  if(pft_len>0)
  {
    fpc_inc2=newvec(Real,pft_len);
    check(fpc_inc2);
  }
  foreachpft(pft,p,&stand->pftlist)
    fpc_inc2[p]=0;

  foreachpft(pft,p,&stand->pftlist)
    if(establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf))
    {
      if (istree(pft))
      {
        treepar=pft->par->data;
        k_est_thiscell=stand->cell->ml.manage.k_est[pft->par->id];
        if(k_est_thiscell<0)
          k_est_thiscell=treepar->k_est;
        if(pft->nind<k_est_thiscell)
        {
          flux_return=establishment(pft,fpc_total,fpc_type[pft->par->type],
                                    n_est[pft->par->type]);
          flux_estab.carbon+=flux_return.carbon;
          flux_estab.nitrogen+=flux_return.nitrogen;
          fpc_inc2[p]=pft->fpc+fpc_total-1;
        }
      }
      else
      {
        flux_return=establishment(pft,fpc_total,fpc_type[pft->par->type],
                                  n_est[pft->par->type]);
        flux_estab.carbon+=flux_return.carbon;
        flux_estab.nitrogen+=flux_return.nitrogen;
      }
    }
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if(fpc_total>1.0)
    light(stand,fpc_inc2,config);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if (fpc_total>1.0)
    foreachpft(pft,p,&stand->pftlist)
      adjust(&stand->soil.litter,pft,fpc_type[pft->par->type],FPC_MAX,config);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if (fpc_total>1.0)
    foreachpft(pft,p,&stand->pftlist)
      if(pft->par->type==GRASS)
        reduce(&stand->soil.litter,pft,fpc_total,config);
  stand->cell->balance.estab_storage_tree[data->irrigation.irrigation].carbon-=flux_estab.carbon*stand->frac;
  stand->cell->balance.estab_storage_tree[data->irrigation.irrigation].nitrogen-=flux_estab.nitrogen*stand->frac;
  flux_estab.carbon=flux_estab.nitrogen=0;

  stand->cell->balance.flux_estab.carbon+=flux_estab.carbon*stand->frac;
  stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
  stand->cell->output.dcflux-=flux_estab.carbon*stand->frac;

  foreachpft(pft,p,&stand->pftlist)
    if(istree(pft))
    {
      treepar=pft->par->data;
      if (data->age>=treepar->max_rotation_length)
        isdead=TRUE;
    }

  getoutputindex(&stand->cell->output,CFTFRAC,index,config)+=stand->frac;
  getoutputindex(&stand->cell->output,CFT_NHARVEST,index,config)+=1.0;

  free(present);
  free(fpc_type);
  if(pft_len>0)
    free(fpc_inc2);
  free(n_est);
  if(isdead)
  {
    update_irrig(stand,agtree(ncft,config->nwptype)+data->irrigation.pft_id-npft,ncft,config);
    if(setaside(stand->cell,stand,stand->cell->ml.with_tillage,intercrop,npft,data->irrigation.irrigation,year,config))
      return TRUE;
  }
  else
  {
    stand->cell->balance.soil_storage+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->frac*stand->cell->coord.area;
    data->age++;
    data->growing_time++;
  }
  foreachpft(pft,p,&stand->pftlist)
  {
    if(pft->par->type==TREE)
    {
      tree=pft->data;
      getoutputindex(&stand->cell->output,PFT_VEGC,nnat+index,config)+=vegc_sum(pft);
      getoutputindex(&stand->cell->output,PFT_VEGN,nnat+index,config)+=vegn_sum(pft)+pft->bm_inc.nitrogen;
      getoutputindex(&stand->cell->output,PFT_CROOT,nnat+index,config)+=tree->ind.root.carbon;
      getoutputindex(&stand->cell->output,PFT_NROOT,nnat+index,config)+=tree->ind.root.nitrogen;
      getoutputindex(&stand->cell->output,PFT_CLEAF,nnat+index,config)+=tree->ind.leaf.carbon;
      getoutputindex(&stand->cell->output,PFT_NLEAF,nnat+index,config)+=tree->ind.leaf.nitrogen;
    }
  }
  return FALSE;
} /* of 'annual_agriculture_tree' */
