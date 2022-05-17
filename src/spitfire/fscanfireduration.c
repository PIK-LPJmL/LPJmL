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
                       Verbosity verbose
                      )                        /** \return TRUE on error */
{
  String name;
  LPJfile array,item,s;
  int i,size,index;
  for(i=0;i<nstand;i++)
    standtypes[i]->max_fireduration=param.max_fireduration;
  if(iskeydefined(file,"max_fireduration"))
  {
    if(fscanarray(file,&array,&size,FALSE,"max_fireduration",verbose))
      return TRUE;
    for(i=0;i<size;i++)
    {
      fscanarrayindex(&array,&item,i,verbose);
      if(fscanstruct(&item,&s,NULL,verbose))
        return TRUE;
      if(fscanstring(&s,name,"stand",FALSE,verbose)) 
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
      if(fscanreal(&s,&standtypes[index]->max_fireduration,"duration",FALSE,verbose)) 
        return TRUE;
    }
  }
  return FALSE;
} /* of 'fscanfireduration */
