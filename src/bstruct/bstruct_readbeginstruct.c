/**************************************************************************************/
/**                                                                                \n**/
/**   b  s  t  r  u  c  t  _  r  e  a  d  b  e  g  i  n  s  t  r  u  c  t  .  c    \n**/
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

Bool bstruct_readbeginstruct(Bstruct bstr,    /**< pointer to restart file */
                            const char *name /**< name of object or NULL */
                           )                 /** \return TRUE on error */
{
  Byte token;
  if(bstruct_readtoken(bstr,&token,BSTRUCT_BEGINSTRUCT,name))
    return TRUE;
  if(bstruct_findobject(bstr,&token,BSTRUCT_BEGINSTRUCT,name))
    return TRUE;
  if((token & 63) !=BSTRUCT_BEGINSTRUCT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not struct.\n",
              name,bstruct_typenames[token & 63]);
    return TRUE;
  }
  if(bstr->level==MAXLEVEL-1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR515: Too deep nesting of structs, %d allowed.\n",MAXLEVEL);
    freenamestack(bstr);
    return TRUE;
  }
  /* add empty list of names for this new level */
  bstr->namestack[bstr->level].varnames=newlist(0);
  bstr->namestack[bstr->level].type=BSTRUCT_BEGINSTRUCT;
  bstr->namestack[bstr->level].nr=0;
  bstr->namestack[bstr->level].size=0;
  /* store name of struct */
  bstr->namestack[bstr->level++].name=(name==NULL) ? NULL : strdup(name);
  return FALSE;
} /* of 'bstruct_readbeginstruct' */
