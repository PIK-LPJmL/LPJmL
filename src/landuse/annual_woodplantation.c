/**************************************************************************************/
/**                                                                                \n**/
/**     a  n  n  u  a  l  _  w  o  o  d  p  l  a  n  t  a  t  i  o  n  .  c        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for wood plantation stand                                             \n**/
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
#include "agriculture.h"
#include "woodplantation.h"

Bool annual_woodplantation(Stand *stand,         /**< Pointer to stand */
                           int npft,             /**< number of natural pfts */
                           int ncft,             /**< number of crop PFTs */
                           int year,             /**< year (AD) */
                           Bool isdaily,         /**< daily temperature data? */
                           Bool intercrop,       /**< enable intercropping (TRUE/FALSE) */
                           const Config *config  /**< LPJ configuration */
                          )                      /** \return stand has to be killed (TRUE/FALSE) */
{
  int p,pft_len,t,outIdx,index;
  Bool *present,isdead;
  int *n_est;
  Pft *pft;
  Real *fpc_inc,*fpc_inc2,*fpc_type;
  Real fpc_total;
  Stocks flux_return;
  Stocks flux_estab={0,0};
  Stocks estab_store={0,0};
  Stocks biofuel;
  Pfttreepar *treepar;
  Biomass_tree *biomass_tree;
  Stocks yield={0.0,0.0};
  Pfttree *tree;
  
#ifdef COUPLED
  Real ftimber;
#else
  Poolpar frac;
#endif

  biomass_tree=stand->data;

  isdead=FALSE;
  index=rwp(ncft)+biomass_tree->irrigation.irrigation*getnirrig(ncft,config);
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
             pft->bm_inc.carbon,vegc_sum(pft),soilcarbon(&stand->soil));
#endif

      if(istree(pft))
      {
        treepar=pft->par->data;

        if (stand->cell->balance.estab_storage_tree[biomass_tree->irrigation.irrigation].carbon<treepar->sapling_C*stand->frac && pft->bm_inc.carbon>100)
        {
          estab_store.carbon=pft->bm_inc.carbon*0.03;
          estab_store.nitrogen=pft->bm_inc.nitrogen*0.03;
          pft->bm_inc.carbon-=estab_store.carbon;
          pft->bm_inc.nitrogen-=estab_store.nitrogen;
          stand->cell->balance.estab_storage_tree[biomass_tree->irrigation.irrigation].carbon+=estab_store.carbon*stand->frac;
          stand->cell->balance.estab_storage_tree[biomass_tree->irrigation.irrigation].nitrogen+=estab_store.nitrogen*stand->frac;
        }
      }

      if(annualpft(stand,pft,fpc_inc+p,isdaily,config))
      {
        /* PFT killed, delete from list of established PFTs */
        fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1];
        litter_update(&stand->soil.litter,pft,pft->nind,config);
        pft->nind=0;
        delpft(&stand->pftlist,p);
        p--; /* adjust loop variable */
      }

    } /* of foreachpft */
#ifdef DEBUG2
    printf("Number of updated pft: %d\n",stand->pftlist.n);
#endif

    light(stand,fpc_inc,config);
    free(fpc_inc);
  } /* END if(pft_len>0) */
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);

  // STORE WFT SPECIFIC OUTPUT
  foreachpft(pft, p, &stand->pftlist)
  {
    if (pft->par->type==TREE && pft->par->cultivation_type==WP) // tree
    {
      outIdx = pft->par->id-(npft-config->nwft-config->nagtree);
      getoutputindex(&stand->cell->output,WFT_VEGC,outIdx,config)+=vegc_sum(pft);
    }
  }
    /*
  if(setting.fire==FIRE)
  {
    fire_frac=fire_prob(&stand->soil.litter,stand->fire_sum);
    stand->cell->output.firef+=1.0/fire_frac;
    stand->cell->output.firec+=firepft(&stand->soil.litter,
                                       &stand->pftlist,fire_frac)*stand->frac;
  }
  */

#ifdef COUPLED
  if(config->sim_id==LPJML_IMAGE && stand->cell->ml.image_data->timber_frac_wp>0)
  {
    ftimber=min(1,stand->cell->ml.image_data->timber_frac_wp/stand->frac);
    foreachpft(pft,p,&stand->pftlist)
    {
      if(istree(pft))
      {
        treepar=pft->par->data;
        tree=pft->data;
        yield=timber_harvest(pft,&stand->soil,&stand->cell->ml.product,
                             stand->cell->ml.image_data->timber_f,ftimber,stand->frac,&pft->nind,
                             &biofuel,config,stand->cell->ml.image_data->timber_frac_wp,
                             stand->cell->ml.image_data->takeaway);
        pft->bm_inc.nitrogen*=(1-ftimber);
        tree->fruit.nitrogen*=(1-ftimber);
        tree->fruit.carbon*=(1-ftimber);
        getoutput(&stand->cell->output,TRAD_BIOFUEL,config)+=biofuel.carbon;
        stand->cell->balance.trad_biofuel.carbon+=biofuel.carbon;
        stand->cell->balance.trad_biofuel.nitrogen+=biofuel.nitrogen;
        getoutput(&stand->cell->output,HARVESTC,config)+=yield.carbon*stand->frac;
        getoutput(&stand->cell->output,HARVESTN,config)+=yield.nitrogen*stand->frac;
        if(config->pft_output_scaled)
        {
          stand->cell->pft_harvest[index]+=yield.carbon*stand->frac;
          getoutputindex(&stand->cell->output,PFT_HARVESTC,index,config)+=yield.carbon*stand->frac;
          getoutputindex(&stand->cell->output,PFT_HARVESTN,index,config)+=yield.nitrogen*stand->frac;
        }
        else
        {
          stand->cell->pft_harvest[index]+=yield.carbon;
          getoutputindex(&stand->cell->output,PFT_HARVESTC,index,config)+=yield.carbon;
          getoutputindex(&stand->cell->output,PFT_HARVESTN,index,config)+=yield.nitrogen;
        }
        biomass_tree->growing_time=0;
        fpc_tree(pft);
      } /* of if(istree) */
    } /* of foreachpft */
  }
#else
  if(param.ftimber_wp>0)
  {
    frac.fast=param.harvest_fast_frac;
    frac.slow=1-param.harvest_fast_frac;
    foreachpft(pft,p,&stand->pftlist)
    {
      if(istree(pft))
      {
        treepar=pft->par->data;
        tree=pft->data;
        if(biomass_tree->growing_time>=treepar->rotation && biomass_tree->growing_time%treepar->rotation==0)
        {
          yield=timber_harvest(pft,&stand->soil,frac,param.ftimber_wp,stand->frac,&pft->nind,&biofuel,config);
          pft->bm_inc.nitrogen*=(1-param.ftimber_wp);
          tree->fruit.nitrogen*=(1-param.ftimber_wp);
          tree->fruit.carbon*=(1-param.ftimber_wp);
          getoutput(&stand->cell->output,TRAD_BIOFUEL,config)+=biofuel.carbon;
          stand->cell->balance.trad_biofuel.carbon+=biofuel.carbon;
          stand->cell->balance.trad_biofuel.nitrogen+=biofuel.nitrogen;
          stand->cell->balance.timber_harvest.carbon+=yield.carbon;
          stand->cell->balance.timber_harvest.nitrogen+=yield.nitrogen;
          getoutput(&stand->cell->output,TIMBER_HARVESTC,config)+=yield.carbon;
          getoutput(&stand->cell->output,TIMBER_HARVESTN,config)+=yield.nitrogen;
          getoutput(&stand->cell->output,HARVESTC,config)+=yield.carbon;
          getoutput(&stand->cell->output,HARVESTN,config)+=yield.nitrogen;
          if(config->pft_output_scaled)
          {
            getoutputindex(&stand->cell->output,PFT_HARVESTC,index,config)+=yield.carbon;
            getoutputindex(&stand->cell->output,PFT_HARVESTN,index,config)+=yield.nitrogen;
          }
          else
          {
            getoutputindex(&stand->cell->output,PFT_HARVESTC,index,config)+=yield.carbon/stand->frac;
            getoutputindex(&stand->cell->output,PFT_HARVESTN,index,config)+=yield.nitrogen/stand->frac;
          }
          biomass_tree->growing_time=0;
          fpc_tree(pft);
        }
      } /* of if(istree) */
    } /* of foreachpft */
  }
#endif

  for(p=0;p<npft;p++)
  {
    if(establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf) &&
       ((config->pftpar[p].type==TREE && config->pftpar[p].cultivation_type==WP) ||
        (config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE)))
    {
      if(!present[p] && (estab_store.carbon<epsilon || config->pftpar[p].type!=TREE) && (fpc_type[TREE]<0.7 || config->pftpar[p].type==GRASS))
      {
        addpft(stand,config->pftpar+p,year,0,config);
        n_est[config->pftpar[p].type]++;
      }
      if(present[p])
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

  foreachpft(pft,p,&stand->pftlist){
    if(establish(stand->cell->gdd[pft->par->id],pft->par,&stand->cell->climbuf))
    {
      if (istree(pft))
      {
        treepar=pft->par->data;
        if(pft->nind<treepar->k_est && biomass_tree->age<treepar->max_rotation_length)
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
  }
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);
  if(fpc_total>1.0)
    light(stand,fpc_inc2,config);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);

  if (fpc_total>1.0)
    foreachpft(pft,p,&stand->pftlist)
     adjust(&stand->soil.litter, pft, fpc_type[pft->par->type], param.fpc_tree_max,config);
  fpc_total=fpc_sum(fpc_type,config->ntypes,&stand->pftlist);

  if (fpc_total>1.0)
    foreachpft(pft,p,&stand->pftlist)
      reduce(&stand->soil.litter,pft,fpc_total,config);
  stand->cell->balance.estab_storage_tree[biomass_tree->irrigation.irrigation].carbon-=flux_estab.carbon*stand->frac;
  stand->cell->balance.estab_storage_tree[biomass_tree->irrigation.irrigation].nitrogen-=flux_estab.nitrogen*stand->frac;
  flux_estab.carbon=flux_estab.nitrogen=0;

  getoutput(&stand->cell->output,FLUX_ESTABC,config)+=flux_estab.carbon*stand->frac;
  getoutput(&stand->cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen*stand->frac;
#ifdef COUPLED
  stand->cell->flux_estab_wp+=flux_estab.carbon*stand->frac;
#endif
  stand->cell->output.dcflux+=flux_estab.carbon*stand->frac;

  foreachpft(pft,p,&stand->pftlist)
    if(istree(pft))
    {
      treepar=pft->par->data;
      if (biomass_tree->age>=treepar->max_rotation_length)
        isdead=TRUE;
    }

  getoutputindex(&stand->cell->output,CFTFRAC,index,config)=stand->frac;
  getoutputindex(&stand->cell->output,CFT_NHARVEST,index,config)+=1.0;

  free(present);
  free(fpc_type);
  free(n_est);
  if(pft_len>0)
    free(fpc_inc2);
  if(isdead)
  {
    cutpfts(stand,config);
    biomass_tree->age=biomass_tree->growing_time=0;
    stand->cell->discharge.dmass_lake+=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*stand->cell->coord.area*stand->frac;
    stand->cell->balance.awater_flux-=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*stand->frac;
    if(setaside(stand->cell,stand,stand->cell->ml.with_tillage,intercrop,npft,ncft,biomass_tree->irrigation.irrigation,year,config))
      return TRUE;
  }
  else
  {
    stand->cell->balance.soil_storage+=(biomass_tree->irrigation.irrig_stor+biomass_tree->irrigation.irrig_amount)*stand->frac*stand->cell->coord.area;
    biomass_tree->age++;
    biomass_tree->growing_time++;
    foreachpft(pft,p,&stand->pftlist)
    {
      getoutputindex(&stand->cell->output,PFT_VEGC,getnnat(npft,config)+index,config)+=vegc_sum(pft);
      getoutputindex(&stand->cell->output,PFT_VEGN,getnnat(npft,config)+index,config)+=vegn_sum(pft)+pft->bm_inc.nitrogen;
    }
  }
  return FALSE;
} /* of 'annual_woodplantation' */
