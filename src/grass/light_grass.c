/**************************************************************************************/
/**                                                                                \n**/
/**           l  i  g  h  t  _  g  r  a  s  s  .  c                                \n**/
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
#include "grass.h"

void light_grass(Litter *litter, /**< pointer to litter pools */
                 Pft *pft,       /**< pointer to grass PFT */
                 Real excess     /**< excess in FPC */
                )

{
  Grassphys m_kill; /* reduction in grass PFT mass to reduce grass cover
                       to permitted maximum (gC)*/
  Stocks lm_old;
  Pftgrass *grass;
  Real nind_kill;
  grass=pft->data;

  nind_kill=(excess<1e-20) ? 1 : pft->nind*(1-excess/pft->fpc);
#if 0
  if(nind_kill>pft->nind)
    nind_kill=pft->nind;
#endif
  lm_old=grass->ind.leaf;
  grass->ind.leaf.carbon*=nind_kill;
  //grass->ind.leaf.carbon=-2.0*log(1.0-(pft->fpc-excess*pft->fpc/fpc_total_grass))/getpftpar(pft,sla);
  if(grass->ind.leaf.nitrogen>0)
    grass->ind.leaf.nitrogen*=nind_kill;
    //grass->ind.leaf.nitrogen=-2.0*log(1.0-(pft->fpc-excess))/getpftpar(pft,sla)*grass->ind.leaf.nitrogen/lm_old.carbon;
  m_kill.leaf.carbon=lm_old.carbon-grass->ind.leaf.carbon;
  if(lm_old.carbon>epsilon)
    m_kill.root.carbon=grass->ind.root.carbon*(m_kill.leaf.carbon/lm_old.carbon);
  else
    m_kill.root.carbon=m_kill.leaf.carbon*getpftpar(pft,lmro_ratio);
  m_kill.leaf.nitrogen=lm_old.nitrogen-grass->ind.leaf.nitrogen;
  if(lm_old.nitrogen>epsilon)
    m_kill.root.nitrogen=grass->ind.root.nitrogen*(m_kill.leaf.nitrogen/lm_old.nitrogen);
  else
    m_kill.root.nitrogen=0;
  grass->ind.root.carbon-=m_kill.root.carbon;
  grass->ind.root.nitrogen-=m_kill.root.nitrogen;
  litter->item[pft->litter].ag.leaf.carbon+=m_kill.leaf.carbon*pft->nind;
  litter->item[pft->litter].ag.leaf.nitrogen+=m_kill.leaf.nitrogen*pft->nind;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,m_kill.leaf.carbon*pft->nind);
  litter->item[pft->litter].bg.carbon+=m_kill.root.carbon*pft->nind;
  litter->item[pft->litter].bg.nitrogen+=m_kill.root.nitrogen*pft->nind;
  fpc_grass(pft);
} /* of 'light_grass' */
