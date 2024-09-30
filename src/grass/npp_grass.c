/**************************************************************************************/
/**                                                                                \n**/
/**                    n  p  p  _  g  r  a  s  s  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates daily net primary productivity of grass                \n**/
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

Real npp_grass(Pft *pft,               /**< PFT variables */
               Real UNUSED(gtemp_air), /**< value of air temperature response function */
               Real gtemp_soil,        /**< value of soil temperature response function */
               Real assim,             /**< assimilation (gC/m2) */
               const Config *config,       /**< [in] LPJ configuration */
               int with_nitrogen       /**< with nitrogen (TRUE/FALSE) */
              )                        /** \return net primary productivity (gC/m2) */
{
  Pftgrass *grass;
  const Pftgrasspar *par;
  Real npp,mresp,gresp,nc_root;
  grass=pft->data;
  par=pft->par->data;
  if(with_nitrogen && grass->ind.root.carbon>epsilon)
    nc_root=grass->ind.root.nitrogen/grass->ind.root.carbon;
  else
    nc_root=par->nc_ratio.root;
  //maximum respiration dependency to NC ratio not higher than measured leaf NC
  if(nc_root>pft->par->ncleaf.high/par->ratio) nc_root=pft->par->ncleaf.high/par->ratio;

  mresp=grass->ind.root.carbon*pft->nind*pft->par->respcoeff*param.k*nc_root*gtemp_soil*pft->phen;
  gresp=(assim-mresp)*param.r_growth;
  if (gresp<0.0) gresp=0.0;
#ifdef DEBUG3
  printf("mresp=%g gresp=%g assim=%g bminc=%g\n",mresp,gresp,assim,pft->bm_inc.carbon);
#endif
  npp=assim-mresp-gresp;
  pft->bm_inc.carbon+=npp;
  getoutput(&pft->stand->cell->output,RA,config)+=(mresp+gresp)*pft->stand->frac;
  return npp;
} /* of 'npp_grass' */
