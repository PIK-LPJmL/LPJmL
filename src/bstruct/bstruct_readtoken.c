/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  r  e  a  d  t  o  k  e  n  .  c      \n**/
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

Bool bstruct_readtoken(Bstruct bstr,     /**< pointer to restart file */
                       Byte *token_read, /**< token read from file */
                       Byte token,       /**< token expected */
                       const char *name  /**< name of object expected */
                      )                  /** \return TRUE on error */
{
  /* Function reads token from file */
  /* The bits of the token byte are defined as follows:
     [   8   |  7    | 6  5  4  3  2  1|
     |hasname|id >255|  type (0..63)   |
     Next byte contains the index in the name table if bit 8 is set.
     Next 16bit word contains the index in the name table if bit 8 and 7 are set.
     token & 128(=0b10000000)  gets top bit
     token & 64 (=0b01000000)  gets bit 7
     token & 63 (=0b00111111)  gets bit 1..6 for type
   */
  if(fread(token_read,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR501: Cannot read '%s': %s.\n",
              name,strerror(errno));
    return TRUE;
  }
  if(isinvalidtoken(*token_read))
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR502: Invalid token %d reading %s.\n",
                *token_read,bstruct_typenames[token & 63]);
      else
        fprintf(stderr,"ERROR502: Invalid token %d reading %s '%s'.\n",
                *token_read,bstruct_typenames[token & 63],name);
     }
     return TRUE;
  }
  if(*token_read==BSTRUCT_END)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR523: End token read reading %s.\n",
                bstruct_typenames[token & 63]);
      else
        fprintf(stderr,"ERROR523: End token read reading %s '%s'.\n",
                bstruct_typenames[token & 63],name);
    }
    return TRUE;
  }
  if(*token_read==BSTRUCT_ENDARRAY)
  {
    if(bstr->isout)
    {
      if(name==NULL)
      {
        fprintf(stderr,"ERROR503: End of array found, %s expected.\n",
                bstruct_typenames[token & 63]);
        bstruct_printnamestack(bstr);
      }
      else
      {
        fprintf(stderr,"ERROR506: Object '%s' not found in array.\n",name);
        bstruct_printnamestack(bstr);
      }
    }
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readtoken' */
