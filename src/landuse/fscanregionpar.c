/**************************************************************************************/
/**                                                                                \n**/
/**          f  s  c  a  n  r  e  g  i  o  n  p  a  r  .  c                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define UNDEF (-1)

#define fscanreal2(verb,file,var,name,region)\
  if(fscanreal(file,var,name,verb))\
  {\
    if(verb)\
      fprintf(stderr,"ERROR102: Cannot read region '%s' in %s()\n",region,__FUNCTION__);\
    return 0;\
  }

int fscanregionpar(FILE *file,            /**< file pointer */
                   Regionpar **regionpar, /**< Pointer to regionpar array */
                   Verbosity verb         /**< verbosity level (NO_ERR,ERR,VERB) */
                  )                       /** \return number of elements in array */
{
  int nregions,n,id;
  String s;
  Regionpar *region;
  if (verb>=VERB) puts("// Region parameters");
  if(fscanint(file,&nregions,"nregions",verb))
    return 0;

  *regionpar=newvec(Regionpar,nregions);
  check(*regionpar);
  for(n=0;n<nregions;n++)
    (*regionpar)[n].id=UNDEF;
  for(n=0;n<nregions;n++)
  {
    if(fscanint(file,&id,"region number",verb))
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
    if(fscanstring(file,s,verb!=NO_ERR))
    {
      if(verb)
        readstringerr("name");
      return 0;
    }
    if(verb>=VERB)
      printf("REGION_NAME %s\n",s);
    region->name=strdup(s);
    check(region->name);
    region->id=id;
    fscanreal2(verb,file,&region->fuelratio,"fuelratio",region->name);
    fscanreal2(verb,file,&region->bifratio,"bifratio",region->name);
    fscanreal2(verb,file,&region->woodconsum,"woodconsum",region->name);

  } /* of 'for(n=0;...)' */
  return n;
} /* of 'fscanregionpar' */
