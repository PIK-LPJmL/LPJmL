/**************************************************************************************/
/**                                                                                \n**/
/**  b  s  t  r  u  c  t  _  w  r  i  t  e  u  s  h  o  r  t  a  r  r  a  y  .  c  \n**/
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

Bool bstruct_writeushortarray(Bstruct bstr,                 /**< pointer to restart file */
                              const char *name,             /**< name of object or NULL */
                              const unsigned short value[], /**< array written to file */
                              int size                      /**< size of array */
                             )                              /** \return TRUE on error */
{
  int i;
  if(bstruct_writebeginarray(bstr,name,size))
    return TRUE;
  for(i=0;i<size;i++)
    if(bstruct_writeushort(bstr,NULL,value[i]))
      return TRUE;
  return bstruct_writeendarray(bstr);
} /* of 'bstruct_writeushortarray */
