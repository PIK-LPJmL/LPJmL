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

Bool freadoutputdata(FILE *file,          /**< pointer to restart file */
                     Output *output,      /**< output data */
                     Bool swap,           /**< byte order has to be changed? */
                     Config *config /**< LPJ configuration */
                    )
{
  if(freadint(&config->totalsize,1,swap,file)!=1)
    return TRUE;
  output->data=newvec(Real,config->totalsize);
  if(output->data==NULL)
  {
    printallocerr("data");
    return TRUE;
  }
  return freadreal(output->data,config->totalsize,swap,file)!=config->totalsize;
} /* of 'freadoutputdata' */
