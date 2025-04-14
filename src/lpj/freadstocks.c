/**************************************************************************************/
/**                                                                                \n**/
/**                  f  r  e  a  d  s  t  o  c  k  s  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads stock data from binary file                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/
#include "lpj.h"

Bool freadstocks(Bstruct file,     /**< pointer to restart file */
                 const char *name, /**< name of object or NULL */
                 Stocks *stocks    /**< data read from file */
                )                  /** \return TRUE on error */
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(bstruct_readreal(file,"C",&stocks->carbon))
    return TRUE;
  if(bstruct_readreal(file,"N",&stocks->nitrogen))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of 'freadstocks' */

Bool freadstocksarray(Bstruct file,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Stocks data[],    /**< array read from file */
                      int size          /**< size of array */
                     )                  /** \return TRUE on error */
{
  int i,n;
  if(bstruct_readarray(file,name,&n))
    return TRUE;
  if(size!=n)
  {
    fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(freadstocks(file,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(file,name);
} /* of 'freadstocksarray' */

