/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  i  n  t  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads an int value from a text file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

Bool fscanint(FILE *file,       /**< pointer to text file */
              int *value,       /**< integer to be read from file */
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
    *value=(int)strtol(token,&ptr,10);
    rc=*ptr!='\0';
  }
  if(rc && verb)
  {
    fprintf(stderr,"ERROR101: Cannot read int '%s' in line %d of '%s', ",
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
  else if (verb >= VERB) printf("%s %d\n", name, *value);
  return rc;
} /* of 'fscanint' */
