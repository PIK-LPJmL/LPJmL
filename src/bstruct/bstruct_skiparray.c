/**************************************************************************************/
/**                                                                                \n**/
/**                b  s  t  r  u  c  t  _  s  k  i  p  a  r  r  a  y  .  c         \n**/
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

Bool bstruct_skiparray(Bstruct bstr /**< pointer to restart file */
                      )              /** \return TRUE on error */
{
  /* Function skips an array object in restart file */
  int string_len;
  Byte b;
  if(bstr->level>0)
  {
    free(bstr->namestack[bstr->level-1].name);
    bstr->level--;
  }
  /* skip whole array */
  do
  {
    if(fread(&b,1,1,bstr->file)!=1)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR508: Unexpected end of file reading token in array.\n");
      return TRUE;
    }
    if(bstruct_isinvalidtoken(b))
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR502: Invalid token %d skipping data.\n",b);
      return TRUE;
    }
    if(b==BSTRUCT_END)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR508: Unexpected end token found skipping array.\n");
      return TRUE;
    }
    if(b==BSTRUCT_ENDSTRUCT)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR508: Unexpected end of struct token in array found.\n");
      return TRUE;
    }
    if(b==BSTRUCT_INDEXARRAY)
    {
      if(freadint(&string_len,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading index array length.\n");
        return TRUE;
      }
      if(fseek(bstr->file,sizeof(long long)*string_len,SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping index array of size %d.\n",
                  string_len);
        return TRUE;
      }
    }
    else if(b!=BSTRUCT_ENDARRAY)
    {
      /* skip object name */
      if((b & 128)==128) /* top bit in token set, object name stored in next byte or short */
      {
        if(fseek(bstr->file,((b & 64)==64) ? sizeof(short) : 1,SEEK_CUR))
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR507: Unexpected end of file skipping object name.\n");
          return TRUE;
        }
      }
      /* call bstruct_skipdata() */
      if(bstruct_skipdata(bstr,b))
        return TRUE;
    }
  } while(b!=BSTRUCT_ENDARRAY);
  return FALSE;
} /* of 'bstruct_skiparray' */
