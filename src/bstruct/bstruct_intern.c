/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  i  n  t  e  r  n  .  c               \n**/
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

const size_t bstruct_typesizes[]={1,sizeof(short),sizeof(int),sizeof(float),
                                  sizeof(double),0,0,sizeof(unsigned short),0,0};

char *bstruct_typenames[]={"byte","short","int","float","double","bool","bool","ushort",
                           "zero","fzero","string","string1","array","array1","struct","indexarray",
                           "endstruct","endarray","end"};

int bstruct_gethashkey(const char *key,int size)
{
  /* Function converts string into hash value in [0,size-1] */
  unsigned int hashvalue=0;
  while(*key!='\0')
    hashvalue=(hashvalue*256+(unsigned char)(*key++)) % size;
  return hashvalue;
} /* of 'hashkey' */

int bstruct_cmpname(const void *ptr1,const void *ptr2)
{
  /* Function compares two hash items by key, used for sorting and searching items */
  const Hashitem *h1=ptr1;
  const Hashitem *h2=ptr2;
  return strcmp(h1->key,h2->key);
} /* of 'bstruct_cmpname' */

int bstruct_cmpdata(const void *ptr1,const void *ptr2)
{
  /* Function compares two hash items by data , used for sorting and searching items */
  const Hashitem *h1=ptr1;
  const Hashitem *h2=ptr2;
  return *(const short *)(h1->data)-*(const short *)(h2->data);
} /* of 'bstruct_cmpdata' */

void bstruct_freenamestack(Bstruct bstruct)
{
  /* Function deallocates stack of already read objects */
  int i,j;
  for(i=0;i<bstruct->level;i++)
  {
    free(bstruct->namestack[i].name);
    if(bstruct->namestack[i].varnames!=NULL)
    {
      foreachlistitem(j,bstruct->namestack[i].varnames)
        free(getlistitem(bstruct->namestack[i].varnames,j));
      freelist(bstruct->namestack[i].varnames);
    }
  }
  bstruct->level=0;
} /* of 'bstruct_freenamestack' */

void bstruct_printnamestack(const Bstruct bstr)
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
} /* of 'bstruct_printnamestack' */
