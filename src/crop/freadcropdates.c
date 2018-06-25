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

Cropdates *freadcropdates(FILE *file, /**< Pointer to binary file */
                          int ncft,   /**< Number of crop PFTs */
                          Bool swap   /**< byte order has to be changed */
                         )            /** \return allocated cropdates */
{
  Cropdates *cropdates;
  cropdates=newvec(Cropdates,ncft);
  if(cropdates==NULL)
    return NULL;
  freadint((int *)cropdates,sizeof(Cropdates)/sizeof(int)*ncft,swap,file);
  return cropdates;
} /* of 'freadcropdates' */
