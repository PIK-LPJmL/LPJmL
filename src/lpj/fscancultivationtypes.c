/**************************************************************************************/
/**                                                                                \n**/
/**     f  s  c  a  n  c  u  l  t  i  v  a  t  i  o  n  s  t  y  p  e  s  .  c     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads cultivation type array from configuration file             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscancultivationtypes(LPJfile *lpjfile, /**< pointer to LPJ file */
                           int **cult_types, /**< cultivation type array */
                           int *ncult_types, /**< size of array */
                           Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
                          )                  /** \return TRUE on error */
{
  LPJfile arr,item;
  int i;
  if(fscanarray(lpjfile,&arr,ncult_types,TRUE,"cultivation_types",verb))
    return TRUE;
  *cult_types=newvec(int,*ncult_types);
  if(*cult_types==NULL)
  {
    printallocerr("cult_types");
    return TRUE; 
  }
  for(i=0;i<*ncult_types;i++)
  {
    fscanarrayindex(&arr,&item,i,verb);
    if(fscankeywords(&item,(*cult_types)+i,NULL,cultivation_type,5,FALSE,verb))
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid value for cultivation type of item %d in 'cultivation_types'.\n",i+1);
      free(*cult_types);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'fscancultivationtypes' */
