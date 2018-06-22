/**************************************************************************************/
/**                                                                                \n**/
/**                 g e t p r o d u c t p o o l s . c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens file with initial product pool data                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef IMAGE

Bool getproductpools(Productinit *productinit,
                     Product productpools[],
                     int ncell
                    )
{
  Real *vec;
  int cell;

  if(fseek(productinit->file,productinit->offset,SEEK_SET))
  {
    fprintf(stderr,"ERROR150: Cannot seek file to position %d in getproductpools().\n",
            productinit->offset);
    return TRUE;
  } 
  vec=newvec(Real,ncell*2);
  if(vec==NULL)
  {
    printallocerr("vec");
    return TRUE;
  }
  if(readrealvec(productinit->file,vec,0,productinit->scalar,ncell*2,productinit->swap,productinit->datatype))
  {
    fprintf(stderr,"ERROR151: Cannot read initial product pools.\n");
    free(vec);
    return TRUE;
  } 
  for(cell=0;cell<ncell;cell++)
  {
    productpools[cell].fast=vec[2*cell];
    productpools[cell].slow=vec[2*cell+1];
  }
  free(vec);
  return FALSE ;
} /* of 'getproductpools' */

#endif
