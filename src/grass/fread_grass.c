/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  a  d  _  g  r  a  s  s  .  c                            \n**/
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

Bool fread_grass(FILE *file, /**< pointer to binary file */
                 Pft *pft,   /**< pointer to PFT data read */
                 Bool swap   /**< byte order has to be swapped (TRUE/FALSE) */
                )            /** \return TRUE on error */
{
  Pftgrass *grass;
  grass=new(Pftgrass);
  pft->data=grass;
  if(grass==NULL)
  {
    printallocerr("grass");
    return TRUE;
  }
  freadreal((Real *)&grass->turn,sizeof(Grassphys)/sizeof(Real),swap,file);
  freadreal((Real *)&grass->ind,sizeof(Grassphys)/sizeof(Real),swap,file);
  return freadreal((Real *)&grass->max_leaf,1,swap,file)!=1;
} /* of 'fread_grass' */
