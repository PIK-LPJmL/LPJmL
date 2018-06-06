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
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanint2(file,var,name) if(fscanint(file,var,name,verb)) return NULL;
#define fscanfloat2(file,var,name) if(fscanfloat(file,var,name,verb)){ return NULL;}
#define fscanname(file,var,name) {              \
    if(fscanstring(file,var,verb!=NO_ERR)) {                 \
      if(verb) readstringerr(name);  \
      return NULL;                              \
    }                                              \
    if(verb>=VERB)    \
      printf("%s %s\n", name, var);                     \
  }

Variable *fscanoutputvar(FILE *file,    /**< File pointer to text file */
                         int nout_max,  /**< maximum number of output files */
                         Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
                        )               /** \return TRUE on error */
{
  String name;
  Variable *outnames;
  int index,i;
  if (verb>=VERB) puts("// Output parameters");
  outnames=newvec(Variable,nout_max);
  if(outnames==NULL)
    return NULL;
  for(i=0;i<nout_max;i++)
    outnames[i].name=NULL; 
  for(i=0;i<nout_max;i++)
  {
    fscanint2(file,&index,"id");
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
    fscanname(file,name,"name");
    outnames[index].name=strdup(name);
    fscanname(file,name,"var");
    outnames[index].var=strdup(name);
    fscanname(file,name,"description");
    outnames[index].descr=strdup(name);
    fscanname(file,name,"unit");
    outnames[index].unit=strdup(name);
    fscanfloat2(file,&outnames[index].scale,"scale");
  }
  return outnames;
} /* of 'fscanoutputvar' */
