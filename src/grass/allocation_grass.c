/**************************************************************************************/
/**                                                                                \n**/
/**       a  l  l  o  c  a  t  i  o  n  _  g  r  a  s  s  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates yearly biomass increment to leafs and roots             \n**/
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

Bool allocation_grass(Litter *litter,   /**< litter pool */
                      Pft *pft,         /**< pointer to PFT */
                      Real *fpc_inc,    /**< fpc increment */
                      const Config *config /**< LPJmL configuration */
                     )                  /** \return TRUE on death */
{
  Stocks bm_inc_ind={0,0};
  Real lmtorm,vscal,wscal;
  Grassphys inc_ind;
  Pftgrass *grass;
  Output *output;
  const Pftgrasspar *grasspar;
  Grassphys lastday;
  Real a;
  int growing_days;
  grasspar=pft->par->data;
  grass=pft->data;
#ifdef CHECK_BALANCE
  Real start,end;
  Stocks stocks;
  stocks=litterstocks(litter);
  start = vegc_sum(pft)+pft->bm_inc.carbon-(grass->turn_litt.leaf.carbon+grass->turn_litt.root.carbon)+stocks.carbon;
#endif
  output=&pft->stand->cell->output;
  bm_inc_ind.carbon=pft->bm_inc.carbon/pft->nind;
  bm_inc_ind.nitrogen=pft->bm_inc.nitrogen/pft->nind;
  if(pft->stand->type->landusetype!=GRASSLAND && pft->stand->type->landusetype!=OTHERS && pft->stand->type->landusetype!=BIOMASS_GRASS && pft->stand->type->landusetype!=SETASIDE_RF && pft->stand->type->landusetype!=SETASIDE_IR)
    growing_days=NDAYYEAR;
  else
  {
    growing_days=grass->growing_days;
    grass->growing_days=1;
  }

  if (growing_days>0)
  {
    vscal=(config->with_nitrogen) ? min(1,pft->vscal/growing_days) : 1;
    wscal=pft->wscal_mean/growing_days;
    lmtorm=getpftpar(pft,lmro_ratio)*(getpftpar(pft,lmro_offset)+(1-getpftpar(pft,lmro_offset))*min(vscal,wscal));
  }
  else
  {
    vscal=(config->with_nitrogen) ? min(1,pft->vscal/NDAYYEAR) : 1;
    wscal=pft->wscal_mean/NDAYYEAR;
    lmtorm=getpftpar(pft,lmro_ratio)*(getpftpar(pft,lmro_offset)+(1-getpftpar(pft,lmro_offset))*min(vscal,wscal));
  }

  pft->wscal_mean=pft->vscal=0;
  /* daily allocation for new grasslands */
  if(pft->stand->type->landusetype==GRASSLAND || pft->stand->type->landusetype==OTHERS || pft->stand->type->landusetype==BIOMASS_GRASS ||
      pft->stand->type->landusetype==SETASIDE_IR || pft->stand->type->landusetype==SETASIDE_RF || pft->stand->type->landusetype==SETASIDE_WETLAND)
  {
    /* otherwise there wouldn't be any growth */
    lmtorm=max(lmtorm,0.25);

    if(bm_inc_ind.carbon>0.0)
    { /* try to achieve today's lmtorm */

      inc_ind.leaf.carbon=(bm_inc_ind.carbon+grass->ind.root.carbon-grass->ind.leaf.carbon/lmtorm)/(1.0+1.0/lmtorm);
      /* no reallocation from leaves to roots */
      inc_ind.leaf.carbon=max(inc_ind.leaf.carbon,0.0);
      /* no reallocation from roots to leaves */
      inc_ind.leaf.carbon=min(inc_ind.leaf.carbon,bm_inc_ind.carbon);

      inc_ind.root.carbon=bm_inc_ind.carbon-inc_ind.leaf.carbon;
    }
    else
    {
      /* negative bm_inc reduces leaves and roots proportionally */
      inc_ind.leaf.carbon=bm_inc_ind.carbon*grass->ind.leaf.carbon/(grass->ind.root.carbon+grass->ind.leaf.carbon);
      inc_ind.root.carbon=bm_inc_ind.carbon*grass->ind.root.carbon/(grass->ind.root.carbon+grass->ind.leaf.carbon);
    }
    output->bm_inc=pft->bm_inc.carbon;
    pft->bm_inc.carbon-=(inc_ind.leaf.carbon+inc_ind.root.carbon)*pft->nind;
  }
  else /* this is for natural vegetation with yearly full reallocation */
  {
    if (lmtorm<1.0e-10)
    {
      inc_ind.leaf.carbon=0.0;
      inc_ind.root.carbon=bm_inc_ind.carbon;
    }
    else
    {
      inc_ind.leaf.carbon=(bm_inc_ind.carbon+grass->ind.root.carbon-grass->ind.leaf.carbon/lmtorm)/(1.0+1.0/lmtorm);
      if(inc_ind.leaf.carbon<0.0)  /*NEGATIVE ALLOCATION TO LEAF MASS */
      {
        inc_ind.root.carbon=bm_inc_ind.carbon;
        inc_ind.leaf.carbon=(grass->ind.root.carbon+inc_ind.root.carbon)*lmtorm-grass->ind.leaf.carbon;
        /* put negative carbon only into litter if litter is large enough otherwise into estab flux */
        if(litter->item[pft->litter].agtop.leaf.carbon>=inc_ind.leaf.carbon*pft->nind)
        {
          litter->item[pft->litter].agtop.leaf.carbon-=inc_ind.leaf.carbon*pft->nind;
          getoutput(output,LITFALLC,config)-=inc_ind.leaf.carbon*pft->nind*pft->stand->frac;
        }
        else
          getoutput(output,FLUX_ESTABC,config)+=inc_ind.leaf.carbon*pft->nind*pft->stand->frac;
        update_fbd_grass(litter,pft->par->fuelbulkdensity,-inc_ind.leaf.carbon*pft->nind);
      }
      else
      {
        if(bm_inc_ind.carbon>0 && inc_ind.leaf.carbon>bm_inc_ind.carbon)
          inc_ind.leaf.carbon=bm_inc_ind.carbon;
        inc_ind.root.carbon=bm_inc_ind.carbon-inc_ind.leaf.carbon;
      }
    }
    output->bm_inc=pft->bm_inc.carbon;
    pft->bm_inc.carbon=0;
  }
  if(bm_inc_ind.carbon>0 && inc_ind.leaf.carbon>0 && inc_ind.root.carbon>0)
  {
    grass->falloc.leaf=inc_ind.leaf.carbon/bm_inc_ind.carbon;
    grass->falloc.root=inc_ind.root.carbon/bm_inc_ind.carbon;
  }

  grass->ind.leaf.carbon+=inc_ind.leaf.carbon;
  grass->ind.root.carbon+=inc_ind.root.carbon;
  if(config->with_nitrogen)
  {
    lastday.leaf.nitrogen = grass->ind.leaf.nitrogen;
    lastday.root.nitrogen = grass->ind.root.nitrogen;
#ifdef DEBUG_N
    printf("NC_leaf, NC_root: %g %g\n",grass->ind.leaf.nitrogen/grass->ind.leaf.carbon,
           grass->ind.root.nitrogen/grass->ind.root.carbon);
#endif
    if (grass->ind.leaf.carbon >epsilon && bm_inc_ind.nitrogen>epsilon)
    {
      a = (grasspar->ratio*grass->ind.root.nitrogen*grass->ind.leaf.carbon - grass->ind.leaf.nitrogen*grass->ind.root.carbon + grass->ind.leaf.carbon*bm_inc_ind.nitrogen*grasspar->ratio) /
        (bm_inc_ind.nitrogen*grass->ind.root.carbon + bm_inc_ind.nitrogen*grasspar->ratio*grass->ind.leaf.carbon);
      if (a<0)
        a = 0;
      if (a>1)
        a = 1;
      if((grass->ind.leaf.nitrogen+a*bm_inc_ind.nitrogen)/grass->ind.leaf.carbon > pft->par->ncleaf.high)
      {
        grass->ind.leaf.nitrogen = grass->ind.leaf.carbon*pft->par->ncleaf.high;
        pft->bm_inc.nitrogen -= (grass->ind.leaf.nitrogen-lastday.leaf.nitrogen)*pft->nind;
        if (pft->bm_inc.nitrogen >= grass->ind.root.carbon*pft->par->ncleaf.high / grasspar->ratio - grass->ind.root.nitrogen)
        {
          grass->ind.root.nitrogen = grass->ind.root.carbon*pft->par->ncleaf.high / grasspar->ratio;
          pft->bm_inc.nitrogen -= (grass->ind.root.nitrogen - lastday.root.nitrogen)*pft->nind;
        }
        else
        {
          grass->ind.root.nitrogen += pft->bm_inc.nitrogen/pft->nind;
          pft->bm_inc.nitrogen = 0;
        }
      }
      else
      {
        grass->ind.leaf.nitrogen += a*bm_inc_ind.nitrogen;
        grass->ind.root.nitrogen += (1 - a)*bm_inc_ind.nitrogen;
        pft->bm_inc.nitrogen = 0;
        /* testing if there is too much carbon for allowed NC ratios */
        if (grass->ind.leaf.nitrogen / grass->ind.leaf.carbon<pft->par->ncleaf.low)
        {
          lastday.leaf.carbon = grass->ind.leaf.carbon;
          grass->ind.leaf.carbon = grass->ind.leaf.nitrogen / pft->par->ncleaf.low;
          litter->item[pft->litter].agtop.leaf.carbon += (lastday.leaf.carbon - grass->ind.leaf.carbon)*pft->nind;
          getoutput(output,LITFALLC,config) += (lastday.leaf.carbon - grass->ind.leaf.carbon)*pft->nind*pft->stand->frac;
          update_fbd_grass(litter, pft->par->fuelbulkdensity,
            (lastday.leaf.carbon - grass->ind.leaf.carbon)*pft->nind);
        }
        if (grass->ind.root.nitrogen/grass->ind.root.carbon<pft->par->ncleaf.low/grasspar->ratio)
        {
          lastday.root.carbon = grass->ind.root.carbon;
          grass->ind.root.carbon = grass->ind.root.nitrogen / pft->par->ncleaf.low*grasspar->ratio;
          litter->item[pft->litter].bg.carbon += (lastday.root.carbon - grass->ind.root.carbon)*pft->nind;
          getoutput(output,LITFALLC,config) += (lastday.root.carbon - grass->ind.root.carbon)*pft->nind*pft->stand->frac;
        }
      }
    }
    else
    {
      /* testing if there is too much nitrogen for allowed NC ratios */
      if (grass->ind.leaf.carbon>0 && grass->ind.leaf.nitrogen/grass->ind.leaf.carbon>pft->par->ncleaf.high)
      {
        grass->ind.leaf.nitrogen = grass->ind.leaf.carbon*pft->par->ncleaf.high;
        pft->bm_inc.nitrogen += (lastday.leaf.nitrogen-grass->ind.leaf.nitrogen)*pft->nind;
      }
      if(grass->ind.root.carbon>0 && grass->ind.root.nitrogen/grass->ind.root.carbon>pft->par->ncleaf.high/grasspar->ratio)
      {
        grass->ind.root.nitrogen = grass->ind.root.carbon*pft->par->ncleaf.high/grasspar->ratio;
        pft->bm_inc.nitrogen += (lastday.root.nitrogen-grass->ind.root.nitrogen)*pft->nind;
      }
      /* testing if there is too much carbon for allowed NC ratios */
      if(grass->ind.leaf.carbon>0 && grass->ind.leaf.nitrogen / grass->ind.leaf.carbon<pft->par->ncleaf.low)
      {
        lastday.leaf.carbon = grass->ind.leaf.carbon;
        grass->ind.leaf.carbon = grass->ind.leaf.nitrogen / pft->par->ncleaf.low;
        litter->item[pft->litter].agtop.leaf.carbon += (lastday.leaf.carbon - grass->ind.leaf.carbon)*pft->nind;
        getoutput(output,LITFALLC,config) += (lastday.leaf.carbon - grass->ind.leaf.carbon)*pft->nind*pft->stand->frac;
        update_fbd_grass(litter, pft->par->fuelbulkdensity,
          (lastday.leaf.carbon - grass->ind.leaf.carbon)*pft->nind);
      }
      if(grass->ind.root.carbon>0 && grass->ind.root.nitrogen/grass->ind.root.carbon<pft->par->ncleaf.low/grasspar->ratio)
      {
        lastday.root.carbon = grass->ind.root.carbon;
        grass->ind.root.carbon = grass->ind.root.nitrogen / pft->par->ncleaf.low*grasspar->ratio;
        litter->item[pft->litter].bg.carbon += (lastday.root.carbon - grass->ind.root.carbon)*pft->nind;
        getoutput(output,LITFALLC,config) += (lastday.root.carbon - grass->ind.root.carbon)*pft->nind*pft->stand->frac;
      }
    }
    pft->nleaf = grass->ind.leaf.nitrogen*pft->nind;
  } /* of config->with_nitrogen */

#ifdef CHECK_BALANCE
  stocks=litterstocks(litter);
  end = vegc_sum(pft)-(grass->turn_litt.leaf.carbon+grass->turn_litt.root.carbon)+stocks.carbon;

  if(fabs(end-start)>0.01)
    fprintf(stderr, "C_ERROR allocation_grass: %g start : %g end : %g  bm_inc.carbon: %g  PFT:%s nind: %g leaf_turn_litt: %g root_turn_litt: %g  root_turn: %g  leaf_turn: %g \n",
        end-start, start,end,pft->bm_inc.carbon,pft->par->name,pft->nind,grass->turn_litt.root.carbon,grass->turn_litt.leaf.carbon,grass->turn.root.carbon,grass->turn.leaf.carbon);
#endif

  *fpc_inc=fpc_grass(pft);
#ifdef DEBUG
  printf("allocation grass leaf %g root %g fpcinc %g\n",grass->ind.leaf.carbon,grass->ind.root.carbon,*fpc_inc);
#endif
  return (grass->ind.leaf.carbon<0 || grass->ind.root.carbon<0.0 || pft->fpc<=1e-20);
} /* of 'allocation_grass' */
