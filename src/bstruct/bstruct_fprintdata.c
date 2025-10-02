/**************************************************************************************/
/**                                                                                \n**/
/**           b  s  t  r  u  c  t  _  f  p  r  i  n  t  d  a  t  a  .  c           \n**/
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

void bstruct_fprintdata(FILE *file,               /**< pointer to text file */
                        const Bstruct_data *data, /**< pointer to data object to print */
                        int decimals              /**< number of decimals for float/double */ 
                       )
{
  String line;
  switch(data->token)
  {
    case BSTRUCT_NULL:
      fputs("null",file);
      break;
    case BSTRUCT_ZERO:
      fputc('0',file);
      break;
    case BSTRUCT_FZERO:
      fputs("0.0",file);
      break;
    case BSTRUCT_TRUE: case BSTRUCT_FALSE:
      fprintf(file,"%s",bool2str(data->data.b));
      break;
    case BSTRUCT_BYTE:
      fprintf(file,"%d",data->data.b);
      break;
    case BSTRUCT_SHORT:
      fprintf(file,"%d",data->data.s);
      break;
    case BSTRUCT_USHORT:
      fprintf(file,"%d",data->data.us);
      break;
    case BSTRUCT_INT:
      fprintf(file,"%d",data->data.i);
      break;
    case BSTRUCT_FLOAT:
      snprintf(line,STRING_LEN,"%.*g",decimals,data->data.f);
      fputs(line,file);
      if(strchr(line,'.')==NULL && strchr(line,'e')==NULL && strstr(line,"nan")==NULL && strstr(line,"inf")==NULL)
        fputs(".0",file);
      break;
    case BSTRUCT_DOUBLE:
      snprintf(line,STRING_LEN,"%.*g",decimals,data->data.d);
      fputs(line,file);
      if(strchr(line,'.')==NULL && strchr(line,'e')==NULL && strstr(line,"nan")==NULL && strstr(line,"inf")==NULL)
        fputs(".0",file);
      break;
    case BSTRUCT_STRING: case BSTRUCT_STRING1:
      fprintf(file,"\"%s\"",data->data.string);
      break;
    default:
      break;
  } /* of switch(data->token) */
} /* of 'bstruct_fprintdata' */
