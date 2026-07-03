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
  if(bstr->level>0)
  {
    free(bstr->namestack[bstr->level-1].name);
    bstr->level--;
  }
  /* skip whole array */
  return bstruct_skiparrayelements(bstr);
} /* of 'bstruct_skiparray' */
