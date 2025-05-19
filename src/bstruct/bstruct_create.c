/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  c  r  e  a  t  e  .  c               \n**/
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

Bstruct bstruct_create(const char *filename /**< filename of restart file to create */
                      )                     /** \return pointer to restart file or NULL in case of error */
{
  /* Function creates restart file */
  Bstruct bstruct;
  bstruct=new(struct bstruct);
  if(bstruct==NULL)
  {
    printallocerr("bstruct");
    return NULL;
  }
  bstruct->isout=TRUE;
  /* Initialize name stack */
  bstruct->level=1;
  bstruct->namestack[0].type=BSTRUCT_BEGINSTRUCT;
  bstruct->namestack[0].nr=0;
  bstruct->namestack[0].size=0;
  bstruct->namestack[0].varnames=NULL;
  bstruct->namestack[0].name=NULL;
  bstruct->count=0;
  bstruct->names=NULL;
  bstruct->file=fopen(filename,"wb");
  if(bstruct->file==NULL)
  {
    printfcreateerr(filename);
    free(bstruct);
    return NULL;
  }
  bstruct->hash=newhash(BSTRUCT_HASHSIZE,bstruct_gethashkey,free);
  if(bstruct->hash==NULL)
  {
    printallocerr("hash");
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  fwritetopheader(bstruct->file,BSTRUCT_HEADER,BSTRUCT_VERSION);
  /* skip space for file position for name table */
  fseek(bstruct->file,sizeof(long long),SEEK_CUR);
  return bstruct;
} /* of 'bstruct_create' */
