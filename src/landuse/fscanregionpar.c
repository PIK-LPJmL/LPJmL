/**************************************************************************************/
/**                                                                                \n**/
/**          f  s  c  a  n  r  e  g  i  o  n  p  a  r  .  c                        \n**/
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

#define fscanreal2(verb,file,var,name,region)\
  if(fscanreal(file,var,name,FALSE,verb))\
  {\
    if(verb)\
      fprintf(stderr,"ERROR102: Cannot read float '%s' for region '%s'.\n",name,region);\
    return 0;\
  }

#define fscanreal012(verb,file,var,name,region)\
  if(fscanreal01(file,var,name,FALSE,verb))\
  {\
    if(verb)\
      fprintf(stderr,"ERROR102: Cannot read float '%s' for region '%s'.\n",name,region);\
    return 0;\
  }

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return 0;}

int fscanregionpar(LPJfile *file,         /**< pointer to LPJ file */
                   Regionpar **regionpar, /**< Pointer to regionpar array */
                   Verbosity verb         /**< verbosity level (NO_ERR,ERR,VERB) */
                  )                       /** \return number of elements in array */
{
  LPJfile *arr,*item;
  int nregions,n,id;
  const char *s;
  Regionpar *region;
  if (verb>=VERB) puts("// Region parameters");
  arr=fscanarray(file,&nregions,"regionpar",verb);
  if(arr==NULL)
    return 0;

  *regionpar=newvec(Regionpar,nregions);
  checkptr(*regionpar);
  for(n=0;n<nregions;n++)
    (*regionpar)[n].id=UNDEF;
  for(n=0;n<nregions;n++)
  {
    item=fscanarrayindex(arr,n);
    if(fscanint(item,&id,"id",FALSE,verb))
      return 0;
    if(id<0 || id>=nregions)
    {
      if(verb)
        fprintf(stderr,
                "ERROR126: Invalid range of region number=%d in fscanregionpar(), must be in [0,%d].\n",
                id,nregions-1);
      return 0;
    }
    region=(*regionpar)+id;
    if(region->id!=UNDEF)
    {
      if(verb)
        fprintf(stderr,
                "ERROR179: Region number=%d has been already defined in fscanregionpar().\n",id);
      return 0;
    }
    s=fscanstring(item,NULL,"name",verb);
    if(s==NULL)
    {
      if(verb)
        readstringerr("name");
      return 0;
    }
    region->name=strdup(s);
    checkptr(region->name);
    region->id=id;
    fscanreal012(verb,item,&region->fuelratio,"fuelratio",region->name);
    fscanreal012(verb,item,&region->bifratio,"bifratio",region->name);
  } /* of 'for(n=0;...)' */
  return n;
} /* of 'fscanregionpar' */
