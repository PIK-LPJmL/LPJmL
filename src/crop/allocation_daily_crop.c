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
  Real a,b,c,d;
  Real fhiopt,himind,hi;
  Real ndf=100; /* nitrogen deficit factor in percent, computed as wdf from accumulated n_demand and n_uptake */
  Real df;
  crop=pft->data;
  par=pft->par->data;
  /* vegetation growth */
  pft->bm_inc.carbon+=npp;

  /* root growth */
  if(config->with_nitrogen)
  {
    crop->vscal_sum+=pft->vscal;
    ndf=(crop->growingdays>0.0) ? crop->vscal_sum/crop->growingdays*100 : 100.0;
  }
  /* this work also without with_nitrogen, as ndf is initialized to 100 */
  df=min(wdf,ndf);
  froot=FROOTMAX-(FROOTMIN*crop->fphu)*df/(df+exp(6.13-0.0883*df));

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
  if(config->with_nitrogen)
  {
    /* Nitrogen allocation */
    if(crop->ind.leaf.carbon>epsilon && pft->bm_inc.nitrogen>0)
    {
      /* all of bm_inc.nitrogen is allocated, so setting current allocation to zero first */
      crop->ind.leaf.nitrogen=crop->ind.root.nitrogen=crop->ind.so.nitrogen=crop->ind.pool.nitrogen=0;
      solve(&a,&b,&c,&d,
            crop->ind.leaf.nitrogen,crop->ind.root.nitrogen,crop->ind.so.nitrogen,crop->ind.pool.nitrogen,
            crop->ind.leaf.carbon,crop->ind.root.carbon,crop->ind.so.carbon,crop->ind.pool.carbon,
            par->ratio.root,par->ratio.so,par->ratio.pool,pft->bm_inc.nitrogen);
    
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
    }
  }
  pft->nleaf=crop->ind.leaf.nitrogen;
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
