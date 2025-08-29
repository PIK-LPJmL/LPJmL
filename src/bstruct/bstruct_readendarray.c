/**************************************************************************************/
/**                                                                                \n**/
/**           b  s  t  r  u  c  t  _  r  e  a  d  e  n  d  a  r  r  a  y  .  c     \n**/
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

Bool bstruct_readendarray(Bstruct bstr,    /**< pointer to restart file */
                          const char *name /**< object name */
                         )                 /** \return TRUE on error */
{
  Byte token;
  if(fread(&token,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading token for endarray of '%s'.\n",
              getname(name));
    return TRUE;
  }
  if(token!=BSTRUCT_ENDARRAY)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR509: Type=%s is not endarrary.\n",
                bstruct_typenames[token & 63]);
      else
        fprintf(stderr,"ERROR509: Type=%s is not endarray for array '%s'.\n",
                bstruct_typenames[token & 63],name);
    }
    return TRUE;
  }
  if(bstr->level==0)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR516: Too many endarrays found.\n");
    return TRUE;
  }
  bstr->level--;
  free(bstr->namestack[bstr->level].name);
  return FALSE;
} /* of 'bstruct_readendarray' */
