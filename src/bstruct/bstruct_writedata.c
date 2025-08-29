/**************************************************************************************/
/**                                                                                \n**/
/**             b  s  t  r  u  c  t  _  w  r  i  t  e  d  a  t  a  .  c            \n**/
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

Bool bstruct_writedata(Bstruct bstr,            /**< pointer to restart file */
                       const Bstruct_data *data /**< pointer to data */
                      )                         /** \return TRUE on error */
{
  switch(data->token)
  {
    case BSTRUCT_BEGINARRAY: case BSTRUCT_BEGINARRAY1:
      return bstruct_writebeginarray(bstr,data->name,data->size);
    case BSTRUCT_BEGINSTRUCT:
      return bstruct_writebeginstruct(bstr,data->name);
    case BSTRUCT_ENDSTRUCT:
      return bstruct_writeendstruct(bstr);
    case BSTRUCT_ENDARRAY:
      return bstruct_writeendarray(bstr);
    case BSTRUCT_STRING: case BSTRUCT_STRING1:
      return bstruct_writestring(bstr,data->name,data->data.string);
    case BSTRUCT_TRUE:
      return bstruct_writebool(bstr,data->name,TRUE);
    case BSTRUCT_FALSE:
      return bstruct_writebool(bstr,data->name,FALSE);
    case BSTRUCT_INT:
      return bstruct_writeint(bstr,data->name,data->data.i);
    case BSTRUCT_SHORT:
      return bstruct_writeshort(bstr,data->name,data->data.s);
    case BSTRUCT_USHORT:
      return bstruct_writeushort(bstr,data->name,data->data.us);
    case BSTRUCT_BYTE:
      return bstruct_writebyte(bstr,data->name,data->data.b);
    case BSTRUCT_FLOAT:
      return bstruct_writefloat(bstr,data->name,data->data.f);
    case BSTRUCT_DOUBLE:
      return bstruct_writedouble(bstr,data->name,data->data.d);
    case BSTRUCT_ZERO:
      return bstruct_writeint(bstr,data->name,0);
    case BSTRUCT_FZERO:
      return bstruct_writefloat(bstr,data->name,0);
    case BSTRUCT_NULL:
      return bstruct_writenull(bstr,data->name);
    case BSTRUCT_INDEXARRAY:
      fwrite(&data->token,1,1,bstr->file);
      if(fwrite(&data->size,sizeof(data->size),1,bstr->file)!=1)
        return TRUE;
      return fwrite(data->data.index,sizeof(long long),data->size,bstr->file)!=data->size;
    default:
      return TRUE;
  }
} /* of 'bstruct_writdata' */
