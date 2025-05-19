/**************************************************************************************/
/**                                                                                \n**/
/**        b  s  t  r  u  c  t  _  w  r  i  t  e  e  n  d  a  r  r  a  y  .  c     \n**/
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

Bool bstruct_writeendarray(Bstruct bstr /**< pointer to restart file */
                          )             /** \return TRUE on error */
{
  Byte token;
  if(bstr->namestack[bstr->level-1].type==BSTRUCT_BEGINSTRUCT)
  {
    fprintf(stderr,"ERROR521: Endarray not allowed in struct '%s'.\n",
            getname(bstr->namestack[bstr->level-1].name));
    printnamestack(bstr);
  }
  if(bstr->level==0)
  {
    fprintf(stderr,"ERROR516: Too many endarrays found.\n");
    return TRUE;
  }
  if(bstr->namestack[bstr->level-1].nr!=bstr->namestack[bstr->level-1].size)
    fprintf(stderr,"ERROR522: Size of array '%s'=%d not equal of number of objects written=%d\n",
            getname(bstr->namestack[bstr->level-1].name),
            bstr->namestack[bstr->level-1].size,
            bstr->namestack[bstr->level-1].nr);
  /* remove object from name stack */
  free(bstr->namestack[bstr->level-1].name);
  bstr->level--;
  token=BSTRUCT_ENDARRAY;
  return fwrite(&token,1,1,bstr->file)!=1;
} /* of 'bstruct_writeendarray' */
