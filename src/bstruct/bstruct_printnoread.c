/**************************************************************************************/
/**                                                                                \n**/
/**    b  s  t  r  u  c  t  _  p  r  i  n  t  n  o  r  e  a  d  .  c               \n**/
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

void bstruct_printnoread(Bstruct bstr, /**< pointer to restart file */
                         Bool flag     /**< enable print of unread variables (TRUE/FALSE) */
                        )
{
  bstr->print_noread=flag;
} /* of 'bstruct_printnoread' */
