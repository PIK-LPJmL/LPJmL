/**************************************************************************************/
/**                                                                                \n**/
/**             b  s  t  r  u  c  t  _  r  e  a  d  d  a  t  a  .  c               \n**/
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

Bool bstruct_readdata(Bstruct bstr,      /**< pointer to restart file */
                      Bstruct_data *data /**< data read from file */
                     )                   /**  \return TRUE on error */
{
  /* Function reads one data object */
  Byte len,id1;
  int string_length,i;
  short id,*id2;
  data->name=NULL;
  if(fread(&data->token,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading token.\n");
    return TRUE;
  }
  if(data->token==BSTRUCT_END || data->token==BSTRUCT_ENDARRAY || data->token==BSTRUCT_ENDSTRUCT)
    return FALSE;
  if(data->token!=BSTRUCT_INDEXARRAY)
  {
    if((data->token & 128)==128) /* top bit in token set, object name stored in next byte or short */
    {
      if((data->token & 64)==64) /* bit 7 set, id is of type short */
      {
        if(freadshort(&id,1,bstr->swap,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name for %s.\n",
                    bstruct_typenames[data->token & 63]);
          return TRUE;
        }
      }
      else
      {
        if(fread(&id1,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name for %s.\n",
                    bstruct_typenames[data->token & 63]);
          return TRUE;
        }
        id=id1;
      }
      for(i=0;i<bstr->count;i++)
      {
        id2=bstr->names[i].data;
        if(id==*id2)
        {
          data->name=bstr->names[i].key;
          break;
        }
      }
    }
  }
  data->token &= 63; /* strip top 2 bits in token */
  switch(data->token)
  {
    case BSTRUCT_FALSE:
      data->data.b=FALSE;
      return FALSE;
    case BSTRUCT_TRUE:
       data->data.b=TRUE;
      return FALSE;
    case BSTRUCT_BYTE:
      if(fread(&data->data.b,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_USHORT:
      if(freadushort(&data->data.us,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_SHORT:
      if(freadshort(&data->data.s,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_INT:
      if(freadint(&data->data.i,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_FLOAT:
      if(freadfloat(&data->data.f,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_DOUBLE:
      if(freaddouble(&data->data.d,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_STRING: case BSTRUCT_STRING1:
      if(data->token==BSTRUCT_STRING1)
      {
        if(fread(&len,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading string length of '%s'.\n",
                    getname(data->name));
          return TRUE;
        }
        string_length=len;
      }
      else
      {
        if(freadint(&string_length,1,bstr->swap,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading string length of '%s'.\n",
                    getname(data->name));
          return TRUE;
        }
      }
      data->data.string=malloc(string_length+1);
      if(data->data.string==NULL)
      {
        printallocerr("string");
        return TRUE;
      }
      if(fread(data->data.string,1,string_length,bstr->file)!=string_length)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading string '%s' of size %d.\n",
                  getname(data->name),string_length);
        free(data->data.string);
        data->data.string=NULL;
        return TRUE;
      }
      data->data.string[string_length]='\0';
      return FALSE;
    case BSTRUCT_BEGINARRAY1:
      if(fread(&len,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading array size of '%s'.\n",
                  getname(data->name));
        return TRUE;
      }
      data->size=len;
      return FALSE;
    case BSTRUCT_BEGINARRAY:
      if(freadint(&data->size,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading array size of '%s'.\n",
                  getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_INDEXARRAY:
      if(freadint(&data->size,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading size of index array.\n");
        return TRUE;
      }
      data->data.index=newvec(long long,data->size);
      if(data->data.index==NULL)
      {
        printallocerr("index");
        return TRUE;
      }
      if(freadlong(data->data.index,data->size,bstr->swap,bstr->file)!=data->size)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading index array of size %d.\n",
                  data->size);
        free(data->data.index);
        data->data.index=NULL;
        return TRUE;
      }
      return FALSE;
    default:
      return FALSE;
  } /* of  switch(data->token) */
  return FALSE;
} /* of 'bstruct_readdata' */
