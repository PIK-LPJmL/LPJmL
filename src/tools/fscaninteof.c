/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  i  n  t  e  o  f  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads an int value from a text file                               \n**/
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

Bool fscaninteof(FILE *file,       /**< file pointer to text file */
                 int *value,       /**< int value read from file */
                 const char *name, /**< name of variable */
                 Bool *iseof,      /**< end of file reached (TRUE/FALSE) */
                 Bool isout        /**< enable error mesage (TRUE/FALSE) */
                )                  /** \return TRUE on error */
{
  String line,token;
  char *ptr;
  Bool rc;
  rc=fscantoken(file,token);
  if(rc)
  {
    *iseof=strlen(token)==0;
    return TRUE;
  }
  *iseof=FALSE;
  if(!rc)
  {
    *value=(int)strtol(token,&ptr,10);
    rc=*ptr!='\0';
  }
  if(rc && isout)
  {
    fprintf(stderr,"ERROR101: Cannot read int '%s' in line %d of '%s', read:\n",
            name,getlinecount(),getfilename());
    if(fgets(line,STRING_LEN,file)!=NULL)
      line[strlen(line)-1]='\0';
    else
      line[0]='\0';
    fputs("          '",stderr);
    fputprintable(stderr,token);
    fputprintable(stderr,line);
    fputs("'\n           ",stderr);
    frepeatch(stderr,'^',strlen(token));
    fputc('\n',stderr);
  }
  return rc;
} /* of 'fscaninteof' */
