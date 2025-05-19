/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  a  p  p  e  n  d  .  c               \n**/
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

Bstruct bstruct_append(const char *filename, /**< filename of restart file to append */
                       Bool isout            /**< enable error output on stderr */
                      )                      /** \return pointer to restart file or NULL in case of error */
{
  /* Function opens restart file and seeks at the end of file to append data */
  Bstruct bstruct;
  bstruct=new(struct bstruct);
  if(bstruct==NULL)
  {
    printallocerr("bstruct");
    return NULL;
  }
  bstruct->isout=isout;
  /* Initialize name stack */
  bstruct->level=1;
  bstruct->namestack[0].type=BSTRUCT_BEGINSTRUCT;
  bstruct->namestack[0].nr=0;
  bstruct->namestack[0].size=0;
  bstruct->namestack[0].varnames=NULL;
  bstruct->namestack[0].name=NULL;
  bstruct->names=NULL;
  bstruct->count=0;
  bstruct->file=fopen(filename,"r+b");
  if(bstruct->file==NULL)
  {
    if(isout)
      printfopenerr(filename);
    free(bstruct);
    return NULL;
  }
  fseek(bstruct->file,0,SEEK_END); /* seek to end of file for append */
  bstruct->hash=newhash(BSTRUCT_HASHSIZE,hashkey,free);
  if(bstruct->hash==NULL)
  {
    printallocerr("hash");
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  return bstruct;
} /* of 'bstruct_append' */
