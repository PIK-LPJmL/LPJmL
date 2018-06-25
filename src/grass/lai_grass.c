/**************************************************************************************/
/**                                                                                \n**/
/**             l  a  i  _  g  r  a  s  s  .  c                                    \n**/
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

Real lai_grass(const Pft *pft)
{
  return ((Pftgrass *)pft->data)->ind.leaf*getpftpar(pft,sla);
} /* 'lai_grass' */
/*
- this function is called in fpc_grass() and returns the maximum lai of a grass (last year's LAI)
*/
Real actual_lai_grass(const Pft *pft)
{
  return ((Pftgrass *)pft->data)->ind.leaf*getpftpar(pft,sla)*pft->phen;
} /* 'actual_lai_grass' */
