/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  s  k  i  p  d  a  t  a  .  c         \n**/
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

Bool bstruct_skipdata(Bstruct bstr, /**< pointer to restart file */
                      Byte token    /**< token */
                     )              /** \return TRUE on error */
{
  /* Function skips one object in restart file */
  int string_len;
  Byte len,b;
  if(isinvalidtoken(token))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR502: Invalid token %d skipping data.\n",token);
    return TRUE;
  }
  switch(token & 63) /* strip top 2 bits */
  {
    case BSTRUCT_BEGINSTRUCT: /* object is a struct */
      do
      {
        /* skip whole struct */
        if(fread(&b,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading token in struct.\n");
          return TRUE;
        }
        if(isinvalidtoken(b))
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR502: Invalid token %d skipping data.\n",b);
          return TRUE;
        }
#ifdef DEBUB_BSTRUCT
        printf("type %s\n",bstruct_typenames[b & 63]);
#endif
        if(b==BSTRUCT_END)
        { 
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end token found skipping struct.\n");
          return TRUE;
        }
        if(b==BSTRUCT_ENDARRAY)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of array token found in struct.\n");
          return TRUE;
        }
        if(b!=BSTRUCT_ENDSTRUCT)
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
          /* call bstruct_skipdata() recursively */
          if(bstruct_skipdata(bstr,b))
            return TRUE;
        }
      } while(b!=BSTRUCT_ENDSTRUCT);
      break;
    case BSTRUCT_BEGINARRAY: case BSTRUCT_BEGINARRAY1: /* object is an array */
      /* skip array size */
      if(fseek(bstr->file,((token & 63)==BSTRUCT_BEGINARRAY1) ? 1 : sizeof(int),SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping array length.\n");
        return TRUE;
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
        if(isinvalidtoken(b))
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
          /* call bstruct_skipdata() recursively */
          if(bstruct_skipdata(bstr,b))
            return TRUE;
        }
      } while(b!=BSTRUCT_ENDARRAY);
      break;
    case BSTRUCT_STRING:
      if(freadint(&string_len,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading string length.\n");
        return TRUE;
      }
      if(fseek(bstr->file,string_len,SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping string of length %d.\n",
                  string_len);
        return TRUE;
      }
      break;
    case BSTRUCT_STRING1:
      if(fread(&len,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading string length.\n");
        return TRUE;
      }
      if(fseek(bstr->file,len,SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping string of length %d.\n",
                  len);
        return TRUE;
      }
      break;
    default:
      /* skip object data */
      if(fseek(bstr->file,bstruct_typesizes[token & 63],SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping %s.\n",
                  bstruct_typenames[token & 63]);
        return TRUE;
      }
  } /* of switch(token) */
  return FALSE;
} /* of 'bstruct_skipdata' */
