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

void light_grass(Litter *litter,Pft *pft,Real excess)

{
  Grassphys  m_kill; /* reduction in grass PFT mass to reduce grass cover
                        to permitted maximum (gC)*/
  Stocks lm_old;
  Pftgrass *grass;
  grass=pft->data;
  
  lm_old=grass->ind.leaf;
  grass->ind.leaf.carbon=-2.0*log(1.0-(pft->fpc-excess))/getpftpar(pft,sla);
  if(grass->ind.leaf.nitrogen>0)
    grass->ind.leaf.nitrogen=-2.0*log(1.0-(pft->fpc-excess))/getpftpar(pft,sla)*grass->ind.leaf.nitrogen/lm_old.carbon;
  m_kill.leaf.carbon=lm_old.carbon-grass->ind.leaf.carbon;
  if(lm_old.carbon>0)
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
  litter->ag[pft->litter].trait.leaf.carbon+=m_kill.leaf.carbon;
  litter->ag[pft->litter].trait.leaf.nitrogen+=m_kill.leaf.nitrogen;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,m_kill.leaf.carbon);
  litter->bg[pft->litter].carbon+=m_kill.root.carbon;
  litter->bg[pft->litter].nitrogen+=m_kill.root.nitrogen;
  fpc_grass(pft);
} /* of 'light_grass' */
