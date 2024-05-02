/**************************************************************************************/
/**                                                                                \n**/
/**           i  r  r  i  g  a  t  i  o  n  _  s  t  a  n  d  .  c                 \n**/
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
#include "agriculture.h"

void irrigation_stand(Stand *stand, /* pointer to non-natural stand */
                      Real gp_stand,
                      Real wet[],   /* wet array for PFT list */
                      Real pet      /* potential evapotranspiration (mm) */
                     ) 
{
  Pft *pft;
  int p,l;
  Real supply,demand,wr,satlevel;
  Real soildepth_irrig,nir,dist;
  Irrigation *data;
  data=stand->data;

  foreachpft(pft,p,&stand->pftlist)
  {
    wr=0;
    for(l=0;l<LASTLAYER;l++)
      wr+=pft->par->rootdist[l]*(stand->soil.w[l]+stand->soil.ice_depth[l]/stand->soil.par->whcs[l]);
    
    if(pft->stand->type->landusetype==AGRICULTURE)
    {
      supply=pft->par->emax*wr*(1-exp(-1.0*pft->par->sla*((Pftcrop *)pft->data)->ind.root));
      //demand=(gp_stand>0 && pft->phen>0 && fpar(pft)>0) ? pet*param.ALPHAM/(1+param.GM/(gp_stand/pft->phen*fpar(pft))) : 0;
      demand=(gp_stand>0 && pft->phen>0 && fpar(pft)>0) ? pet*param.ALPHAM/(1+(param.GM*param.ALPHAM)/(gp_stand/pft->phen*fpar(pft))) : 0;
    }
    else
    {
      supply=pft->par->emax*wr*pft->phen;
      //demand=(gp_stand>0) ? pet*param.ALPHAM/(1+param.GM/gp_stand) : 0;
      demand=(gp_stand>0) ? pet*param.ALPHAM/(1+(param.GM*param.ALPHAM)/gp_stand) : 0;
   }

    if(supply<demand && pft->phen>0.0)
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
          nir+=max(0,(param.irrigation_soilfrac-stand->soil.w[l]-stand->soil.ice_depth[l]/stand->soil.par->whcs[l])*stand->soil.par->whcs[l]*min(1,soildepth_irrig/soildepth[l])*(1-stand->soil.freeze_depth[l]/soildepth[l]));
          /* farmer's estimate of distribution requirements: satlevel scales max freewater; demand = max freewater + whcs - available water */
          dist+=max(0,((stand->soil.par->wsats[l]-stand->soil.par->wpwps[l]-stand->soil.par->whcs[l])*satlevel-stand->soil.w_fw[l])*min(1,soildepth_irrig/soildepth[l])*(1-stand->soil.freeze_depth[l]/soildepth[l]));
        }
        l++;
      }while((soildepth_irrig-=soildepth[l-1])>0);
      if(data->irrig_system==SPRINK)
        dist+=interception(&wet[p],pft,pet,nir+dist); /* proxy for interception of next day, based on current wet */

#ifdef DEBUG
      printf("demand:%f supply::%f irrig:%f\n",demand,supply,nir+dist);
#endif

      if(nir>data->net_irrig_amount) /* for pft loop */
        data->net_irrig_amount=nir;
      if(dist>data->dist_irrig_amount)
        data->dist_irrig_amount=dist;
    }
  } /* of foreachpft() */
} /* of 'irrigation_stand' */
