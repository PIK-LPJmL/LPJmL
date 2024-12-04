/**************************************************************************************/
/**                                                                                \n**/
/**                         c  a  l  c  _  n  i  r  .  c                           \n**/
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

void calc_nir(Stand *stand,        /**< pointer to non-natural stand */
              Irrigation *data,    /**< irrigation data */
              Real gp_stand,       /**< potential canopy conductivity */
              Real wet[],          /**< wet array for PFT list */
              Real eeq,            /**< equilibrium evapotranspiration (mm) */
              const Config *config /**< LPJmL configuration */
             )
{
  Pft *pft;
  int p,l;
  Real supply,demand,wr,satlevel;
  Real soildepth_irrig,nir,dist;
  nir=dist=0;

  foreachpft(pft,p,&stand->pftlist)
  {
    wr=getwr(&stand->soil,pft->par->rootdist);

    if(stand->type->landusetype==AGRICULTURE || (stand->type->landusetype==OTHERS && config->others_to_crop))
    {
      supply=pft->par->emax*wr*(1-exp(-0.04*((Pftcrop *)pft->data)->ind.root.carbon));
      demand=(gp_stand>0 && pft->phen>0 && fpar(pft)>0) ? eeq*param.ALPHAM/(1+(param.GM*param.ALPHAM)/(gp_stand/pft->phen*fpar(pft))) : 0;
    }
    else
    {
      supply=pft->par->emax*wr*pft->phen;
      demand=(gp_stand>0) ? eeq*param.ALPHAM/(1+(param.GM*param.ALPHAM)/gp_stand) : 0;
    }
    if(pft->par->id==config->rice_pft)
    {
      demand=satwater(&stand->soil)-rootwater(&stand->soil);
      soildepth_irrig=SOILDEPTH_IRRIG;
      nir=demand;
      dist=0;
      l=0;
      do
      {
        if (stand->soil.freeze_depth[l]< soildepth[l])
        {
          dist+=max(0,((stand->soil.wsats[l]-stand->soil.wpwps[l]-stand->soil.whcs[l])*param.sat_level[0]-stand->soil.w_fw[l])*min(1,soildepth_irrig/soildepth[l])*(1-stand->soil.freeze_depth[l]/soildepth[l]));
        }
         l++;
       }while((soildepth_irrig-=soildepth[l-1])>0);
    }
    else if(supply<demand && pft->phen>0.0)
    {
      /* level free water to be requested based on irrigation system */
      satlevel=param.sat_level[data->irrig_system];

      soildepth_irrig=SOILDEPTH_IRRIG;
      l=0;
      nir=0.0;
      dist=0.0;
      do
      {
        if (stand->soil.freeze_depth[l]< soildepth[l])
        {
          /* net irrigation requirement: field capacity soil water deficit */
          nir+=max(0,(param.irrigation_soilfrac-stand->soil.w[l]-stand->soil.ice_depth[l]/stand->soil.whcs[l])*stand->soil.whcs[l]*min(1,soildepth_irrig/soildepth[l])*(1-stand->soil.freeze_depth[l]/soildepth[l]));
          /* farmer's estimate of distribution requirements: satlevel scales max freewater; demand = max freewater + whcs - available water */
          dist+=max(0,((stand->soil.wsats[l]-stand->soil.wpwps[l]-stand->soil.whcs[l])*satlevel-stand->soil.w_fw[l])*min(1,soildepth_irrig/soildepth[l])*(1-stand->soil.freeze_depth[l]/soildepth[l]));
        }
        l++;
      }while((soildepth_irrig-=soildepth[l-1])>0);
      if(data->irrig_system==SPRINK)
        dist+=interception(&wet[p],pft,eeq,nir+dist); /* proxy for interception of next day, based on current wet */

#ifdef DEBUG2
      printf("demand:%f supply::%f irrig:%f\n",demand,supply,nir+dist);
#endif
    }
    /* avoid large irrigation amounts for dist if nir is zero */
    if(nir<1)
      dist=0;
    if(nir>data->net_irrig_amount) /* for pft loop */
      data->net_irrig_amount=nir;
    if(dist>data->dist_irrig_amount)
      data->dist_irrig_amount=dist;

#ifdef DEBUG2
    if(pft->par->id==config->rice_pft)
      fprintf(stdout,"nir: %g dist: %g supply: %g demand: %g net_irrig_amount: %g dist_irrig_amount: %g\n",nir,dist,supply,demand,data->net_irrig_amount,data->dist_irrig_amount);
#endif
  } /* of foreachpft() */

} /* of 'calc_nir' */
