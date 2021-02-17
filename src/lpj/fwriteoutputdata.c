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

#define iswrite(index) (config->outnames[index].timestep>1)

#define writeoutputvar(index,name,type) if(iswrite(index))\
   fwrite(&output->name,sizeof(type),1,file)

#define writeoutputarray(index,name,n,type) if(iswrite(index))\
   fwrite(output->name,sizeof(type),n,file)

#define writeoutputarrayitem(index,name,item,n,type) if(iswrite(index))\
  {\
    for(i=0;i<n;i++)\
     fwrite(&output->name[i].item,sizeof(type),1,file);\
  }

void fwriteoutputdata(FILE *file,           /**< pointer to restart file */
                      const Output *output, /**< output data */
                      const Config *config  /**< LPJ configuration */
                     )
{
  fwrite(output->data,sizeof(Real),config->totalsize,file);
} /* of 'fwriteoutputdata' */
