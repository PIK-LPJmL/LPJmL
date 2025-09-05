/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  a  n  f  i  r  e  d  u  r  a  t  i  o  n  .  c             \n**/
/**                                                                                \n**/
/**     Function reads stand->specific maximum fire duration from config file      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static int findstandname(const char *name,Standtype **standtypes,int nstand)
{
  int i;
  for(i=0;i<nstand;i++)
    if(!strcmp(name,standtypes[i]->name))
      return i;
  return NOT_FOUND;
} /* of 'findstandname' */

Bool fscanfireduration(LPJfile *file,          /**< pointer to LPJ file */
                       Standtype **standtypes, /**< stand type array */
                       int nstand,             /**< number of stand types */
                       Verbosity verbose       /**< verbosity level (NO_ERR,ERR,VERB) */
                      )                        /** \return TRUE on error */
{
  const char* name;
  LPJfile *array,*item,*s;
  int i,size,index;
  for(i=0;i<nstand;i++)
  {
    standtypes[i]->fireduration[0]=param.fireduration[0];
    standtypes[i]->fireduration[1]=param.fireduration[1];
    standtypes[i]->max_ndayfire=param.max_ndayfire;
  }
  if(iskeydefined(file,"fireduration"))
  {
    array=fscanarray(file,&size,"fireduration",verbose);
    if(array==NULL)
      return TRUE;
    for(i=0;i<size;i++)
    {
      item=fscanarrayindex(array,i);
      s=fscanstruct(item,NULL,verbose);
      if(s==NULL)
        return TRUE;
      name=fscanstring(s,NULL,"stand",verbose);
      if(name==NULL)
        return TRUE;
      index=findstandname(name,standtypes,nstand);
      if(index==NOT_FOUND)
      {
        if(verbose)
        {
          fprintf(stderr,"ERROR245: Stand type '%s' not defined, must be in [",name);
          for(i=0;i<nstand;i++)
          {
            fprintf(stderr,"\"%s\"",standtypes[i]->name);
            if(i<nstand-1)
              fprintf(stderr,",");
          }
          fprintf(stderr,"].\n");
        }
        return TRUE;
      }
      if(fscanrealarray(s,standtypes[index]->fireduration,2,"duration",verbose))
        return TRUE;
      if(fscanint(s,&standtypes[index]->max_ndayfire,"ndayfire",FALSE,verbose))
        return TRUE;
      standtypes[index]->dailyfire=dailyfire;
    }
  }
  return FALSE;
} /* of 'fscanfireduration */

Bool fscanfirestand(LPJfile *file,          /**< pointer to LPJ file */
                    Standtype **standtypes, /**< stand type array */
                    int nstand,             /**< number of stand types */
                    Verbosity verbose
                   )                        /** \return TRUE on error */
{
  const char*name;
  LPJfile *array,*item;
  int i,size,index;
  if(iskeydefined(file,"firestand"))
  {
    array=fscanarray(file,&size,"firestand",verbose);
    if(array==NULL)
      return TRUE;
    for(i=0;i<size;i++)
    {
      item=fscanarrayindex(array,i);
      name=fscanstring(item,NULL,NULL,verbose);
      if(name==NULL)
        return TRUE;
      index=findstandname(name,standtypes,nstand);
      if(index==NOT_FOUND)
      {
        if(verbose)
        {
          fprintf(stderr,"ERROR245: Stand type '%s' not defined, must be in [",name);
          for(i=0;i<nstand;i++)
          {
            fprintf(stderr,"\"%s\"",standtypes[i]->name);
            if(i<nstand-1)
              fprintf(stderr,",");
          }
          fprintf(stderr,"].\n");
        }
        return TRUE;
      }
      standtypes[index]->dailyfire=dailyfire;
    }
  }
  return FALSE;
} /* of 'fscanfirestand */
