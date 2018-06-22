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

Bool allocation_tree(Litter *litter, /**< litter pool */
                     Pft *pft,       /**< pointer to PFT */
                     Real *fpc_inc   /**< fpc increment */
                    )                /** \return TRUE on death */
{
  Real bm_inc_ind,lmtorm;
  Real cmass_loan,cmass_deficit;
  Real x1,x2;
  Treephys2 tinc_ind,tinc_ind_min;
  Pfttree *tree;
  Pfttreepar *treepar;
  Data data;
  tree=pft->data;
  if(isneg_tree(pft))
  {
    if(pft->nind>0)
    {
      bm_inc_ind=pft->bm_inc/pft->nind;
      tree->ind.root+=bm_inc_ind;
    }
    return TRUE;
  }
  bm_inc_ind=pft->bm_inc/pft->nind;
  lmtorm=getpftpar(pft,lmro_ratio)*(pft->wscal_mean/365);
  treepar=pft->par->data;
  tinc_ind.heartwood=tinc_ind.debt=0.0;
  if (lmtorm<1.0e-10) /* No leaf production possible - put all biomass 
                           into roots (Individual will die next time period)*/
  {
    tinc_ind.leaf=0.0;
    tinc_ind.root=bm_inc_ind;
    tinc_ind.sapwood=-tree->ind.sapwood;
    tinc_ind.heartwood=-tinc_ind.sapwood;
    tinc_ind.debt=0.0;
  }
  else
  {
    if (tree->height>0.0) 
    {
      tinc_ind_min.leaf=k_latosa*tree->ind.sapwood/(wooddens*tree->height*
                        pft->par->sla)-tree->ind.leaf;
      tinc_ind_min.root=k_latosa*tree->ind.sapwood/(wooddens*tree->height*
                        pft->par->sla*lmtorm)-tree->ind.root;
    }
    else 
      tinc_ind_min.leaf=tinc_ind_min.root=0.0;

    cmass_deficit=tinc_ind_min.leaf+tinc_ind_min.root-bm_inc_ind;
    if (cmass_deficit>0.0) 
    {
      cmass_loan=max(min(cmass_deficit*CDEBT_MAXLOAN_DEFICIT,
                 tree->ind.sapwood-tree->ind.debt)*CDEBT_MAXLOAN_MASS,0.0);
      bm_inc_ind+=cmass_loan;
      tinc_ind.debt=cmass_loan;
    }
    else 
      tinc_ind.debt=0.0;
  
    if (tinc_ind_min.root>=0.0 && tinc_ind_min.leaf>=0.0 &&
        (tinc_ind_min.root+tinc_ind_min.leaf<=bm_inc_ind || bm_inc_ind<=0.0)) 
    {
      data.b= tree->ind.sapwood+bm_inc_ind-tree->ind.leaf/lmtorm+
              tree->ind.root;
      data.lm=1+1/lmtorm;
      data.k1=pow(treepar->allom2,2.0/treepar->allom3)*4.0*M_1_PI/wooddens;
      data.k3=k_latosa/wooddens/pft->par->sla;
      data.ind_leaf=tree->ind.leaf;
      data.ind_heart=tree->ind.heartwood;
      data.allom3=treepar->allom3;
      x2=(bm_inc_ind-(tree->ind.leaf/lmtorm-tree->ind.root))/data.lm;
      x1= (tree->ind.leaf<1.0e-10)  ? x2/NSEG : 0;

/*  Bisection loop
 *  Search iterates on value of xmid until xmid lies within
 *  xacc of the root, i.e. until |xmid-x|<xacc where f(x)=0
 */
 
      if((x1==0 && x2==0) || data.b-x1*data.lm<0.0 || data.ind_leaf+x1<=0.0 
         || data.b-x2*data.lm<0.0 || data.ind_leaf+x2<=0.0 )
        tinc_ind.leaf=0;
      else
        tinc_ind.leaf=leftmostzero((Bisectfcn)f,x1,x2,&data,0.001,1.0e-10,40);
      if (tinc_ind.leaf<0.0)
        tinc_ind.root=0.0;
      else
        tinc_ind.root=(tinc_ind.leaf+tree->ind.leaf)/lmtorm-tree->ind.root; 
      tinc_ind.sapwood=bm_inc_ind-tinc_ind.leaf-tinc_ind.root;
    }
    else 
    {
    
/* Abnormal allocation: reduction in some biomass compartment(s) to
 * satisfy allometry
 * Attempt to distribute this year's production among leaves and roots only
 */
      tinc_ind.leaf=(bm_inc_ind+tree->ind.root-tree->ind.leaf/lmtorm)/
                    (1.0+1.0/lmtorm);
      if (tinc_ind.leaf>0.0) 
        tinc_ind.root=bm_inc_ind-tinc_ind.leaf;
      else 
      {
        tinc_ind.root=bm_inc_ind;
        tinc_ind.leaf=(tree->ind.root+tinc_ind.root)*lmtorm-tree->ind.leaf;
        litter->ag[pft->litter].trait.leaf+=-tinc_ind.leaf*pft->nind;
      }
      tinc_ind.sapwood=(tinc_ind.leaf+tree->ind.leaf)*wooddens*tree->height*
                       pft->par->sla/k_latosa-tree->ind.sapwood;
      tinc_ind.heartwood=-tinc_ind.sapwood;
    }
  }
  tree->ind.leaf+=tinc_ind.leaf;
  tree->ind.root+=tinc_ind.root;
  tree->ind.sapwood+=tinc_ind.sapwood;
  tree->ind.heartwood+=tinc_ind.heartwood;
  tree->ind.debt+=tinc_ind.debt;
  allometry_tree(pft);
  *fpc_inc=fpc_tree(pft);
  return isneg_tree(pft);
} /* of 'allocation_tree' */
