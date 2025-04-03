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

static Bool fwritegrassphys(FILE *file,const char *name,const Grassphys *grass)
{
  writestruct(file,name);
  writestocks(file,"leaf",&grass->leaf);
  writestocks(file,"root",&grass->root);
  return writeendstruct(file);
}

static Bool fwritegrassphyspar(FILE *file,const char *name,const Grassphyspar *grass)
{
  writestruct(file,name);
  writereal(file,"leaf",grass->leaf);
  writereal(file,"root",grass->root);
  return writeendstruct(file);
}

Bool fwrite_grass(FILE *file,    /**< pointer to binary file */
                  const Pft *pft /**< pointer to grass PFT */
                 )               /** \return TRUE on error */
{
  const Pftgrass *grass;
  grass=pft->data;
  fwritegrassphys(file,"turn",&grass->turn);
  fwritegrassphys(file,"turn_litt",&grass->turn_litt);
  writereal(file,"max_leaf",grass->max_leaf);
  writereal(file,"excess_carbon",grass->excess_carbon);
  fwritegrassphys(file,"ind",&grass->ind);
  fwritegrassphyspar(file,"falloc",&grass->falloc);
  return writeint(file,"growing_days",grass->growing_days);
} /* of 'fwrite_grass' */
