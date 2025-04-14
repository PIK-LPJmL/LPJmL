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

static Bool freadgrassphys(Bstruct file,const char *name,Grassphys *grass)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(freadstocks(file,"leaf",&grass->leaf))
    return TRUE;
  if(freadstocks(file,"root",&grass->root))
    return TRUE;
  return bstruct_readendstruct(file);
} /* of 'freadgrassphys' */

static Bool freadgrassphyspar(Bstruct file,const char *name,Grassphyspar *grass)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(bstruct_readreal(file,"leaf",&grass->leaf))
    return TRUE;
  if(bstruct_readreal(file,"root",&grass->root))
    return TRUE;
  return bstruct_readendstruct(file);
} /* of 'freadgrassphyspar' */

Bool fread_grass(Bstruct file, /**< pointer to binary file */
                 Pft *pft,   /**< pointer to PFT data read */
                 Bool UNUSED(separate_harvests)
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
  if(freadgrassphys(file,"turn",&grass->turn))
    return TRUE;
  if(freadgrassphys(file,"turn_litt",&grass->turn_litt))
    return TRUE;
  if(bstruct_readreal(file,"max_leaf",&grass->max_leaf))
    return TRUE;
  if(bstruct_readreal(file,"excess_carbon",&grass->excess_carbon))
    return TRUE;
  if(freadgrassphys(file,"ind",&grass->ind))
    return TRUE;
  if(freadgrassphyspar(file,"falloc",&grass->falloc))
    return TRUE;
  return bstruct_readint(file,"growing_days",&grass->growing_days);
} /* of 'fread_grass' */
