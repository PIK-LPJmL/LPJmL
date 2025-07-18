/**************************************************************************************/
/**                                                                                \n**/
/**             b  s  t  r  u  c  t  _  i  s  d  e  f  i  n  e  d  .  c            \n**/
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

Bool bstruct_isdefined(Bstruct bstr,   /**< pointer to restart file */
                       const char *key /**< key to compare */
                      )                /** \return TRUE if key is identical to name read from file */
{
  /* Function checks if a key is defined in a struct */
  long long pos;
  Bool rc;
  Byte token,isout;
  /* store file position */
  pos=ftell(bstr->file);
  /* read token */
  if(fread(&token,1,1,bstr->file)!=1)
    return FALSE;
  if(isinvalidtoken(token))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR502: Invalid token %d checking for object '%s'.\n",
              token,getname(key));
    return FALSE;
  }
  isout=bstr->isout;
  /* temporarily switch off error messages */
  bstr->isout=FALSE;
  rc=bstruct_findobject(bstr,&token,BSTRUCT_BYTE,key);
  bstr->isout=isout;
  /* restore position in file */
  fseek(bstr->file,pos,SEEK_SET);
  return !rc;
} /* of bstruct_isdefined' */
