/**************************************************************************************/
/**                                                                                \n**/
/**             f  w  r  i  t  e  _  c  r  o  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes crop specific variables into binary file                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

Bool fwrite_crop(FILE *file, /**< pointer to file data */
                 const Pft *pft /**< pointer to PFT data */
                )               /** \return TRUE on error */
{
  return (fwrite(pft->data,sizeof(Pftcrop),1,file)!=1);
} /* of 'fwrite_crop' */
