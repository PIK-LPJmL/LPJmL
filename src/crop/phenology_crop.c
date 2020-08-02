/**************************************************************************************/
/**                                                                                \n**/
/**            p  h  e  n  o  l  o  g  y  _  c  r  o  p  .  c                      \n**/
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

#define VD_SLOPE 7 /* slope parameter for reduced vernalization effectivity +/- 7 deg outside min/max range*/
#ifdef CROPSHEATFROST
#define TLOW 30.0
#define THIGH 40.0 /* low and high temperature for reduction of harvest index */
#endif

Bool phenology_crop(Pft *pft,      /**< pointer to PFT variables */
                    Real temp,     /**< temperature (deg C) */
                    Real tmax,     /**< daily maximum temperature (deg C) */
                    Real daylength, /**< length of day (h) */
                    int npft,
                    const Config *config /**< LPJ configuration */
                    )               /** \return harvesting crop (TRUE/FALSE) */
{
  Pftcrop *crop;
  const Pftcroppar *par;
  Real hu,k,c;
  Real lai0,lai00;
  Real lai_inc=0;
  Real laimax;
  Bool harvesting;
  int hlimit;
  Real vd_inc,vd_b,vrf,prf;
#ifdef CROPSHEATFROST
  Real as;
#endif
  crop=pft->data;
  par=pft->par->data;
  harvesting=FALSE;
  crop->growingdays++;

  if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
    hlimit=par->hlimit;
  else
    hlimit=(crop->wtype) ? par->hlimit+90 : par->hlimit; /* add 90 days for winter crops for internally computed seasons */

  crop->senescence0=crop->senescence;

  laimax=pft->stand->cell->ml.manage.laimax[pft->par->id];

#ifdef CROPSHEATFROST
  /* accelerated senescence factor should be between 1 and 2 and applied to mean temperature, following Maiorano et al. 2017 */
  as = 1.0;
  if (tmax > TLOW && tmax < THIGH)
    as = 1/(THIGH-TLOW)*(tmax-TLOW)+1;
  else if(tmax > THIGH)
    as = 2;
#endif

  if(crop->husum<crop->phu)
  {
#ifdef CROPSHEATFROST
    if(!crop->senescence)
      hu=max(0,temp*as-crop->basetemp);
    else 
      hu=max(0,temp-crop->basetemp);
#else
    hu=max(0,temp-crop->basetemp);
#endif

    if (crop->wtype) /* if winter crops with vernalization requirements */
    {
      /* Calculation of daily vernalization increment */
      if (crop->vdsum<pft->stand->cell->climbuf.V_req[pft->par->id-npft])
      {
        if (temp>=par->tv_eff.low && temp<par->tv_opt.low)                          /* temp within effective, but below optimal conditions */
          vd_inc=(temp-par->tv_eff.low )/(par->tv_opt.low-par->tv_eff.low );
        else if (temp<=par->tv_eff.high && temp>=par->tv_opt.high)                  /* temp within effective, but above optimal conditions */
          vd_inc=(par->tv_eff.high-temp)/(par->tv_eff.high-par->tv_opt.high);
        else if (temp>=par->tv_opt.low && temp<par->tv_opt.high)                    /* temp within optimal conditions */
          vd_inc=1.0;
      }
      else
        vd_inc=0.0;

      crop->vdsum+=max(0,vd_inc);

      /* Calculation of vernalization reduction factor */
      // vd_b=pft->stand->cell->climbuf.V_req[pft->par->id-npft]/5; /* base requirements, 20% of total vernalization requirements */
      vd_b=pft->stand->cell->climbuf.V_req[pft->par->id-npft]*0.01; /* macmit_intensification: modified base requirements */

      if (crop->vdsum<vd_b) /* no phenological development before 20% of vern. requirements are accumulated */
        vrf=0.0;            /* ToDo: hu sums are not accumulated until V_b is reached; causes autumn heat sums to be neglected. This is not a problem if PHUs are calculated the same way */
      else if (crop->vdsum>=vd_b && crop->vdsum<pft->stand->cell->climbuf.V_req[pft->par->id-npft]) /* the previous version stopped the vernalization penalty at 0.2 fphu "&& (crop->fphu<=0.2)" */
        vrf=max(0,min(1,(crop->vdsum-vd_b)/(pft->stand->cell->climbuf.V_req[pft->par->id-npft]-vd_b)));
      else
        vrf=1.0;
    }
    else
      vrf = 1.0;

    /* Response to photoperiodism */
    if (crop->fphu <= par->fphusen)
      prf=(1-par->psens)*min(1,max(0,(daylength-par->pb)/(par->ps-par->pb)))+par->psens;
    else
      prf=1.0;

    /* Calculation of temperature sum (deg Cd) */
    crop->husum += hu * vrf * prf;

    /* fraction of growing season */
    crop->fphu=min(1.0,crop->husum/crop->phu);
    if(crop->fphu<par->fphusen)
    {
      c=par->fphuc/par->flaimaxc-par->fphuc;
      k=par->fphuk/par->flaimaxk-par->fphuk;
      crop->flaimax=crop->fphu/(crop->fphu+c*pow(c/k,(par->fphuc-crop->fphu)/(par->fphuk-par->fphuc)));
    }
    else
    {
      crop->senescence=TRUE;
      crop->flaimax=pow((1-crop->fphu)/(1-par->fphusen),par->shapesenescencenorm)
       *(1-par->flaimaxharvest)+par->flaimaxharvest;
    }
  }
  else
    harvesting=TRUE;
  
  if(crop->growingdays==hlimit)
    harvesting=TRUE;
  
  /* determination of lai */
  lai0=crop->lai;

  if(crop->senescence)
  {
    if(!crop->senescence0)
      crop->laimax_adjusted=lai0;
    crop->lai=crop->flaimax*crop->laimax_adjusted;
  }
  else
    crop->lai=crop->flaimax*laimax;
  
  lai00=crop->lai000; 
  crop->lai000=crop->lai;
  
  if(!crop->senescence)
  {
    /* scale daily LAI increment with minimum of wscal and vscal as simplest approach
       we could also compute how much N we have to support how much leaf area with optimal C/N ratios */
    lai_inc=(crop->lai-lai00)*min(pft->wscal,pft->vscal);
    crop->lai=lai_inc+lai0;
  }
  
  pft->phen=crop->lai/laimax;
  
  return harvesting;
} /* of 'phenology_crop' */

/*
- the function is called for each pft in update_daily():
  -> leaf_phenology(pft,temp,day,daylength,npft);
- initialisation of local variables
- daily computation of the phenological scalar between 0 at sowing and 1 at 
  maturity (harvest)
  -> calculation of hu (heat unit)
  -> calculation of vrf (vernalization reduction factor) dependent on pvd
     (vernalization days) to reduce hu (only for winter crops)
  -> calculation of prf (photoperiodism reduction factor) to reduce hu 
     (still inactive, yet)
  -> calculation of fphu (fraction of phenological heat unit)
  -> calculation of flaimax (fraction of maximum LAI) dependent of fphu
     -> two different shapes: - development and growth (increase)
                              - senescence (decrease)
  -> harvest if phenological scalar reaches 1 or growingdays (number of days for
     the growing period; see hlimit in pft.par) are reached
  -> determination of lai (leaf area index) and pft->phen

- returns TRUE on hdate, so that the landusetype will be set to KILL, the litter
  pools will be updated and the stand will be go to the set-aside stand 
  (see update_daily.c,setaside.c)
*/
