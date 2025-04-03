/**************************************************************************************/
/**                                                                                \n**/
/**        f  r  e  a  d  _  b  i  o  m  a  s  s  _  t  r  e  e  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads biomass tree data of stand                                  \n**/
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

#define readint2(file,name,val,swap) if(readint(file,name,val,swap)) return TRUE

Bool fread_biomass_tree(FILE *file,   /**< pointer to binary file */
                        Stand *stand, /**< stand pointer */
                        Bool swap     /**< byte order has to be changed */
                       )              /** \return TRUE on error */
{
  Biomass_tree *biomass_tree;
  biomass_tree=new(Biomass_tree);
  stand->data=biomass_tree;
  if(biomass_tree==NULL)
  {
    printallocerr("biomass_tree");
    return TRUE;
  }
  if(fread_irrigation(file,"irrigation",&biomass_tree->irrigation,swap))
    return TRUE;
  readint2(file,"growing_days",&stand->growing_days,swap);
  readint2(file,"growing_time",&biomass_tree->growing_time,swap);
  return readint(file,"age",&biomass_tree->age,swap);
} /* of 'fread_biomass_tree' */
