/**************************************************************************************/
/**                                                                                \n**/
/**               b  s  t  r  u  c  t  _  w  r  i  t  e  n  u  l  l  .  c          \n**/
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

Bool bstruct_writenull(Bstruct bstr,    /**< pointer to restart file */
                       const char *name /**< name of object or NULL */
                      )                 /** \return TRUE on error */
{
  return bstruct_writename(bstr,BSTRUCT_NULL,name);
} /* of 'bstruct_writenull' */
