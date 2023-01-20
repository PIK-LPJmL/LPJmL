/**************************************************************************************/
/**                                                                                \n**/
/**                f  w  r  i  t  e  o  u  t  p  u  t  d  a  t  a  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes output data to restart file                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fwriteoutputdata(FILE *file,           /**< pointer to restart file */
                      const Output *output, /**< output data */
                      const Config *config  /**< LPJ configuration */
                     )
{
  fwrite(&config->totalsize,sizeof(int),1,file);
  fwrite(output->data,sizeof(Real),config->totalsize,file);
} /* of 'fwriteoutputdata' */
