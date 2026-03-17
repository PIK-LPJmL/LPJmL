/**************************************************************************************/
/**                                                                                \n**/
/**                 n  e  w  i  r  r  i  g  s  y  s  t  e  m  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function allocates and initializes irrigation system struct                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/
#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return NULL; }

Irrig_system *newirrigsystem(int ncft,   /**< number of crop PFTs */
                             int nagtree /**< number of agricultural trees */
                            )            /** \return pointer to allocated irrigation system or NULL */
{
  Irrig_system *irrig_system;
  irrig_system=new(Irrig_system);
  checkptr(irrig_system);
  irrig_system->crop=newvec(IrrigationType,ncft);
  checkptr(irrig_system->crop);
  irrig_system->ag_tree=newvec(IrrigationType,nagtree);
  checkptr(irrig_system->ag_tree);
  initirrigsystem(irrig_system,NOIRRIG,ncft,nagtree);
  return irrig_system;
} /* of 'newirrigsystem' */
