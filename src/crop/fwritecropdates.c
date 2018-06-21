/**************************************************************************************/
/**                                                                                \n**/
/**        f  w  r  i  t  e  c  r  o  p  d  a  t  e  s  .  c                       \n**/
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

Bool fwritecropdates(FILE *file, /**< pointer to binary file */
                     const Cropdates *cropdates, /**< array of crop dates to write */
                     int ncft /**< number of crop dates */
                    )         /** \return TRUE on error */
{
  return fwrite(cropdates,sizeof(Cropdates),ncft,file)!=ncft;
} /* of 'fwritecropdates' */
