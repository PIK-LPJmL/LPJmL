/**************************************************************************************/
/**                                                                                \n**/
/**  b  s  t  r  u  c  t  _  w  r  i  t  e  b  e  g  i  n  s  t  r  u  c  t  .  c  \n**/
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

Bool bstruct_writebeginstruct(Bstruct bstr,    /**< pointer to restart file */
                              const char *name /**< name of object or NULL */
                             )                 /** \return TRUE on error */
{
  /* Function adds a new level of struct in restart file */
  if(bstruct_writename(bstr,BSTRUCT_BEGINSTRUCT,name))
    return TRUE;
  if(bstr->level==MAXLEVEL-1)
  {
    fprintf(stderr,"ERROR515: Too deep nesting of structs, %d allowed.\n",MAXLEVEL);
    bstruct_printnamestack(bstr);
    return TRUE;
  }
  /* add new item to namestack */
  bstr->namestack[bstr->level].type=BSTRUCT_BEGINSTRUCT;
  bstr->namestack[bstr->level].nr=0;
  bstr->namestack[bstr->level].name=(name==NULL) ? NULL : strdup(name);
  bstr->namestack[bstr->level++].varnames=NULL;
  return FALSE;
} /* of 'bstruct_writebeginstruct' */
