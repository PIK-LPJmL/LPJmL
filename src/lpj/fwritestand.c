/**************************************************************************************/
/**                                                                                \n**/
/**                 f  w  r  i  t  e  s  t  a  n  d  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes stand data to binary file                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwritestand(FILE *file,        /**< pointer to binary file */
                 const Stand *stand, /**< pointer to stand */
                 int ntotpft        /**< total number of PFTs*/
                )                   /** \return TRUE on error */
{
  Byte b;
  /* write PFT list to file */
  if(fwritepftlist(file,&stand->pftlist)!=getnpft(&stand->pftlist))
    return TRUE;
  /* write soil data to file */
  if(fwritesoil(file,&stand->soil,ntotpft))
    return TRUE;
  fwrite1(&stand->frac,sizeof(Real),file);
  b=(Byte)stand->type->landusetype;
  fwrite(&b,sizeof(b),1,file);
  /* write stand-specific data */
  if(stand->type->fwrite(file,stand))
    return TRUE;
  return (fwrite(stand->frac_g,sizeof(Real),NSOILLAYER,file)!=NSOILLAYER);
} /* of 'fwritestand' */
