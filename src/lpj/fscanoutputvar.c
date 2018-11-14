/**************************************************************************************/
/**                                                                                \n**/
/**            f  s  c  a  n  o  u  t  p  u  t  v  a  r  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads name, description and units of output variables             \n**/
/**     from a configuration file                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanint2(file,var,name) if(fscanint(file,var,name,FALSE,verb)) return NULL;
#define fscanfloat2(file,var,name,out) if(fscanfloat(file,var,name,TRUE,verb)){ \
    if(verb) fprintf(stderr,"ERRROR229: Cannot read float '%s' for output '%s'.\n",name,out);\
    return NULL;}
#define fscanname(file,var,name,out) {              \
    if(fscanstring(file,var,name,FALSE,verb)) {                 \
    if(verb) fprintf(stderr,"ERRROR229: Cannot read string '%s' for output '%s'.\n",name,out);\
      return NULL;                              \
    }                                              \
  }

Variable *fscanoutputvar(LPJfile *file, /**< pointer to LPJ file */
                         int nout_max,  /**< maximum number of output files */
                         Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
                        )               /** \return TRUE on error */
{
  LPJfile arr,item;
  String name;
  Variable *outnames;
  int index,i,size;
  if (verb>=VERB) puts("// Output parameters");
  size=nout_max;
  if(fscanarray(file,&arr,&size,FALSE,"outputvar",verb))
    return NULL;
  if(size!=nout_max)
  {
    if(verb)
      fprintf(stderr,"ERROR232: Number of items=%d in 'outputvars' array does not match %d, check NOUT in 'include/conf.h'.\n",size,nout_max);
    return NULL;
  }
  outnames=newvec(Variable,nout_max);
  if(outnames==NULL)
    return NULL;
  for(i=0;i<nout_max;i++)
    outnames[i].name=NULL; 
  for(i=0;i<nout_max;i++)
  {
    fscanarrayindex(&arr,&item,i,verb);
    fscanint2(&item,&index,"id");
    if(index<0 || index>=nout_max)
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid index %d in line %d of '%s' for output description.\n",
               index,getlinecount(),getfilename());
      return NULL;
    }
    if(outnames[index].name!=NULL)
    {
      if(verb)
        fprintf(stderr,"ERROR202: Index %d in line %d of '%s' already used for output description.\n",index,getlinecount(),getfilename());
      return NULL;
    }
    fscanname(&item,name,"name",outnames[index].name);
    outnames[index].name=strdup(name);
    fscanname(&item,name,"var",outnames[index].name);
    outnames[index].var=strdup(name);
    fscanname(&item,name,"descr",outnames[index].name);
    outnames[index].descr=strdup(name);
    fscanname(&item,name,"unit",outnames[index].name);
    outnames[index].unit=strdup(name);
    outnames[index].scale=1.0;
    fscanfloat2(&item,&outnames[index].scale,"scale",outnames[index].name);
    outnames[index].offset=0.0;
    fscanfloat2(&item,&outnames[index].offset,"offset",outnames[index].name);
  }
  return outnames;
} /* of 'fscanoutputvar' */
