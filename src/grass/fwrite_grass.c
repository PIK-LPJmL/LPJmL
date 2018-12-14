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

Bool fwrite_grass(FILE *file,    /**< pointer to binary file */
                  const Pft *pft /**< pointer to grass PFT */
                 )               /** \return TRUE on error */
{
  const Pftgrass *grass;
  grass=pft->data;
  fwrite(&grass->turn,sizeof(Grassphys),1,file);
  fwrite(&grass->max_leaf,sizeof(Real),1,file);
  fwrite(&grass->excess_carbon,sizeof(Real),1,file);
  fwrite(&grass->ind,sizeof(Grassphys),1,file);
  fwrite(&grass->falloc,sizeof(Grassphyspar),1,file);
  return fwrite(&grass->growing_days,sizeof(int),1,file)!=1;
} /* of 'fwrite_grass' */
