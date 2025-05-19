/**************************************************************************************/
/**                                                                                \n**/
/**    b  s  t  r  u  c  t  _  w  r  i  t  e  e  n  d  s  t  r  u  c  t  .  c      \n**/
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

Bool bstruct_writeendstruct(Bstruct bstr /**< pointer to restart file */
                           )             /** \return TRUE on error */
{
  /* Function ends current struct */
  Byte token;
  if(bstr->namestack[bstr->level-1].type==BSTRUCT_BEGINARRAY)
  {
    fprintf(stderr,"ERROR521: Endstruct not allowed in array '%s'.\n",
            getname(bstr->namestack[bstr->level-1].name));
    bstruct_printnamestack(bstr);
  }
  if(bstr->level==0)
  {
    fprintf(stderr,"ERROR516: Too many endstructs found.\n");
    return TRUE;
  }
  /* remove struct object from name stack */
  free(bstr->namestack[bstr->level-1].name);
  bstr->level--;
  token=BSTRUCT_ENDSTRUCT;
  return fwrite(&token,1,1,bstr->file)!=1;
} /* of 'bstruct_writeendstruct' */
