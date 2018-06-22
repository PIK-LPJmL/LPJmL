/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  s  i  z  e  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a size_t value from a text file                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

Bool fscansize(FILE *file,       /**< pointer to text file */
               size_t *value,    /**< value to be read from file */
               const char *name, /**< variable name */
               Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
              )                  /** \return TRUE on error */
{
  String line,token;
  char *ptr;
  Bool rc;
  rc=fscantoken(file,token);
  if(!rc)
  {
    *value=strtoul(token,&ptr,10);
    rc=*ptr!='\0';
  }
  if(rc && verb)
  {
    fprintf(stderr,"ERROR101: Cannot read size '%s' in line %d of '%s', ",
            name,getlinecount(),getfilename());
    if(strlen(token)>0)
    {
      fputs("read:\n",stderr);

      if(fgets(line,STRING_LEN,file)!=NULL)
        line[strlen(line)-1]='\0';
      else
      line[0]='\0';
      fprintf(stderr,"          '%s%s'\n           ",token,line);
      frepeatch(stderr,'^',strlen(token));
      fputc('\n',stderr);
    }
    else
      fputs("EOF reached.\n",stderr);
  }
  else if(verb>=VERB)
    printf("%s %llu",name,(unsigned long long)*value);
  return rc;
} /* of 'fscansize' */
