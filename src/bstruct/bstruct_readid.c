/**************************************************************************************/
/**                                                                                \n**/
/**              b  s  t  r  u  c  t  _  r  e  a  d  i  d  .  c                    \n**/
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

Bool bstruct_readid(Bstruct bstr, /**< pointer to restart file */
                    Byte token,   /**< token */
                    Id *id        /**< id read */
                   )              /** \return TRUE on error */
{
  /* Function reads id */
  Byte id1;
  if((token & 64)==64) /* bit 7 set, id is of type unsigned short */
  {
    if(freadushort(id,1,bstr->swap,bstr->file)!=1)
      return TRUE;
  }
  else
  {
    if(fread(&id1,1,1,bstr->file)!=1)
      return TRUE;
    *id=id1;
  }
  return FALSE;
} /* of 'bstruct_readid' */
