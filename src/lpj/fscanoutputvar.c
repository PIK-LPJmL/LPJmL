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
    if(verb) fprintf(stderr,"ERRROR229: Cannot read string '%s' for output '%s'.\n",name,out==NULL ? "N/A" : out);\
      return NULL;                              \
    }                                              \
  }

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return NULL; }

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
      fprintf(stderr,"ERROR232: Number of items=%d in 'outputvar' array does not match %d, check NOUT in 'include/conf.h'.\n",size,nout_max);
  }
  outnames=newvec(Variable,nout_max);
  checkptr(outnames);
  for(i=0;i<nout_max;i++)
    outnames[i].name=NULL;
  for(i=0;i<size;i++)
  {
    fscanarrayindex(&arr,&item,i,verb);
    fscanint2(&item,&index,"id");
    if(fscanstring(&item,name,"name",FALSE,verb))
    {
      if(verb)
        fprintf(stderr,"ERROR233: No name defined for output index %d.\n",index);
      return NULL;
    }
    if(index<0 || index>=nout_max)
    {
      if(verb)
        fprintf(stderr,"ERROR201: Invalid index %d for description of output '%s'.\n",
               index,name);
      return NULL;
    }
    if(outnames[index].name!=NULL)
    {
      if(verb)
        fprintf(stderr,"ERROR202: Index %d already used for description of output '%s'.\n",
                index,name);
      return NULL;
    }
    outnames[index].name=strdup(name);
    checkptr(outnames[index].name);
    fscanname(&item,name,"var",outnames[index].name);
    outnames[index].var=strdup(name);
    checkptr(outnames[index].var);
    fscanname(&item,name,"descr",outnames[index].name);
    outnames[index].descr=strdup(name);
    checkptr(outnames[index].descr);
    fscanname(&item,name,"unit",outnames[index].name);
    if(strstr(name,"/month")!=NULL)
      outnames[index].time=MONTH;
    else if(strstr(name,"/yr")!=NULL)
      outnames[index].time=YEAR;
    else if(strstr(name,"/day")!=NULL || strstr(name,"d-1")!=NULL)
      outnames[index].time=DAY;
    else if(strstr(name,"/sec")!=NULL || strstr(name,"s-1")!=NULL)
      outnames[index].time=SECOND;
    else
      outnames[index].time=MISSING_TIME;
    outnames[index].unit=strdup(name);
    checkptr(outnames[index].unit);
    outnames[index].scale=1.0;
    fscanfloat2(&item,&outnames[index].scale,"scale",outnames[index].name);
    outnames[index].offset=0.0;
    fscanfloat2(&item,&outnames[index].offset,"offset",outnames[index].name);
    if(fscantimestep(&item,&outnames[index].timestep,verb))
    {
      if(verb) 
        fprintf(stderr,"ERRROR229: Cannot read int 'timestep' for output '%s'.\n",outnames[index].name);
      return NULL;
    }
  }
  for(i=0;i<nout_max;i++)
    if(outnames[i].name==NULL)
    {
      if(verb)
        fprintf(stderr,"ERRROR230: Output description not defined for index=%d in 'outputvar'\n",i);
      return NULL;
    }
  return outnames;
} /* of 'fscanoutputvar' */
