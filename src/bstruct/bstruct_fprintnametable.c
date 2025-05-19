/**************************************************************************************/
/**                                                                                \n**/
/**   b  s  t  r  u  c  t  _  f  p  r  i  n  t  n  a  m  e  t  a  b  l  e  .  c    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions for reading/writing JSON-like objects from binary file           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "bstruct_intern.h"

void bstruct_fprintnametable(FILE *file,         /**< pointer to open text file */
                             const char *name,   /**< name of JSON array */
                             const Bstruct bstr, /**< pointer to restart file */
                             Bool isjson         /**< output is in JSON format */
                            )
{
  /* Function prints name table in YAML/JSON format */
  int i;
  if(isjson)
  {
    fprintf(file,"\"%s\" :\n[\n",name);
    for(i=0;i<bstr->count;i++)
    {
      fprintf(file,"  \"%s\"",(char *)bstr->names[i].key);
      if(i<bstr->count-1)
        fputs(",\n",file);
      else
        fputc('\n',file);
    }
    fputc(']',file);
  }
  else
  {
    /* output in YAML format */
    fprintf(file,"%s:\n",name);
    for(i=0;i<bstr->count;i++)
      fprintf(file,"  - \"%s\"\n",(char *)bstr->names[i].key);
  }
} /* of 'bstruct_fprintnametable' */
