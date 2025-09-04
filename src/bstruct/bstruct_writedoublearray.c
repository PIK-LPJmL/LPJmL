/**************************************************************************************/
/**                                                                                \n**/
/**  b  s  t  r  u  c  t  _  w  r  i  t  e  d  o  u  b  l  e  a  r  r  a  y .  c   \n**/
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

Bool bstruct_writedoublearray(Bstruct bstr,       /**< pointer to restart file */
                              const char *name,   /**< name of object or NULL */
                              const double vec[], /**< array written to file */
                              int size            /**< size of array */
                             )                    /** \return TRUE on error */
{
  int i;
  bstruct_writebeginarray(bstr,name,size);
  for(i=0;i<size;i++)
    bstruct_writedouble(bstr,NULL,vec[i]);
  return bstruct_writeendarray(bstr);
} /* of 'bstruct_writedoublearray' */
