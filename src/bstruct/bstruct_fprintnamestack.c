/**************************************************************************************/
/**                                                                                \n**/
/**    b  s  t  r  u  c  t  _  f  p  r  i  n  t  n  a  m  e  s  t  a  c  k  .  c   \n**/
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

void bstruct_fprintnamestack(FILE *file,        /**< pointer to text file */
                             const Bstruct bstr /**< pointer to restart file */
                            )
{
  Var *var;
  /* Function prints object stack */
  int i,j;
  fputs("=====001: Object stack:\n"
        "=====002:   Type   Name                 Pos. Objects\n",stderr);
  for(i=bstr->level-1;i>=0;i--)
  {
    fprintf(stderr,"=====%03d:   %-6s %-20s %4d ",bstr->level-i+2,
            (bstr->namestack[i].type==BSTRUCT_BEGINSTRUCT) ? "struct" : "array",
            getname(bstr->namestack[i].name),
            bstr->namestack[i].nr);
    if(bstr->namestack[i].varnames!=NULL)
    {
      foreachlistitem(j,bstr->namestack[i].varnames)
      {
        var=getlistitem(bstr->namestack[i].varnames,j);
        fputs(bstr->names2[var->id].key,stderr);
        if(j<getlistlen(bstr->namestack[i].varnames)-1)
          fputs(", ",stderr);
      }
    }
    fputc('\n',stderr);
  }
} /* of 'bstruct_fprintnamestack' */
