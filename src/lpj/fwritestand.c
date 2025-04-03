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

Bool fwritestand(FILE *file,         /**< pointer to restart file */
                 const char *name,   /**< name of object */
                 const Stand *stand, /**< pointer to stand */
                 int ntotpft         /**< total number of PFTs*/
                )                    /** \return TRUE on error */
{
  writestruct(file,name);
  writebyte(file,"landusetype",stand->type->landusetype);
  /* write PFT list to file */
  if(fwritepftlist(file,"pftlist",&stand->pftlist)!=getnpft(&stand->pftlist))
    return TRUE;
  /* write soil data to file */
  if(fwritesoil(file,"soil",&stand->soil,ntotpft))
    return TRUE;
  writereal(file,"frac",stand->frac);
  /* write stand-specific data */
  if(stand->type->fwrite(file,stand))
    return TRUE;
  writerealarray(file,"frac_g",stand->frac_g,NSOILLAYER);
  return writeendstruct(file);
} /* of 'fwritestand' */
