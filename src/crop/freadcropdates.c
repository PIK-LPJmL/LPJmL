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

static Bool readcropdate(FILE *file,Cropdates *cropdates,Bool swap)
{
  if(readstruct(file,NULL,swap))
    return TRUE;
  if(readint(file,"fall_sdate20",&cropdates->fall_sdate20,swap))
    return TRUE;
  if(readint(file,"last_update_fall",&cropdates->last_update_fall,swap))
    return TRUE;
  if(readint(file,"spring_sdate20",&cropdates->spring_sdate20,swap))
    return TRUE;
  if(readint(file,"last_update_spring",&cropdates->last_update_spring,swap))
    return TRUE;
  if(readint(file,"vern_date20",&cropdates->vern_date20,swap))
    return TRUE;
  if(readint(file,"last_update_vern",&cropdates->last_update_vern,swap))
    return TRUE;
  return readendstruct(file);
} /* of 'readcropdate' */

Cropdates *freadcropdates(FILE *file, /**< Pointer to binary file */
                          const char *name, /**< name of object */
                          int ncft,   /**< Number of crop PFTs */
                          Bool swap   /**< byte order has to be changed */
                         )            /** \return allocated cropdates */
{
  int size,cft;
  Cropdates *cropdates;
  if(readarray(file,name,&size,swap))
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
    if(readcropdate(file,cropdates+cft,swap))
    {
      free(cropdates);
      return NULL;
    }
  }
  if(readendarray(file))
  {
    free(cropdates);
    return NULL;
  }
  return cropdates;
} /* of 'freadcropdates' */
