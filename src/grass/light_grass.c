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

void light_grass(Litter *litter,Pft *pft,Real excess, Real fpc_total_grass)

{
  Grassphys  m_kill; /* reduction in grass PFT mass to reduce grass cover
                        to permitted maximum (gC)*/
  Real lm_old;
  Pftgrass *grass;
  grass=pft->data;
  lm_old=grass->ind.leaf;
  grass->ind.leaf=-2.0*log(1.0-(pft->fpc-excess*pft->fpc/fpc_total_grass))/getpftpar(pft,sla);
  m_kill.leaf=lm_old-grass->ind.leaf;
  if(lm_old>0)
    m_kill.root=grass->ind.root*(m_kill.leaf/lm_old);
  else
    m_kill.root=m_kill.leaf*getpftpar(pft,lmro_ratio);
  grass->ind.root-=m_kill.root;
  litter->ag[pft->litter].trait.leaf+=m_kill.leaf;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,m_kill.leaf);
  litter->bg[pft->litter]+=m_kill.root;
  fpc_grass(pft);
} /* of 'light_grass' */
