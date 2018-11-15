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

int fscanregionpar(LPJfile *file,         /**< pointer to LPJ file */
                   Regionpar **regionpar, /**< Pointer to regionpar array */
                   Verbosity verb         /**< verbosity level (NO_ERR,ERR,VERB) */
                  )                       /** \return number of elements in array */
{
  LPJfile arr,item;
  int nregions,n,id;
  String s;
  Regionpar *region;
  if (verb>=VERB) puts("// Region parameters");
  if(fscanarray(file,&arr,&nregions,TRUE,"regionpar",verb))
    return 0;

  *regionpar=newvec(Regionpar,nregions);
  check(*regionpar);
  for(n=0;n<nregions;n++)
    (*regionpar)[n].id=UNDEF;
  for(n=0;n<nregions;n++)
  {
    fscanarrayindex(&arr,&item,n,verb);
    if(fscanint(&item,&id,"id",FALSE,verb))
      return 0;
    if(id<0 || id>=nregions)
    {
      if(verb)
        fprintf(stderr,
                "ERROR126: Invalid range of region number=%d in line %d of '%s' in fscanregionpar().\n",id,getlinecount(),getfilename());
      return 0;
    }
    region=(*regionpar)+id;
    if(region->id!=UNDEF)
    {
      if(verb)
        fprintf(stderr,
                "ERROR179: Region number=%d in line %d of '%s' has been already defined in fscanregionpar().\n",id,getlinecount(),getfilename());
      return 0;
    }
    if(fscanstring(&item,s,"name",FALSE,verb))
    {
      if(verb)
        readstringerr("name");
      return 0;
    }
    region->name=strdup(s);
    check(region->name);
    region->id=id;
    fscanreal2(verb,&item,&region->fuelratio,"fuelratio",region->name);
    fscanreal2(verb,&item,&region->bifratio,"bifratio",region->name);
    fscanreal2(verb,&item,&region->woodconsum,"woodconsum",region->name);

  } /* of 'for(n=0;...)' */
  return n;
} /* of 'fscanregionpar' */
