/**************************************************************************************/
/**                                                                                \n**/
/**                  g  e  t  c  f  t  n  a  m  e  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets name of band for land use input                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

const char *getcftname(int index,           /**< index of band */
                       int npft,            /**< number of natural PFTs */
                       int ncft,            /**< number of crop PFTs */
                       const Config *config /**< LPJmL configuration */
                      )                     /** \return name of band or NULL */
{
  if(index>=0 && index<ncft)
    return config->pftpar[npft+index].name;
  index-=ncft;
  if(index<NGRASS)
    return grassland_names[index];
  index-=NGRASS;
  if(index<NBIOMASSTYPE)
    return biomass_names[index];
  index-=NBIOMASSTYPE;
  if(index<config->nwptype)
    return woodplantation_names[index];
  index-=config->nwptype;
  if(index<config->nagtree)
    return config->pftpar[npft-config->nagtree+index].name;
  return NULL;
} /* of 'getcftname' */
