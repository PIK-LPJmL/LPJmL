/**************************************************************************************/
/**                                                                                \n**/
/**                 g  e  t  r  o  u  t  e  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads river route from binary file                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool getroute(FILE *file, /**< pointer to binary file */
              Routing *r, /**< River route read */
              Bool swap   /**< Byte order has to be changed */
             )            /** \return TRUE on error */
{
  /* Function reads river route from file */
  if(fread(r,sizeof(Routing),1,file)!=1)
    return TRUE;
  if(swap)
  {
    /* byte order has to be changed */
    r->index=swapint(r->index);
    r->len=swapint(r->len);
  }
  return FALSE;
} /* of 'getroute' */
