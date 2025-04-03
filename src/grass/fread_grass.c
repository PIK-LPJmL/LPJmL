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

static Bool freadgrassphys(FILE *file,const char *name,Grassphys *grass,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readstocks(file,"leaf",&grass->leaf,swap))
    return TRUE;
  if(readstocks(file,"root",&grass->root,swap))
    return TRUE;
  return readendstruct(file);
}

static Bool freadgrassphyspar(FILE *file,const char *name,Grassphyspar *grass,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readreal(file,"leaf",&grass->leaf,swap))
    return TRUE;
  if(readreal(file,"root",&grass->root,swap))
    return TRUE;
  return readendstruct(file);
}

Bool fread_grass(FILE *file, /**< pointer to binary file */
                 Pft *pft,   /**< pointer to PFT data read */
                 Bool UNUSED(separate_harvests),
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
  pft->nlimit=0.0;
  if(freadgrassphys(file,"turn",&grass->turn,swap))
    return TRUE;
  if(freadgrassphys(file,"turn_litt",&grass->turn_litt,swap))
    return TRUE;
  if(readreal(file,"max_leaf",&grass->max_leaf,swap))
    return TRUE;
  if(readreal(file,"excess_carbon",&grass->excess_carbon,swap))
    return TRUE;
  if(freadgrassphys(file,"ind",&grass->ind,swap))
    return TRUE;
  if(freadgrassphyspar(file,"falloc",&grass->falloc,swap))
    return TRUE;
  return readint(file,"growing_days",&grass->growing_days,swap);
} /* of 'fread_grass' */
