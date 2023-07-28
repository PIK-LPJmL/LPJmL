/**************************************************************************************/
/**                                                                                \n**/
/**           a l l o c a t i o n _ d a i l y _ c r o p . c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

#define FROOTMAX 0.4
#define FROOTMIN 0.3 /* learning from AgMIP wheat and maize pilots */
#define ratio_so_leaf 0.5


static void solve(Real *a,Real *b,Real *c,Real *d,
                  Real n1,Real n2,Real n3,Real n4,
                  Real c1,Real c2,Real c3,Real c4,
                  Real r1,Real r2,Real r3,Real m)
{
  Real nominator;
  nominator=m*(c1*r1*r2*r3+c2*r2*r3+c3*r1*r3+c4*r1*r2);
  *a=(c1*m*r1*r2*r3+c1*n2*r1*r2*r3+c1*n3*r1*r2*r3+c1*n4*r1*r2*r3-c2*n1*r2*r3-c3*n1*r1*r3-c4*n1*r1*r2)/nominator;
  *b=(-c1*n2*r1*r2*r3+c2*m*r2*r3+c2*n1*r2*r3+c2*n3*r2*r3+c2*n4*r2*r3-c3*n2*r1*r3-c4*n2*r1*r2)/nominator;
  *c=(-c1*n3*r1*r2*r3-c2*n3*r2*r3+c3*m*r1*r3+c3*n1*r1*r3+c3*n2*r1*r3+c3*n4*r1*r3-c4*n3*r1*r2)/nominator;
  *d=(-c1*n4*r1*r2*r3-c2*n4*r2*r3-c3*n4*r1*r3+c4*m*r1*r2+c4*n1*r1*r2+c4*n2*r1*r2+c4*n3*r1*r2)/nominator;
}

void allocation_daily_crop(Pft *pft,             /**< PFT variables */
                           Real npp,             /**< net primary production (gC/m2/day) */
                           Real wdf,             /**< water deficit fraction */
                           const Config *config  /**< LPJmL configuration */
                          )
{
  Pftcrop *crop;
  Pftcroppar *par;
  Real froot;
  Real a,b,c,d,nitrogen_before;
  /*Real nf;*/
  /*Real ndf,ndemand_crop_max,sNO3,ndeficit;*/
  Real fhiopt,himind,hi,hiopt=0;
  /*Real leaf_nitrogen_lastday;*/
  Real ndf=100; /* nitrogen deficit factor in percent, computed as wdf from accumulated n_demand and n_uptake */
  Real df;
  /*int l;*/
  crop=pft->data;
  par=pft->par->data;
  /* vegetation growth */
  pft->bm_inc.carbon+=npp;

  /* root growth */
  if(config->with_nitrogen)
  {
   // ndf=(crop->ndemandsum>0.0) ? 100.0*crop->nuptakesum/crop->ndemandsum: 100.0;
    crop->vscal_sum+=pft->vscal;
    ndf=(crop->growingdays>0.0) ? crop->vscal_sum/crop->growingdays*100 : 100.0;
  }
  /* this work also without with_nitrogen, as ndf is initialized to 100 */
  df=min(wdf,ndf);
  //df=wdf;
  froot=FROOTMAX-(FROOTMIN*crop->fphu)*df/(df+exp(6.13-0.0883*df));

  /*maximal demand of N given biomasss growth (actually too much)*/
  /*ndemand_crop_max = par->cn_ratio.leaf*
    (pft->bm_inc.carbon*froot*ratio_leaf_root+ */ /* root growth */
    /*crop->lai/pft->par->sla+*/ /* leaf growth */
    /*max(0,(pft->bm_inc.carbon*(1-froot)-crop->lai/pft->par->sla))*ratio_so_leaf);*/ /* growth of the rest of the plant */
  /*try to satisfy demand by extra uptake */
  /*sNO3=0;
  foreachsoillayer(l) sNO3+=pft->stand->soil.NO3[l];
  foreachsoillayer(l) {
    if(ndemand_crop_max<pft->bm_inc.nitrogen&& sNO3>0) {
      ndeficit=max(sNO3,ndemand_crop_max-pft->bm_inc.nitrogen);
      pft->bm_inc.nitrogen+=ndeficit*pft->stand->soil.NO3[l]/sNO3;
      pft->stand->soil.NO3[l]-=ndeficit*pft->stand->soil.NO3[l]/sNO3;
    }
  }
  crop->ndemandsum+=ndemand_crop_max;
  }*/

  crop->ind.root.carbon=froot*pft->bm_inc.carbon;

  /* leaf growth */
  /* Calculation of leaf biomass (g C/m2), prescribed LAI development, constrained by NPP */
  if(!crop->senescence)
  {
    if(pft->bm_inc.carbon-crop->ind.root.carbon>=crop->lai/pft->par->sla)
    {
      crop->ind.leaf.carbon=crop->lai/pft->par->sla;
      crop->lai_nppdeficit=0;
    }
    else
    {
      crop->ind.leaf.carbon=pft->bm_inc.carbon-crop->ind.root.carbon;
      crop->lai_nppdeficit=crop->lai-crop->ind.leaf.carbon*pft->par->sla;
      /* today's lai_nppdeficit is substracted from tomorrow's LAI in lai_crop() in fpar_crop() and actual_lai_crop()
        which are the routines, where LAI has an effect on the simulation */
      /*crop->lai-=crop->lai_nppdeficit;*/
    }
  }
  else
  {
    if(crop->ind.leaf.carbon+crop->ind.root.carbon+crop->ind.so.carbon>pft->bm_inc.carbon)
      crop->ind.leaf.carbon=pft->bm_inc.carbon-crop->ind.root.carbon-crop->ind.so.carbon;
    if(crop->ind.leaf.carbon<0)
      crop->ind.leaf.carbon=0;
  }

  /* storage organs growth */
  /* scale HIopt according to LAImax */
  fhiopt=100*crop->fphu/(100*crop->fphu+exp(11.1-10.0*crop->fphu));

//
//  if(crop->fphu>0)
//  { /* avoid memory leakage when called from new_crop.c */
//    if(pft->par->id==MAIZE)
//      hiopt=min(1,par->hiopt*(0.8+0.2/4*(pft->stand->cell->ml.manage.laimax[pft->par->id]-1)));
//    else
//      hiopt=par->hiopt*(0.8+0.2/6*(pft->stand->cell->ml.manage.laimax[pft->par->id]-1));
//    hi=(hiopt>1.0) ? fhiopt*(hiopt-1.0)+1.0 : fhiopt*hiopt;
//  }
//  else
  hi=(par->hiopt>1.0) ? fhiopt*(par->hiopt-1.0)+1.0 : fhiopt*par->hiopt;
  himind=(par->himin>1.0) ? fhiopt*(par->himin-1.0)+1.0 : fhiopt*par->himin;
  
  if(wdf>=0)
    hi=(hi-himind)*wdf/(wdf+exp(6.13-0.0883*wdf))+himind;

  if((crop->ind.leaf.carbon+crop->ind.root.carbon)<pft->bm_inc.carbon)
  {
    crop->ind.so.carbon=(par->hiopt>1.0) ? (1.0-1.0/hi)*(1.0-froot)*pft->bm_inc.carbon : hi*(1.0-froot)*pft->bm_inc.carbon;
    if(crop->ind.leaf.carbon+crop->ind.root.carbon+crop->ind.so.carbon>pft->bm_inc.carbon)
      crop->ind.so.carbon=pft->bm_inc.carbon-crop->ind.leaf.carbon-crop->ind.root.carbon;
  }
  else
    crop->ind.so.carbon=0;
//  crop->ind.so.nitrogen=crop->ind.so.carbon*par->cn_ratio.leaf; /* crop->ind.so.nitrogen=crop->ind.so.carbon*par->cn_ratio.leaf*ratio_so_leaf; */

  /* additional pool */
  crop->ind.pool.carbon=pft->bm_inc.carbon-crop->ind.root.carbon-crop->ind.leaf.carbon-crop->ind.so.carbon;
  if(crop->senescence && crop->ind.pool.carbon<0)
  { /* during senescence pool can become negative */
    if(crop->ind.so.carbon+crop->ind.pool.carbon<0)
    {
      crop->ind.pool.carbon+=crop->ind.so.carbon;
      crop->ind.so.carbon=0;
      if(crop->ind.root.carbon+crop->ind.pool.carbon<0)
      {
        crop->ind.pool.carbon+=crop->ind.root.carbon;
        crop->ind.root.carbon=0;
        /* remainder negative pool must be compensated by leaves,
         * negative bm_inc is already checked for in npp_crop() */
        crop->ind.leaf.carbon+=crop->ind.pool.carbon;
        crop->ind.pool.carbon=0;
      }
      else
      {
        crop->ind.root.carbon+=crop->ind.pool.carbon;
        crop->ind.pool.carbon=0;
      }
    }
    else
    {
      crop->ind.so.carbon+=crop->ind.pool.carbon;
      crop->ind.pool.carbon=0;
    }
  }
#if 1
  if(config->with_nitrogen)
  {
  // Nitrogen allocation
  if(crop->ind.leaf.carbon>epsilon && pft->bm_inc.nitrogen>0)
  {
    crop->ind.leaf.nitrogen=crop->ind.root.nitrogen=crop->ind.so.nitrogen=crop->ind.pool.nitrogen=0;
    //crop->ind.leaf.nitrogen=pft->nleaf/pft->nind;
    solve(&a,&b,&c,&d,
          crop->ind.leaf.nitrogen,crop->ind.root.nitrogen,crop->ind.so.nitrogen,crop->ind.pool.nitrogen,
          crop->ind.leaf.carbon,crop->ind.root.carbon,crop->ind.so.carbon,crop->ind.pool.carbon,
          par->ratio.root,par->ratio.so,par->ratio.pool,pft->bm_inc.nitrogen);
//    nitrogen_allocation(&a,&b,&c,crop->ind.leaf,crop->ind.root,crop->ind.so,par->ratio.root,par->ratio.so,pft->bm_inc.nitrogen);
    if(FALSE && (crop->ind.leaf.nitrogen+a*pft->bm_inc.nitrogen)/crop->ind.leaf.carbon>pft->par->ncleaf.high)
    {
      nitrogen_before=crop->ind.leaf.nitrogen;
      crop->ind.leaf.nitrogen=pft->par->ncleaf.high*crop->ind.leaf.carbon;
      pft->bm_inc.nitrogen-=crop->ind.leaf.nitrogen-nitrogen_before;
      if(pft->bm_inc.nitrogen>=crop->ind.root.carbon*pft->par->ncleaf.high/par->ratio.root-crop->ind.root.nitrogen)
      {
        nitrogen_before=crop->ind.root.nitrogen;
        crop->ind.root.nitrogen=pft->par->ncleaf.high*crop->ind.root.carbon/par->ratio.root;
        pft->bm_inc.nitrogen-=crop->ind.root.nitrogen-nitrogen_before;
      }
      else
      {
        crop->ind.root.nitrogen+= pft->bm_inc.nitrogen;
        pft->bm_inc.nitrogen=0;
      }
      if(pft->bm_inc.nitrogen>=crop->ind.so.carbon*pft->par->ncleaf.high/par->ratio.so-crop->ind.so.nitrogen)
      {
        nitrogen_before=crop->ind.so.nitrogen;
        crop->ind.so.nitrogen=pft->par->ncleaf.high*crop->ind.so.carbon/par->ratio.so;
        pft->bm_inc.nitrogen-=crop->ind.so.nitrogen-nitrogen_before;
      }
      else
      {
        crop->ind.so.nitrogen+= pft->bm_inc.nitrogen;
        pft->bm_inc.nitrogen=0;
      }
      if(pft->bm_inc.nitrogen>=crop->ind.pool.carbon*pft->par->ncleaf.high/par->ratio.pool-crop->ind.pool.nitrogen)
      {
        nitrogen_before=crop->ind.pool.nitrogen;
        crop->ind.pool.nitrogen=pft->par->ncleaf.high*crop->ind.so.carbon/par->ratio.pool;
        pft->bm_inc.nitrogen-=crop->ind.pool.nitrogen-nitrogen_before;
      }
      else
      {
        crop->ind.pool.nitrogen+= pft->bm_inc.nitrogen;
        pft->bm_inc.nitrogen=0;
      }
    }
    else
    {
       crop->ind.leaf.nitrogen+=a*pft->bm_inc.nitrogen;
       crop->ind.root.nitrogen+=b*pft->bm_inc.nitrogen;
       crop->ind.so.nitrogen+=c*pft->bm_inc.nitrogen;
       crop->ind.pool.nitrogen+=d*pft->bm_inc.nitrogen;
#ifdef DEBUG_N
       printf("%g %g %g %g\n",crop->ind.leaf.carbon/crop->ind.leaf.nitrogen,
              crop->ind.root.carbon/crop->ind.root.nitrogen,
              crop->ind.so.carbon/crop->ind.so.nitrogen,
              crop->ind.pool.carbon/crop->ind.pool.nitrogen);
#endif
       //pft->bm_inc.nitrogen=0;
    }
  }
  pft->nleaf=crop->ind.leaf.nitrogen;
  }
#else
  if(crop->ind.leaf.carbon>epsilon)
  {
    /* leaf nitrogen */
    crop->ind.leaf.nitrogen=pft->nleaf/pft->nind;

    if(crop->ind.leaf.nitrogen/crop->ind.leaf.carbon>pft->par->ncleaf.high)
    {
      crop->ind.leaf.nitrogen=crop->ind.leaf.carbon*pft->par->ncleaf.high; /*during senescence ncleaf should be reduced to liberate N from the brown leaves to allow for reallocation to storage organs, e.g. with LAI, SLA and leaf carbon */
      pft->nleaf=crop->ind.leaf.nitrogen*pft->nind;
    }

    /*reduce nc ratio if higher than prescribed in pft.par*/
    if(crop->ind.leaf.carbon>epsilon && pft->bm_inc.nitrogen >0)
     nc_ratio=min(pft->par->ncleaf.high,crop->ind.leaf.nitrogen/crop->ind.leaf.carbon);
    else
     nc_ratio=pft->par->ncleaf.high;

    /* root nitrogen and reduce bm_inc.nitrogen accordingly */
    if(crop->ind.root.carbon*nc_ratio/par->ratio.so < pft->bm_inc.nitrogen-crop->ind.leaf.nitrogen)
      crop->ind.root.nitrogen=crop->ind.root.carbon*nc_ratio/par->ratio.root;
    else
      crop->ind.root.nitrogen=max(0,pft->bm_inc.nitrogen-crop->ind.leaf.nitrogen);

    /* storage organ nitrogen and reduce bm_inc.nitrogen accordingly */
    if(crop->ind.so.carbon*nc_ratio/par->ratio.so < pft->bm_inc.nitrogen-crop->ind.leaf.nitrogen-crop->ind.root.nitrogen)
      crop->ind.so.nitrogen  =crop->ind.so.carbon*nc_ratio/par->ratio.so;
    else
      crop->ind.so.nitrogen = max(0,pft->bm_inc.nitrogen-crop->ind.leaf.nitrogen-crop->ind.root.nitrogen);

    /* pool nitrogen and reduce bm_inc.nitrogen accordingly */
    /*if(crop->ind.leaf.carbon*nc_ratio/par->ratio.pool < pft->bm_inc.nitrogen-crop->ind.leaf.nitrogen-crop->ind.root.nitrogen-crop->ind.so.nitrogen)
      crop->ind.pool.nitrogen=crop->ind.leaf.carbon*nc_ratio/par->ratio.pool;
    else
      crop->ind.pool.nitrogen=max(0,pft->bm_inc.nitrogen-crop->ind.leaf.nitrogen-crop->ind.root.nitrogen-crop->ind.so.nitrogen);*/

    crop->ind.pool.nitrogen=pft->bm_inc.nitrogen-crop->ind.leaf.nitrogen-crop->ind.root.nitrogen-crop->ind.so.nitrogen;
  }

  /* to ensure fixed CN ratios, all pools are reduced if more N had been allocated
     than is available */
  /*if((crop->ind.leaf.nitrogen+crop->ind.root.nitrogen+crop->ind.so.nitrogen)>pft->bm_inc.nitrogen)
  {
    nf=pft->bm_inc.nitrogen/(crop->ind.leaf.nitrogen+crop->ind.root.nitrogen+crop->ind.so.nitrogen);
    printf("1 daily_ag ag %g %g %g %g\n",crop->ind.leaf.nitrogen,crop->ind.root.nitrogen,crop->ind.leaf.carbon,crop->ind.root.carbon);
    crop->ind.leaf.carbon*=nf;
    crop->ind.root.carbon*=nf;
    crop->ind.so.carbon*=nf;
    crop->ind.leaf.nitrogen*=nf;
    crop->ind.root.nitrogen*=nf;
    crop->ind.so.nitrogen*=nf;
  }*/
  //printf("8 alloc %g %g %g %g\n",crop->ind_n.leaf,crop->ind_n.root,crop->ind.leaf,crop->ind.root);
  /*printf("alloc 2 bm_inc %g carbon %g %g %g %g nitrogen %g %g %g %g\n",
    pft->bm_inc,crop->ind.leaf,crop->ind.pool,crop->ind.root,crop->ind.so,
    crop->ind_n.leaf,crop->ind_n.pool,crop->ind_n.root,crop->ind_n.so);*/
#endif
} /* of 'allocation_daily_crop' */

/*
- called in npp_crop()
- calculation of the vegetation growth:
  -> calculation of carbon biomass (total plant growth)
  -> calculation of carbon mass root (root growth)
  -> calculation of carbon mass leaf (leaf growth)
  -> calculation of carbon mass so (storage organs growth)
  -> calculation of carbon mass pool (additional pool stem + reserve)
*/
