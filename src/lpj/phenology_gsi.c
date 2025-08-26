/**************************************************************************************/
/**                                                                                \n**/
/**             p  h  e  n  o  l  o  g  y  _  g  s  i  .  c                        \n**/
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

void phenology_gsi(Pft *pft,    /**< pointer to PFT variables */
                   Real temp,   /**< temperature (deg C) */
                   Real light,  /**< light, i.e. shortwave-downward radiation (W m-2) */
                   int day,     /**< day of the year */
                   Bool isdaily, /**< daily temperature data (TRUE/FALSE) */
                   Real daylength, /**< day length (h) */
                   const Config *config /**< LPJmL configuration */
                  )
{
  const Pfttreepar *treepar;
  Pfttree *tree;
  /* get parameters */
  Phen_param tminpar = getpftpar(pft, tmin);
  Phen_param tmaxpar = getpftpar(pft, tmax);
  Phen_param lightpar = getpftpar(pft, light);
  Phen_param wscalpar = getpftpar(pft, wscal);

  /* cold temperature response function */
  pft->phen_gsi.tmin += ( 1 / (1 + exp(-tminpar.sl * (temp - tminpar.base))) - pft->phen_gsi.tmin) * tminpar.tau;
  pft->phen_gsi.tmin=max(epsilon,pft->phen_gsi.tmin);
  /* heat stress response function */
  pft->phen_gsi.tmax += ( 1 / (1 + exp(tmaxpar.sl * (temp - tmaxpar.base))) - pft->phen_gsi.tmax) * tmaxpar.tau;
  pft->phen_gsi.tmax=max(epsilon,pft->phen_gsi.tmax);

  /* photosynthetic active radiation response function */
  if(-lightpar.sl * (light - lightpar.base)<200) /* check to avoid overflow in exp function */
    pft->phen_gsi.light += ( 1 / (1 + exp(-lightpar.sl * (light - lightpar.base))) - pft->phen_gsi.light) * lightpar.tau;
  else
    pft->phen_gsi.light -=pft->phen_gsi.light * lightpar.tau;
  pft->phen_gsi.light=max(epsilon,pft->phen_gsi.light);
  /* water availability response function */
  pft->phen_gsi.wscal += ( 1 / (1 + exp(-wscalpar.sl * (pft->wscal*100 - wscalpar.base))) - pft->phen_gsi.wscal) * wscalpar.tau;
  pft->phen_gsi.wscal=max(epsilon,pft->phen_gsi.wscal);
  if(istree(pft))
  {
    treepar=pft->par->data;
    tree=pft->data;
  }
  /* phenology */
  if(istree(pft) && treepar->evergreen)
  {
    pft->phen=1.0;
  }
  else if(!istree(pft) || !tree->isphen)
  {
    pft->phen = pft->phen_gsi.tmin * pft->phen_gsi.tmax * pft->phen_gsi.light * pft->phen_gsi.wscal;
  }

  turnover_daily(&pft->stand->soil.litter,pft,temp,day,isdaily,config);
  if(istree(pft) && treepar->rainyseason)
  {
    if(day==pft->stand->cell->climbuf.startday_rainyseason)
    {
      pft->aphen=0.0;
      tree->isphen=FALSE;
    }
  }
  else
  {
    if((pft->stand->cell->coord.lat>=0.0 && day==COLDEST_DAY_NHEMISPHERE) ||
        (pft->stand->cell->coord.lat<0.0 && day==COLDEST_DAY_SHEMISPHERE))
    {
      pft->aphen=0.0;
      if(istree(pft))
        tree->isphen=FALSE;
    }
  }
  pft->aphen+=pft->phen;
} /* of 'phenology_gsi' */
