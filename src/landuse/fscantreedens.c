/**************************************************************************************/
/**                                                                                \n**/
/**          f  s  c  a  n  t  r  e  e  d  e  n  s  .  c                           \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define UNDEF (-1)
#define fscanreal2(verb,file,var,name,country)\
  if(fscanreal(file,var,name,FALSE,verb))\
  {\
    if(verb)\
      fprintf(stderr,"ERROR102: Cannot read float '%s' for country '%s' \n",name,country);\
    return 0;\
  }

int fscantreedens(LPJfile *file,          /**< pointer to LPJ file */
                  Countrypar *countrypar, /**< Pointer to countrypar array */
                  int ncountries,         /**< number of countries */
                  int nagtree,            /**< number of agriculture tree PFTs */
                  Verbosity verb          /**< output on stderr (TRUE/FALSE)*/
                 )                        /** \return number of elements in array */
{
  LPJfile arr,item;
  int n,id,size,i;
  Countrypar *country;
  
  if (verb>=VERB) puts("// Tree densities");
  if(fscanarray(file,&arr,&size,FALSE,"treedens",verb))
    return 0;
  if(size!=ncountries)
  {
    if(verb)
      fprintf(stderr,"WARNING029: Size of tree density array=%d is not equal the number of countries=%d.\n",size,ncountries);
  }
   
  for(n=0;n<size;n++)
  {
    fscanarrayindex(&arr,&item,n,verb);
    if(fscanint(&item,&id,"id",FALSE,verb))
      return 0;
    if(id<0 || id>=ncountries)
    {
      if(verb)
        fprintf(stderr,"ERROR125: Invalid range=%d of 'id'.\n",id);
      return 0;
    }
    country=countrypar+id;
    if(country->k_est!=NULL)
    {
      if(verb)
        fprintf(stderr,
                "ERROR178: tree density number=%d has been already defined.\n",id);
      return 0;
    }

    country->k_est=newvec(Real,nagtree);
    if(country->k_est==NULL)
    {
      printallocerr("k_est");
      return 0;
    }
    if(fscanrealarray(&item,country->k_est,nagtree,"k_est",verb))
    {
      if(verb)
        fprintf(stderr,"ERROR102: cannot read 'k_est' vector for '%s'.\n",country->name);
      return 0;
    }
    /*printf("country tree density in %s: %f\n",country->name,country->k_est[nagtree-1]);*/
  } /* of 'for(n=0;...)' */
  for(n=0;n<ncountries;n++)
    if(countrypar[n].k_est==NULL)
    {
      if(verb)
        fprintf(stderr,"WARNING030: 'k_est' vector for country '%s' not defined, default values assumed.\n",countrypar[n].name);
      countrypar[n].k_est=newvec(Real,nagtree);
      if(countrypar[n].k_est==NULL)
      {
        printallocerr("k_est");
        return 0;
      }
      for(i=0;i<nagtree;i++)
        countrypar[n].k_est[i]=-1;         
    }
  return n;
} /* of 'fscantreedens' */
