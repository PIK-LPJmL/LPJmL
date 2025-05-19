/**************************************************************************************/
/**                                                                                \n**/
/**    b  s  t  r  u  c  t  _  w  r  i  t  e  b  e  g  i  n  a  r  r  a  y  .  c   \n**/
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

Bool bstruct_writebeginarray(Bstruct bstr,     /**< pointer to restart file */
                             const char *name, /**< name of object or NULL */
                             int size          /**< size of array */
                            )                  /** \return TRUE on error */
{
  /* Function adds a new level of array with size elements  in restart file */
  Byte b;
  b=(size<=UCHAR_MAX) ? BSTRUCT_BEGINARRAY1 : BSTRUCT_BEGINARRAY;
  if(bstruct_writename(bstr,b,name))
    return TRUE;
  if(bstr->level==MAXLEVEL-1)
  {
    fprintf(stderr,"ERROR515: Too deep nesting of arrays, %d allowed.\n",MAXLEVEL);
    printnamestack(bstr);
    return TRUE;
  }
  /* push object on name stack */
  bstr->namestack[bstr->level].type=BSTRUCT_BEGINARRAY;
  bstr->namestack[bstr->level].nr=0;
  bstr->namestack[bstr->level].size=size;
  bstr->namestack[bstr->level].name=(name==NULL) ? NULL : strdup(name);
  bstr->namestack[bstr->level++].varnames=NULL;
  if(size<=UCHAR_MAX)
  {
    b=size;
    return fwrite(&b,1,1,bstr->file)!=1;
  }
  else
    return fwrite(&size,sizeof(size),1,bstr->file)!=1;
} /* of 'bstruct_writebeginarray' */
