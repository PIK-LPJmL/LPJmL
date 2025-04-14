/**************************************************************************************/
/**                                                                                \n**/
/**                 f  w  r  i  t  e  _  g  r  a  s  s  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function writes grass-specific PFT variables into restart file                 \n**/
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

static Bool fwritegrassphys(Bstruct file,const char *name,const Grassphys *grass)
{
  bstruct_writestruct(file,name);
  fwritestocks(file,"leaf",&grass->leaf);
  fwritestocks(file,"root",&grass->root);
  return bstruct_writeendstruct(file);
}

static Bool fwritegrassphyspar(Bstruct file,const char *name,const Grassphyspar *grass)
{
  bstruct_writestruct(file,name);
  bstruct_writereal(file,"leaf",grass->leaf);
  bstruct_writereal(file,"root",grass->root);
  return bstruct_writeendstruct(file);
}

Bool fwrite_grass(Bstruct file,  /**< pointer to binary file */
                  const Pft *pft /**< pointer to grass PFT */
                 )               /** \return TRUE on error */
{
  const Pftgrass *grass;
  grass=pft->data;
  fwritegrassphys(file,"turn",&grass->turn);
  fwritegrassphys(file,"turn_litt",&grass->turn_litt);
  bstruct_writereal(file,"max_leaf",grass->max_leaf);
  bstruct_writereal(file,"excess_carbon",grass->excess_carbon);
  fwritegrassphys(file,"ind",&grass->ind);
  fwritegrassphyspar(file,"falloc",&grass->falloc);
  return bstruct_writeint(file,"growing_days",grass->growing_days);
} /* of 'fwrite_grass' */
