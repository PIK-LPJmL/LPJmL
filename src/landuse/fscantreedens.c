/**************************************************************************************/
/**                                                                                \n**/
/**          f  s  c  a  n  t  r  e  e  d  e  n  s  .  c                           \n**/
/**                                                                                \n**/
/** Function reads tree densities for agriculturual trees for each country         \n**/
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
      fprintf(stderr,"ERROR102: Cannot read float '%s' for country '%s'.\n",name,country);\
    return 0;\
  }

int fscantreedens(LPJfile *file,          /**< pointer to LPJ file */
                  Countrypar *countrypar, /**< Pointer to countrypar array */
                  int ncountries,         /**< number of countries */
                  int npft,               /**< number of ntural PFTs */
                  Verbosity verb,         /**< output on stderr (TRUE/FALSE)*/
                  const Config *config    /**< LPJmL configuration */
                 )                        /** \return number of elements in array or 0 in case of error */
{
  LPJfile *arr,*item;
  int n,id,size,i,*cftmap;
  Real *k_est;
  Countrypar *country;

  if (verb>=VERB) puts("// Tree densities");
  arr=fscanarray(file,&size,"treedens",verb);
  if(arr==NULL)
    return 0;
  if(size!=ncountries)
  {
    if(verb)
      fprintf(stderr,"WARNING029: Size of tree density array=%d is not equal the number of countries=%d.\n",
              size,ncountries);
  }
  cftmap=fscanagtreemap(file,"treemap",npft,config);
  if(cftmap==NULL)
    return 0;
  for(n=0;n<size;n++)
  {
    item=fscanarrayindex(arr,n);
    if(fscanint(item,&id,"id",FALSE,verb))
    {
      free(cftmap);
      return 0;
    }
    if(id<0 || id>=ncountries)
    {
      if(verb)
        fprintf(stderr,"ERROR125: Invalid range=%d of 'id', must be in [0,%d].\n",id,ncountries-1);
      free(cftmap);
      return 0;
    }
    country=countrypar+id;
    if(country->k_est!=NULL)
    {
      if(verb)
        fprintf(stderr,
                "ERROR178: Tree density number=%d has been already defined.\n",id);
      free(cftmap);
      return 0;
    }

    country->k_est=newvec(Real,config->nagtree);
    if(country->k_est==NULL)
    {
      printallocerr("k_est");
      free(cftmap);
      return 0;
    }
    k_est=newvec(Real,config->nagtree);
    if(k_est==NULL)
    {
      printallocerr("k_est");
      free(cftmap);
      return 0;
    }
    if(fscanrealarray(item,k_est,config->nagtree,"k_est",verb))
    {
      if(verb)
        fprintf(stderr,"ERROR102: Cannot read 'k_est' array for '%s'.\n",country->name);
      free(cftmap);
      return 0;
    }
    for(i=0;i<config->nagtree;i++)
      country->k_est[cftmap[i]]=k_est[i];
    free(k_est);
    /*printf("country tree density in %s: %f\n",country->name,country->k_est[nagtree-1]);*/
  } /* of 'for(n=0;...)' */
  for(n=0;n<ncountries;n++)
    if(countrypar[n].k_est==NULL)
    {
      if(verb)
        fprintf(stderr,"WARNING030: 'k_est' array for country '%s' not defined, default value of -1 assumed.\n",countrypar[n].name);
      countrypar[n].k_est=newvec(Real,config->nagtree);
      if(countrypar[n].k_est==NULL)
      {
        printallocerr("k_est");
        free(cftmap);
        return 0;
      }
      for(i=0;i<config->nagtree;i++)
        countrypar[n].k_est[i]=-1;
    }
  free(cftmap);
  return n;
} /* of 'fscantreedens' */
