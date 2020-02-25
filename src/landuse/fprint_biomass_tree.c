/**************************************************************************************/
/**                                                                                \n**/
/**      f  p  r  i  n  t  _  b  i  o  m  a  s  s  _  t  r  e  e  .  c             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints biomass tree data of stand                                 \n**/
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

void fprint_biomass_tree(FILE *file,        /**< pointer to text file */
                         const Stand *stand /**< pointer to stand */
                        )
{
  const Biomass_tree *biomass_tree;
  biomass_tree=stand->data;
  fprint_irrigation(file,&biomass_tree->irrigation);
  fprintf(file,"Growing time:\t%d (yr)\n"
          "Age:\t\t%d (yr)\n",
          biomass_tree->growing_time,
          biomass_tree->age);
} /* of 'fprint_biomass_tree' */
