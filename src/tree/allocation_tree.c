/**************************************************************************************/
/**                                                                                \n**/
/**        a  l  l  o  c  a  t  i  o  n  _  t  r  e  e  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**             TREE ALLOCATION                                                    \n**/
/**                                                                                \n**/
/**     Allocation of this year's biomass increment (bm_inc_ind) to the            \n**/
/**     three living carbon pools, such that the basic allometric                  \n**/
/**     relationships (A-C below) are always satisfied.                            \n**/
/**                                                                                \n**/
/**     (A) (leaf area) = latosa * (sapwood xs area)                               \n**/
/**         (Pipe Model, Shinozaki et al. 1964a,b; Waring et al 1982)              \n**/
/**     (B) (leaf mass) = lmtorm * (root mass)                                     \n**/
/**     (C) height = allom2 * (stem diameter)**allom3                              \n**/
/**         (source?)                                                              \n**/
/**     (D) (crown area) = min (allom1 * (stem diameter)**reinickerp,              \n**/
/**                             crownarea_max)                                     \n**/
/**                                                                                \n**/
/**     Mathematical derivation:                                                   \n**/
/**                                                                                \n**/
/**       (1) bm_inc_ind = lminc_ind + sminc_ind + rminc_ind                       \n**/
/**       (2) leaf_area_new = latosa * sap_xsa_new   [from (A)]                    \n**/
/**       (3) leaf_area_new = (lm_ind + lminc_ind) * sla                           \n**/
/**     from (2) & (3),                                                            \n**/
/**       (4) (lm_ind + lminc_ind) * sla = latosa * sap_xsa_new                    \n**/
/**     from (4),                                                                  \n**/
/**       (5) sap_xsa_new = (lm_ind + lminc_ind) * sla / latosa                    \n**/
/**       (6) (lm_ind + lminc_ind) = lmtorm * (rm_ind + rminc_ind)                 \n**/
/**             [from (B)]                                                         \n**/
/**       (7) height_new = allom2 * stemdiam_new**allom3  [from (C)]               \n**/
/**     from (1),                                                                  \n**/
/**       (8) sminc_ind = bm_inc_ind - lminc_ind - rminc_ind                       \n**/
/**     from (6),                                                                  \n**/
/**       (9) rminc_ind=((lm_ind + lminc_ind) / lmtorm) - rm_ind                   \n**/
/**     from (8) & (9),                                                            \n**/
/**      (10) sminc_ind = bm_inc_ind - lminc_ind                                   \n**/
/**             - ((lm_ind + lminc_ind)  / lmtorm) + rm_ind                        \n**/
/**      (11) wooddens = (sm_ind + sminc_ind + hm_ind) / stemvolume_new            \n**/
/**      (12) stemvolume_new = height_new * pi * stemdiam_new**2 / 4               \n**/
/**     from (10), (11) & (12)                                                     \n**/
/**      (13) stemdiam_new = [ ((sm_ind + bm_inc_ind - lminc_ind                   \n**/
/**             - ((lm_ind + lminc_ind) / lmtorm) + rm_ind + hm_ind)               \n**/
/**             / wooddens) / (height_new * pi / 4) ]**(1/2)                       \n**/
/**     combining (7) and (13),                                                    \n**/
/**      (14) height_new = allom2 * [ ((sm_ind + bm_inc_ind - lminc_ind            \n**/
/**             - ((lm_ind + lminc_ind) / lmtorm) + rm_ind + hm_ind)               \n**/
/**             / wooddens) / (height_new * pi / 4) ]**(1/2 * allom3)              \n**/
/**     from (14),                                                                 \n**/
/**      (15) height_new**(1 + 2 / allom3) = allom2**(2 / allom3)                  \n**/
/**            * ((sm_ind + bm_inc_ind - lminc_ind - ((lm_ind + lminc_ind)         \n**/
/**            / lmtorm) + rm_ind + hm_ind) / wooddens) / (pi / 4)                 \n**/
/**      (16) wooddens = (sm_ind + sminc_ind) / sapvolume_new                      \n**/
/**     from (10) and (16),                                                        \n**/
/**      (17) wooddens = (sm_ind + bm_inc_ind - lminc_ind                          \n**/
/**             - ((lm_ind + lminc_ind) / lmtorm) + rm_ind)                        \n**/
/**             / (height_new * wooddens)                                          \n**/
/**     from (19),                                                                 \n**/
/**      (20) height_new = (sm_ind + bm_inc_ind - lminc_ind                        \n**/
/**             - ((lm_ind + lminc_ind) / lmtorm) + rm_ind )                       \n**/
/**             / (sap_xsa_new * wooddens)                                         \n**/
/**     from (5) and (20),                                                         \n**/
/**      (21) height_new**(1 + 2 / allom3) = [ (sm_ind + bm_inc_ind                \n**/
/**             - lminc_ind - ((lm_ind + lminc_ind) / lmtorm) + rm_ind )           \n**/
/**             / ((lm_ind + lminc_ind) * sla * wooddens / latosa) ]               \n**/
/**             **(1 + 2 / allom3)                                                 \n**/
/**     ----------------------------------------------------------------           \n**/
/**      (15) and (21) are two alternative expressions for                         \n**/
/**           height_new**(1 + 2 / allom3). Combining these,                       \n**/
/**                                                                                \n**/
/**      (22) allom2**(2 / allom3) * ((sm_ind + bm_inc_ind - lminc_ind             \n**/
/**             - ((lm_ind + lminc_ind) / lmtorm) + rm_ind + hm_ind)               \n**/
/**           / wooddens) / (pi / 4) - [ (sm_ind + bm_inc_ind - lminc_ind          \n**/
/**             - ((lm_ind + lminc_ind) / lmtorm) + rm_ind )                       \n**/
/**             / ((lm_ind + lminc_ind) * sla * wooddens / latosa) ]               \n**/
/**             **(1 + 2 / allom3)                                                 \n**/
/**             = 0                                                                \n**/
/**                                                                                \n**/
/**     Equation (22) can be expressed in the form f(lminc_ind)=0.                 \n**/
/**                                                                                \n**/
/**     Numerical methods are used to solve the equation for the                   \n**/
/**     unknown lminc_ind.                                                         \n**/
/**     -----------------------------------------------------------------          \n**/
/**     Work out minimum leaf production to maintain current sapmass               \n**/
/**                                                                                \n**/
/**      (23) sap_xsa = sm_ind / wooddens / height                                 \n**/
/**     from (A) and (23),                                                         \n**/
/**      (24) leaf_mass * sla = latosa * sap_mass / wooddens / height              \n**/
/**     from (24),                                                                 \n**/
/**      (25) leaf_mass = latosa * sap_mass / (wooddens * height * sla)            \n**/
/**     from (25), assuming sminc_ind=0,                                           \n**/
/**      (26) lm_ind + lminc_ind_min = latosa * sm_ind                             \n**/
/**             / (wooddens * height * sla)                                        \n**/
/**     from (26),                                                                 \n**/
/**      (27) lminc_ind_min = latosa * sm_ind / (wooddens * height * sla)          \n**/
/**             - lm_ind                                                           \n**/
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

#define CDEBT_MAXLOAN_DEFICIT 0.8 /* maximum loan as a fraction of deficit*/
#define CDEBT_MAXLOAN_MASS 0.2 /* maximum loan as a fraction of (sapwood-cdebt)*/
#define NSEG 20 /* number of segments (parameter in numerical methods)*/

typedef struct
{
  Real k1,lm,k3,b,ind_leaf,ind_heart,allom3;
} Data;

static Real f(Real leaf_inc,Data *data) 
{
 return data->k1*(data->b-leaf_inc*data->lm+data->ind_heart)-
         pow((data->b-leaf_inc*data->lm)/(data->ind_leaf+leaf_inc)*data->k3,
             1.0+2/data->allom3);
} /* of 'f' */

Bool allocation_tree(Litter *litter,   /**< litter pool */
                     Pft *pft,         /**< pointer to PFT */
                     Real *fpc_inc,    /**< fpc increment */
                     const Config *config /**< LPJmL configuration */
                    )                  /** \return TRUE on death */
{
  Stocks bm_inc_ind={0,0};
  Real lmtorm;
  Real cmass_loan,cmass_deficit;
  Real x1,x2;
  Treephys2 tinc_ind,tinc_ind_min;
  Pfttree *tree;
  Pfttreepar *treepar;
  Data data;
  Real vscal,wscal;
  Real csapwood,croot,cleaf,a,b,c,nitrogen_before;
  tree=pft->data;
  /* add excess carbon from last year to bm_inc to have this allocated now */
  pft->bm_inc.carbon+=tree->excess_carbon*pft->nind;
  tree->excess_carbon=0;
  if(isneg_tree(pft))
  {
    if(pft->nind!=0)
    {
      bm_inc_ind.carbon=pft->bm_inc.carbon/pft->nind;
      tree->ind.root.carbon+=bm_inc_ind.carbon;
      pft->bm_inc.carbon=0;
    }
    return TRUE;
  }
  if(!strcmp(pft->par->name,"cotton"))
  {
    wscal=pft->wscal_mean/(Real)pft->stand->growing_days;
    vscal=(config->with_nitrogen) ? min(1,pft->vscal/(Real)pft->stand->growing_days) : 1;
    lmtorm=getpftpar(pft,lmro_ratio)*(getpftpar(pft,lmro_offset)+(1-getpftpar(pft,lmro_offset))*min(vscal,wscal));
  }
  else
  {
    vscal=(config->with_nitrogen) ? min(1,pft->vscal/NDAYYEAR) : 1;
    wscal=pft->wscal_mean/NDAYYEAR;
    lmtorm=getpftpar(pft,lmro_ratio)*(getpftpar(pft,lmro_offset)+(1-getpftpar(pft,lmro_offset))*min(vscal,wscal));
  }
  bm_inc_ind.carbon=pft->bm_inc.carbon/pft->nind;
  bm_inc_ind.nitrogen=pft->bm_inc.nitrogen/pft->nind;
  treepar=pft->par->data;
  tinc_ind.heartwood.carbon=tinc_ind.debt.carbon=0.0;
  tinc_ind.heartwood.nitrogen=tinc_ind.debt.nitrogen=0.0;
  if (lmtorm<1.0e-10) /* No leaf production possible - put all biomass 
                           into roots (Individual will die next time period)*/
  {
    tinc_ind.leaf.carbon=0.0;
    tinc_ind.root.carbon=bm_inc_ind.carbon;
    tinc_ind.sapwood.carbon=-tree->ind.sapwood.carbon;
    tinc_ind.heartwood.carbon=-tinc_ind.sapwood.carbon;
    tinc_ind.debt.carbon=0.0;
  }
  else
  {
    if (tree->height>0.0) 
    {
      if (pft->stand->type->landusetype==BIOMASS_TREE && tree->ind.root.carbon>=tree->ind.leaf.carbon/lmtorm &&
          treepar->k_latosa*tree->ind.sapwood.carbon/(treepar->wood_density*tree->height*pft->par->sla)-tree->ind.leaf.carbon>=0.0)
      {
        tinc_ind_min.leaf.carbon=0.1;
        tinc_ind_min.root.carbon=0.1;

      }
      else
      {
        tinc_ind_min.leaf.carbon=treepar->k_latosa*tree->ind.sapwood.carbon/(treepar->wood_density*tree->height*
                        pft->par->sla)-tree->ind.leaf.carbon;
        tinc_ind_min.root.carbon=treepar->k_latosa*tree->ind.sapwood.carbon/(treepar->wood_density*tree->height*
                        pft->par->sla*lmtorm)-tree->ind.root.carbon;
      }
    }
    else 
    {
      tinc_ind_min.leaf.carbon=tinc_ind_min.root.carbon=0.0;
      tinc_ind_min.leaf.nitrogen=tinc_ind_min.root.nitrogen=0.0;
    }

    cmass_deficit=tinc_ind_min.leaf.carbon+tinc_ind_min.root.carbon-bm_inc_ind.carbon;
    if (cmass_deficit>0.0) 
    {
      cmass_loan=max(min(cmass_deficit*CDEBT_MAXLOAN_DEFICIT,
                 tree->ind.sapwood.carbon-tree->ind.debt.carbon)*CDEBT_MAXLOAN_MASS,0.0);
      bm_inc_ind.carbon+=cmass_loan;
      tinc_ind.debt.carbon=cmass_loan;
    }
    else 
      tinc_ind.debt.carbon=0.0;
  
    if (tinc_ind_min.root.carbon>=0.0 && tinc_ind_min.leaf.carbon>=0.0 &&
        (tinc_ind_min.root.carbon+tinc_ind_min.leaf.carbon<=bm_inc_ind.carbon || bm_inc_ind.carbon<=0.0)) 
    {
      data.b= tree->ind.sapwood.carbon+bm_inc_ind.carbon-tree->ind.leaf.carbon/lmtorm+
              tree->ind.root.carbon;
      data.lm=1+1/lmtorm;
      data.k1=pow(treepar->allom2,2.0/treepar->allom3)*4.0*M_1_PI/treepar->wood_density;
      data.k3=treepar->k_latosa/treepar->wood_density/pft->par->sla;
      data.ind_leaf=tree->ind.leaf.carbon;
      data.ind_heart=tree->ind.heartwood.carbon;
      data.allom3=treepar->allom3;
      x2=(bm_inc_ind.carbon-(tree->ind.leaf.carbon/lmtorm-tree->ind.root.carbon))/data.lm;
      x1= (tree->ind.leaf.carbon<1.0e-10)  ? x2/NSEG : 0;

/*  Bisection loop
 *  Search iterates on value of xmid until xmid lies within
 *  xacc of the root, i.e. until |xmid-x|<xacc where f(x)=0
 */
 
      if((x1==0 && x2==0) || data.b-x1*data.lm<0.0 || data.ind_leaf+x1<=0.0 
         || data.b-x2*data.lm<0.0 || data.ind_leaf+x2<=0.0 )
        tinc_ind.leaf.carbon=0;
      else
        tinc_ind.leaf.carbon=leftmostzero((Bisectfcn)f,x1,x2,&data,0.001,1.0e-10,40);
      if (tinc_ind.leaf.carbon<0.0)
        tinc_ind.root.carbon=0.0;
      else
        tinc_ind.root.carbon=(tinc_ind.leaf.carbon+tree->ind.leaf.carbon)/lmtorm-tree->ind.root.carbon; 
      if(bm_inc_ind.carbon>0 && tinc_ind.root.carbon+tinc_ind.leaf.carbon>bm_inc_ind.carbon)
      {
        tinc_ind.root.carbon=bm_inc_ind.carbon*tinc_ind.root.carbon/(tinc_ind.root.carbon+tinc_ind.leaf.carbon);
        tinc_ind.leaf.carbon=bm_inc_ind.carbon*tinc_ind.leaf.carbon/(tinc_ind.root.carbon+tinc_ind.leaf.carbon);
      }
      tinc_ind.sapwood.carbon=bm_inc_ind.carbon-tinc_ind.leaf.carbon-tinc_ind.root.carbon;
    }
    else 
    {
    
/* Abnormal allocation: reduction in some biomass compartment(s) to
 * satisfy allometry
 * Attempt to distribute this year's production among leaves and roots only
 */
      tinc_ind.leaf.carbon=(bm_inc_ind.carbon+tree->ind.root.carbon-tree->ind.leaf.carbon/lmtorm)/
                    (1.0+1.0/lmtorm);
      if (tinc_ind.leaf.carbon>0.0) 
        tinc_ind.root.carbon=bm_inc_ind.carbon-tinc_ind.leaf.carbon;
      else 
      {
        tinc_ind.root.carbon=bm_inc_ind.carbon;
        tinc_ind.leaf.carbon=(tree->ind.root.carbon+tinc_ind.root.carbon)*lmtorm-tree->ind.leaf.carbon;
        litter->item[pft->litter].agtop.leaf.carbon+=-tinc_ind.leaf.carbon*pft->nind;
        getoutput(&pft->stand->cell->output,LITFALLC,config)+=-tinc_ind.leaf.carbon*pft->nind*pft->stand->frac;
      }
      tinc_ind.sapwood.carbon=(tinc_ind.leaf.carbon+tree->ind.leaf.carbon)*treepar->wood_density*tree->height*
                       pft->par->sla/treepar->k_latosa-tree->ind.sapwood.carbon;
      tinc_ind.heartwood.carbon=-tinc_ind.sapwood.carbon;
    }
  }

  if(bm_inc_ind.carbon>0 && tinc_ind.leaf.carbon>0 && tinc_ind.sapwood.carbon>0 && tinc_ind.root.carbon>0)
  {
    tree->falloc.leaf=tinc_ind.leaf.carbon/bm_inc_ind.carbon;
    tree->falloc.root=tinc_ind.root.carbon/bm_inc_ind.carbon;
    tree->falloc.sapwood=tinc_ind.sapwood.carbon/bm_inc_ind.carbon;
  }
  tree->ind.leaf.carbon+=tinc_ind.leaf.carbon;
  tree->ind.root.carbon+=tinc_ind.root.carbon;
  tree->ind.sapwood.carbon+=tinc_ind.sapwood.carbon;
  tree->ind.heartwood.carbon+=tinc_ind.heartwood.carbon;
  tree->ind.debt.carbon+=tinc_ind.debt.carbon;
  /* all carbon from bm_inc_ind.carbon has been allocated according to rules*/
  /* check if there is too much carbon for allowed CN ratios and eventually put
     some carbon back to bm_inc_ind.carbon */
  if(config->with_nitrogen)
  {

    /* nitrogen allocation */
    if(tree->ind.leaf.carbon>0 && bm_inc_ind.nitrogen>0)
    {
      nitrogen_allocation_tree(&a,&b,&c,tree->ind.leaf,tree->ind.root,tree->ind.sapwood,
                               treepar->ratio.root,treepar->ratio.sapwood,bm_inc_ind.nitrogen);
      /* check if there is too much nitrogen, fill up leaves, roots sapwood to max allowed */
      if((tree->ind.leaf.nitrogen+a*bm_inc_ind.nitrogen)/tree->ind.leaf.carbon>pft->par->ncleaf.high)
      {
        nitrogen_before=tree->ind.leaf.nitrogen;
        tree->ind.leaf.nitrogen=tree->ind.leaf.carbon*pft->par->ncleaf.high;
        bm_inc_ind.nitrogen-=tree->ind.leaf.nitrogen-nitrogen_before;
        if(bm_inc_ind.nitrogen>=tree->ind.root.carbon*pft->par->ncleaf.high/treepar->ratio.root-tree->ind.root.nitrogen)
        {
          nitrogen_before=tree->ind.root.nitrogen;
          tree->ind.root.nitrogen=tree->ind.root.carbon*pft->par->ncleaf.high/treepar->ratio.root;
          bm_inc_ind.nitrogen-=tree->ind.root.nitrogen-nitrogen_before;
        }
        else
        {
          tree->ind.root.nitrogen+= bm_inc_ind.nitrogen;
          bm_inc_ind.nitrogen=0;
        }
        if( bm_inc_ind.nitrogen>=tree->ind.sapwood.carbon*pft->par->ncleaf.high/treepar->ratio.sapwood-tree->ind.sapwood.nitrogen)
        {
          nitrogen_before=tree->ind.sapwood.nitrogen;
          tree->ind.sapwood.nitrogen=tree->ind.sapwood.carbon*pft->par->ncleaf.high/treepar->ratio.sapwood;
          bm_inc_ind.nitrogen-=tree->ind.sapwood.nitrogen-nitrogen_before;
        }
        else
        {
          tree->ind.sapwood.nitrogen+= bm_inc_ind.nitrogen;
          bm_inc_ind.nitrogen=0;
        }
      }
      else /* not too much nitrogen, allocate normally */
      {
        tree->ind.leaf.nitrogen+=a*bm_inc_ind.nitrogen;
        tree->ind.root.nitrogen+=b*bm_inc_ind.nitrogen;
        tree->ind.sapwood.nitrogen+=c*bm_inc_ind.nitrogen;
        bm_inc_ind.nitrogen=0;
        /* testing if there is too much carbon for allowed NC ratios */
        if(tree->ind.leaf.nitrogen/tree->ind.leaf.carbon<pft->par->ncleaf.low)
        {
          cleaf=tree->ind.leaf.nitrogen/pft->par->ncleaf.low;
          tree->excess_carbon+=(tree->ind.leaf.carbon-cleaf);
          tree->ind.leaf.carbon=cleaf;
        }
        if(tree->ind.root.carbon>0 && tree->ind.root.nitrogen/tree->ind.root.carbon<pft->par->ncleaf.low/treepar->ratio.root)
        {
          croot=tree->ind.root.nitrogen/pft->par->ncleaf.low*treepar->ratio.root;
          tree->excess_carbon+=(tree->ind.root.carbon-croot);
          tree->ind.root.carbon=croot;
        }
        if(tree->ind.sapwood.carbon>0 && tree->ind.sapwood.nitrogen/tree->ind.sapwood.carbon<pft->par->ncleaf.low/treepar->ratio.sapwood)
        {
          csapwood=tree->ind.sapwood.nitrogen/pft->par->ncleaf.low*treepar->ratio.sapwood;
          tree->excess_carbon+=(tree->ind.sapwood.carbon-csapwood);
          tree->ind.sapwood.carbon=csapwood;
        }
      }
    }
    pft->bm_inc.nitrogen=bm_inc_ind.nitrogen*pft->nind;
  } /* of config->with_nitrogen */
  if(tree->ind.leaf.carbon<=0 && tree->ind.root.carbon>0)   //QUICK-FIX very occasional it happens that leaf carbon is ZERO and a lot of carbon is in roots
  {
      tree->ind.leaf.carbon+=tree->ind.root.carbon/lmtorm;
      tree->ind.root.carbon-=tree->ind.root.carbon/lmtorm;
  }
  pft->bm_inc.carbon=0;
  allometry_tree(pft);
  *fpc_inc=fpc_tree(pft);
  return isneg_tree(pft);
} /* of 'allocation_tree' */
