/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  l  i  t  t  e  r  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes litter variables into binary file                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwritelitter(FILE *file, /**< pointer to binary file */
                  const Litter *litter /**< pointer to litter data written */
                 )                     /** \return TRUE on error */
{
  Byte b;
  int l;
  fwrite(litter->avg_fbd,sizeof(Real),NFUELCLASS+1,file);
  b=(Byte)litter->n;
  fwrite1(&b,sizeof(b),file);
  for(l=0;l<litter->n;l++)
  {
    b=(Byte)litter->ag[l].pft->id;
    fwrite1(&b,sizeof(b),file);
    fwrite1(&litter->ag[l].trait,sizeof(Trait),file);
    fwrite1(&litter->bg[l],sizeof(Real),file);
  }
  return FALSE;
} /* of 'fwritelitter' */
