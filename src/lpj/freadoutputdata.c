/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  a  d  o  u  t  p  u  t  d  a  t  a  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads output data from restart file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadoutputdata(Bstruct file,     /**< pointer to restart file */
                     const char *name, /**< name of object */
                     Output *output,   /**< output data */
                     Config *config    /**< LPJ configuration */
                    )
{
  output->data=bstruct_readvarrealarray(file,name,&config->totalsize);
  return (output->data==NULL);
} /* of 'freadoutputdata' */
