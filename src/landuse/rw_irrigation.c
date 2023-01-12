/**************************************************************************************/
/**                                                                                \n**/
/**                     r w _ i r r i g a t i o n . c                              \n**/
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

#define MAX_RW_IRRIG 1.0

Real rw_irrigation(Stand *stand,     /**< Pointer to non-natural stand */
                   Real gp_stand,    /**< potential stomata conductance */
                   const Real wet[], /**< wet array for PFT list */
                   Real eeq,         /**< potential evapotranspiration (mm) */
                   const Config *config /**<  LPJmL configuration */
                  )                  /** \return irrigation applied (mm) */
{

  Pft *pft;
  int p,l;
  Real supply,demand,irrig_apply=0,wr;
  Real soildepth_irrig;

  /* calculate required irrigation amount */
  foreachpft(pft,p,&stand->pftlist)
  {
    wr=getwr(&stand->soil,pft->par->rootdist);

    if(pft->stand->type->landusetype==AGRICULTURE || (pft->stand->type->landusetype==OTHERS && config->others_to_crop))
    {
      supply=pft->par->emax*wr*(1-exp(-1.0*pft->par->sla*((Pftcrop *)pft->data)->ind.root.carbon));
      //demand=(gp_stand>0 && pft->phen>0 && fpar(pft)>0) ? (1.0-wet[p])*eeq*param.ALPHAM/(1+param.GM/(gp_stand/pft->phen*fpar(pft))) : 0;
      demand=(gp_stand>0 && pft->phen>0 && fpar(pft)>0) ? (1.0-wet[p])*eeq*param.ALPHAM/(1+(param.GM*param.ALPHAM)/(gp_stand/pft->phen*fpar(pft))) : 0;
    }
    else
    {
      supply=pft->par->emax*wr*pft->phen;
      //demand=(gp_stand>0) ? (1.0-wet[p])*eeq*param.ALPHAM/(1+param.GM/gp_stand) : 0;
      demand=(gp_stand>0) ? (1.0-wet[p])*eeq*param.ALPHAM/(1+(param.GM*param.ALPHAM)/gp_stand) : 0;
    }

    if(wr<param.rw_irrig_thres && supply<demand && pft->phen>0.0)
    {
      soildepth_irrig=SOILDEPTH_IRRIG;
      l=0;
      do
      {
        if (stand->soil.freeze_depth[l]< soildepth[l])
          irrig_apply+=max(0,(MAX_RW_IRRIG-stand->soil.w[l]-stand->soil.ice_depth[l]/stand->soil.whcs[l])*stand->soil.whcs[l]*min(1,soildepth_irrig/(soildepth[l]-stand->soil.freeze_depth[l])));
        l++;
      }while((soildepth_irrig-=soildepth[l-1])>0);
    }
  } /* of foreachpft() */

  /* take it from buffer */
  irrig_apply=min(irrig_apply,stand->soil.rw_buffer);
  stand->soil.rw_buffer-=irrig_apply;

  /* write to output */
  getoutput(&stand->cell->output,IRRIG_RW,config)+=irrig_apply*stand->frac;

  return irrig_apply;
} /* of 'rw_irrigation' */
