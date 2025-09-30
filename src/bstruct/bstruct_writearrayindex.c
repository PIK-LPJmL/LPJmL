/**************************************************************************************/
/**                                                                                \n**/
/**    b  s  t  r  u  c  t  _  w  r  i  t  e  a  r  r  a  y  i  n  d  e  x  .  c   \n**/
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

Bool bstruct_writearrayindex(Bstruct bstr,            /**< pointer to restart file */
                             long long filepos,       /**< file position of index vector */
                             const long long index[], /**< position vector to write */
                             int offset,
                             int size                 /**< size of index vector */
                            )                         /** \return TRUE on error */
{
  Bool rc;
  if(fseek(bstr->file,filepos+sizeof(long long)*offset,SEEK_SET))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR519: Cannot skip to position %d in index array.\n",offset);
    return TRUE;
  }
  /* write position vector */
  rc=fwrite(index,sizeof(long long),size,bstr->file)!=size;
  fseek(bstr->file,0,SEEK_END);
  return rc;
} /* of bstruct_writearrayindex' */
