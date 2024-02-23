/**************************************************************************************/
/**                                                                                \n**/
/**     f s c a n c o u n t r y p a r . c                                          \n**/
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

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return 0;}

const char *irrigsys[]={"no irrig","surf","sprink","drip"};

int fscancountrypar(LPJfile *file,           /**< pointer to LPJ file */
                    Countrypar **countrypar, /**< Pointer to countrypar array */
                    int npft,                /**< number of natural PFTs */
                    int ncft,                /**< number of CFTs or zero */
                    Verbosity verb,          /**< verbosity level (NO_ERR,ERR,VERB) */
                    const Config *config     /**< LPJmL configuration */
                   )                         /** \return number of elements in array */
{
  LPJfile *arr,*item;
  int i,ncountries,n,id,*cftmap,cftmap_size;
  Real *laimax_cft;
  const char *s;
  Countrypar *country;
  ncountries = 1;
  if (verb>=VERB) puts("// Country parameters");
  if(ncft)
  {
    cftmap=scancftmap(file,&cftmap_size,"laimaxmap",TRUE,npft,ncft,config);
    if(cftmap==NULL)
      return 0;
    if(cftmap_size!=ncft)
    {
      if(verb)
        fprintf(stderr,"ERROR243: Size of map 'laimaxmap'=%d does not match the number of crops %d.\n",
                cftmap_size,ncft);
      free(cftmap);
      return 0;
    }
  }
  arr=fscanarray(file,&ncountries,"countrypar",verb);
  if(arr==NULL)
  {
    if(ncft)
      free(cftmap);
    return 0;
  }
  *countrypar=newvec(Countrypar,ncountries);
  checkptr(*countrypar);
  for(n=0;n<ncountries;n++)
   (*countrypar)[n].id=UNDEF;
  for(n=0;n<ncountries;n++)
  {
    item=fscanarrayindex(arr,n);
    if(fscanint(item,&id,"id",FALSE,verb))
    {
      if(ncft)
        free(cftmap);
      return 0;
    }
    if(id<0 || id>=ncountries)
    {
      if(verb)
        fprintf(stderr,"ERROR125: Invalid range=%d of 'countrynumber', must be in [0,%d].\n",
                id,ncountries-1);
      if(ncft)
        free(cftmap);
      return 0;
    }
    country=(*countrypar)+id;
    if(country->id!=UNDEF)
    {
      if(verb)
        fprintf(stderr,
                "ERROR178: Country number=%d has been already defined.\n",id);
      if(ncft)
        free(cftmap);
      return 0;
    }
    s=fscanstring(item,NULL,"name",verb);  /*reads country name*/
    if(s==NULL)
    {
      if(verb)
        readstringerr("name");
      if(ncft)
        free(cftmap);
      return 0;
    }
    country->name=strdup(s);
    checkptr(country->name);
    country->id=id;
    country->k_est=NULL;
    if(ncft)
    {
      country->laimax_cft=newvec(Real,ncft);
      checkptr(country->laimax_cft);
      laimax_cft=newvec(Real,ncft);
      checkptr(laimax_cft);
      if(fscanrealarray(item,laimax_cft,ncft,"laimax",verb))
      {
        if(verb)
          fprintf(stderr,"ERROR102: Cannot read array 'laimax' for country '%s'.\n",country->name);  
        free(cftmap);
        return 0;
      } 
      /*in case rw_manage: increases laimax by bridge gap factor*/
      if(config->rw_manage)
        for(i=0;i<ncft;i++)
          laimax_cft[i]+=(7-laimax_cft[i])*param.yield_gap_bridge;
      for(i=0;i<ncft;i++)
        country->laimax_cft[cftmap[i]]=laimax_cft[i];
      free(laimax_cft);
    }  
    else
    {
      country->laimax_cft=NULL;
    }
    if(fscankeywords(item,(int *)&country->default_irrig_system,"default_irrig_system",irrigsys,4,FALSE,verb))
    {
      if(verb)
        fprintf(stderr,"ERROR215: Invalid value for irrigation system for country '%s'.\n",country->name);
      return 0;
    }
  } /* of 'for(n=0;...)' */
  if(ncft)
    free(cftmap);
  return n;
} /* of 'fscancountrypar' */
