/**************************************************************************************/
/**                                                                                \n**/
/**     f  r  e  a  d   c  r  o  p  d  a  t  e  s  .  c                            \n**/
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

static Bool readcropdate(Bstruct file,Cropdates *cropdates)
{
  if(bstruct_readstruct(file,NULL))
    return TRUE;
  if(bstruct_readint(file,"fall_sdate20",&cropdates->fall_sdate20))
    return TRUE;
  if(bstruct_readint(file,"last_update_fall",&cropdates->last_update_fall))
    return TRUE;
  if(bstruct_readint(file,"spring_sdate20",&cropdates->spring_sdate20))
    return TRUE;
  if(bstruct_readint(file,"last_update_spring",&cropdates->last_update_spring))
    return TRUE;
  if(bstruct_readint(file,"vern_date20",&cropdates->vern_date20))
    return TRUE;
  if(bstruct_readint(file,"last_update_vern",&cropdates->last_update_vern))
    return TRUE;
  return bstruct_readendstruct(file,NULL);
} /* of 'readcropdate' */

Cropdates *freadcropdates(Bstruct file, /**< Pointer to binary file */
                          const char *name, /**< name of object */
                          int ncft    /**< Number of crop PFTs */
                         )            /** \return allocated cropdates */
{
  int size,cft;
  Cropdates *cropdates;
  if(bstruct_readarray(file,name,&size))
    return NULL;
  if(size!=ncft)
    return NULL;
  cropdates=newvec(Cropdates,ncft);
  if(cropdates==NULL)
  {
    printallocerr("cropdates");
    return NULL;
  }
  for(cft=0;cft<ncft;cft++)
  {
    if(readcropdate(file,cropdates+cft))
    {
      free(cropdates);
      return NULL;
    }
  }
  if(bstruct_readendarray(file,name))
  {
    free(cropdates);
    return NULL;
  }
  return cropdates;
} /* of 'freadcropdates' */
