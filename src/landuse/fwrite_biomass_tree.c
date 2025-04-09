/**************************************************************************************/
/**                                                                                \n**/
/**      f  w  r  i  t  e  _  b  i  o  m  a  s  s  _  t  r  e  e  .  c             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes biomass tree data of stand                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "biomass_tree.h"

Bool fwrite_biomass_tree(Bstruct file,      /**< pointer to binary file */
                         const Stand *stand /**< stand pointer */
                        )                   /** \return TRUE on error */
{
  const Biomass_tree *biomass_tree;
  biomass_tree=stand->data;
  fwrite_irrigation(file,"irrigation",&biomass_tree->irrigation);
  bstruct_writeint(file,"growing_days",stand->growing_days);
  bstruct_writeint(file,"growing_time",biomass_tree->growing_time);
  return bstruct_writeint(file,"age",biomass_tree->age);
} /* of 'fwrite_biomass_tree' */
