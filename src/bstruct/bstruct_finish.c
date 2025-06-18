/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  f  i  n  i  s  h  .  c               \n**/
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

void bstruct_finish(Bstruct bstruct)
{
  long long filepos;
  int i,count;
  Hashitem *list;
  Var *var;
  Byte len,token;
  short *id;
  /* Function writes name table, closes restart file and frees memory */
  if(bstruct!=NULL)
  {
    if(bstruct->hash!=NULL)
    {
      /* write end token */
      token=BSTRUCT_END;
      fwrite(&token,sizeof(token),1,bstruct->file);
      /* save position of start of name table */
      filepos=ftell(bstruct->file);
      /* convert hash table into array */
      list=hash2array(bstruct->hash);
      if(list==NULL)
      {
        printallocerr("list");
        return;
      }
      count=gethashcount(bstruct->hash);
      /* sort array by names. This is done to use binary search for
         finding the id of object names */
      qsort(list,count,sizeof(Hashitem),bstruct_cmpname);
      /* write name table at end of restart file */
      /* write size of name table */
      fwrite(&count,sizeof(int),1,bstruct->file);
      for(i=0;i<count;i++)
      {
        len=strlen(list[i].key);
        /* write name length */
        fwrite(&len,1,1,bstruct->file);
        /* write name */
        fwrite(list[i].key,len,1,bstruct->file);
        /* write corresponding id */
        id=list[i].data;
        fwrite(id,sizeof(short),1,bstruct->file);
#ifdef DEBUG_BSTRUCT
        printf("%d: '%s'\n",*id,(char *)list[i].key);
#endif
      }
      free(list);
      /* write file position of name table after file header */
      fseek(bstruct->file,strlen(BSTRUCT_HEADER)+sizeof(int),SEEK_SET);
      fwrite(&filepos,sizeof(filepos),1,bstruct->file);
      freehash(bstruct->hash);
      if(bstruct->level>1)
      {
        fprintf(stderr,"ERROR523: %s not closed.\n",
                bstruct->namestack[bstruct->level-1].type==BSTRUCT_BEGINSTRUCT ? "Struct" : "Array");
        bstruct_printnamestack(bstruct);
      }
    }
    else if(bstruct->namestack[bstruct->level-1].varnames!=NULL)
    {
      foreachlistitem(i,bstruct->namestack[bstruct->level-1].varnames)
      {
        var=getlistitem(bstruct->namestack[bstruct->level-1].varnames,i);
        if(!var->isread)
        {
          bstruct->skipped++;
          if(bstruct->isout && bstruct->print_noread)
            fprintf(stderr,"REMARK502: Object '%s' in struct '%s' not read.\n",
                    bstruct->names2[var->id].key,getname(bstruct->namestack[bstruct->level-1].name));
        }
      }
    }
    bstruct_freenamestack(bstruct);
    fclose(bstruct->file);
    /* free name table */
    for(i=0;i<bstruct->count;i++)
    {
      free(bstruct->names[i].key);
      free(bstruct->names[i].data);
    }
    free(bstruct->names);
    free(bstruct->names2);
    free(bstruct);
  }
} /* of 'bstruct_finish' */
