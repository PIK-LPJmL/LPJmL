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
#include "agriculture.h"

#define FROOTMAX 0.4
#define FROOTMIN 0.3 /* learning from AgMIP wheat and maize pilots */

void allocation_daily_crop(Pft *pft, /**< PFT variables */
                           Real npp, /**< net primary production (gC/m2) */ 
                           Real wdf,  /**< water deficit fraction */
                           Daily_outputs* output /**< daily output data */
                          )
{
  Pftcrop *crop;
  Pftcroppar *par;
  Real froot;
  Real fhiopt,himind,hi,hiopt=0;
  Irrigation *data;
  data=pft->stand->data;
  crop=pft->data;
  par=pft->par->data;

  /* vegetation growth */
  pft->bm_inc+=npp;

  /* root growth */
  /*
  froot=FROOTMAX-FROOTMIN*crop->fphu;
  */
  froot=FROOTMAX-(FROOTMIN*crop->fphu)*wdf/(wdf+exp(6.13-0.0883*wdf));
  crop->ind.root=froot*pft->bm_inc;
  
  /* leaf growth */  
  /* Calculation of leaf biomass (g C/m2), prescribed LAI development, constrained by NPP */  
  if(!crop->senescence)
  {
    if(pft->bm_inc-crop->ind.root>=crop->lai/pft->par->sla)
    {
      crop->ind.leaf=crop->lai/pft->par->sla;
      crop->lai_nppdeficit=0;
    }
    else
    {
      crop->ind.leaf=pft->bm_inc-crop->ind.root;
      crop->lai_nppdeficit=crop->lai-crop->ind.leaf*pft->par->sla;
      /* today's lai_nppdeficit is substracted from tomorrow's LAI in lai_crop() in fpar_crop() and actual_lai_crop()
      * which are the routines, where LAI has an effect on the simulation */
      /*crop->lai-=crop->lai_nppdeficit;*/
    }
  }
  else
  {
    if(crop->ind.leaf+crop->ind.root+crop->ind.so>pft->bm_inc)
      crop->ind.leaf=pft->bm_inc-crop->ind.root-crop->ind.so;
    if(crop->ind.leaf<0)
      crop->ind.leaf=0;
  }

  /* storage organs growth */
  /* scale HIopt according to LAImax */
  fhiopt=100*crop->fphu/(100*crop->fphu+exp(11.1-10.0*crop->fphu));
  if(crop->fphu>0)
  { /* avoid memory leakage when called from new_crop.c */
    if(pft->par->id==MAIZE)
      hiopt=min(1,par->hiopt*(0.8+0.2/4*(pft->stand->cell->ml.manage.laimax[pft->par->id]-1)));
    else
      hiopt=par->hiopt*(0.8+0.2/6*(pft->stand->cell->ml.manage.laimax[pft->par->id]-1));
    hi=(hiopt>1.0) ? fhiopt*(hiopt-1.0)+1.0 : fhiopt*hiopt;
  }
  else
  hi=(par->hiopt>1.0) ? fhiopt*(par->hiopt-1.0)+1.0 : fhiopt*par->hiopt;
  himind=(par->himin>1.0) ? fhiopt*(par->himin-1.0)+1.0 : fhiopt*par->himin;
  /*
  if(wdf>=0)
    hi=(hi-himind)*wdf/(wdf+exp(6.13-0.0883*wdf))+himind;
  */  
  if((crop->ind.leaf+crop->ind.root)<pft->bm_inc)
  {
    crop->ind.so=(par->hiopt>1.0) ? (1.0-1.0/hi)*(1.0-froot)*pft->bm_inc : hi*(1.0-froot)*pft->bm_inc;
    if(crop->ind.leaf+crop->ind.root+crop->ind.so>pft->bm_inc)
      crop->ind.so=pft->bm_inc-crop->ind.leaf-crop->ind.root;
  }
  else
    crop->ind.so=0;
  
  /* additional pool */
  crop->ind.pool=pft->bm_inc-crop->ind.root-crop->ind.leaf-crop->ind.so;
  if(crop->senescence && crop->ind.pool<0)
  { /* during senescence pool can become negative */
    if(crop->ind.so+crop->ind.pool<0)
    {
      crop->ind.pool+=crop->ind.so;
      crop->ind.so=0;
      if(crop->ind.root+crop->ind.pool<0)
      {
        crop->ind.pool+=crop->ind.root;
        crop->ind.root=0;
        /* remainder negative pool must be compensated by leaves,
         * negative bm_inc is already checked for in npp_crop() */
        crop->ind.leaf+=crop->ind.pool;
        crop->ind.pool=0;
      }
      else
      {
        crop->ind.root+=crop->ind.pool;
        crop->ind.pool=0;
      }
    }
    else
    {
      crop->ind.so+=crop->ind.pool;
      crop->ind.pool=0;
    }
  }
  if(output!=NULL && pft->par->id==output->cft &&
     data->irrigation==output->irrigation)
  {
    output->froot=froot;
    output->hi=hi;
    output->himind=himind;
    output->fhiopt=fhiopt;
    output->lainppdeficit=crop->lai_nppdeficit;
  }
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
