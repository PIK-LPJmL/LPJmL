/**************************************************************************************/
/**                                                                                \n**/
/**      b  s  t  r  u  c  t  _  s  e  e  k  i  n  d  e  x  a  r  r  a  y  .  c    \n**/
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

Bool bstruct_seekindexarray(Bstruct bstr, /**< pointer to restart file */
                            int index,    /**< index of array to seek */
                            int size      /**< [out] size of array */
                           )              /** \return TRUE on error */
{
  long long pos;
  int n;
  Byte token;
  if(bstruct_readtoken(bstr,&token,BSTRUCT_INDEXARRAY,NULL))
    return TRUE;
  if(token!=BSTRUCT_INDEXARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of %s is not an indexarray.\n",
              bstruct_typenames[token & 63]);
    return TRUE;
  }
  if(freadint(&n,1,bstr->swap,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR512: Cannot read length of index array.\n");
    return TRUE;
  }
  if(n!=size)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR510: Size of index array=%d not %d.\n",n,size);
    return TRUE;
  }
  if(fseek(bstr->file,sizeof(long long)*index,SEEK_CUR))
  {
    fprintf(stderr,"ERROR511: Cannot skip to %d in index array.\n",index);
    return TRUE;
  }
  if(freadlong(&pos,1,bstr->swap,bstr->file)!=1)
  {
    fprintf(stderr,"ERROR512: Cannot read file index for %d.\n",index);
    return TRUE;
  }
  if(fseek(bstr->file,pos,SEEK_SET))
  {
    fprintf(stderr,"ERROR511: Cannot skip to file position %lld.\n",pos);
    return TRUE;
  }
  bstr->namestack[bstr->level-1].nr=index;
  return FALSE;
} /* of 'bstruct_seekindexarray' */
