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
  /* Function creates restart file
   *   Structure of restart file:
   *   |'B'|'S'|'T'|'R'|'U'|'C'|'T'|1|0|0|0| # file header and versiona    (7 + 4 bytes)
   *   |p1|p2|p3|p4|p5|p6|p7|p8|             # pointer to name table       (8 bytes)
   *   |00TOKEN|                             # type without id             (1 byte)
   *   |10TOKEN|id|                          # type with byte id           (2 bytes)
   *   |11TOKEN|id1|id2|                     # type with word id           (3 bytes)
   *   |10TOKEN|id|d1|d2...|dn|              # type with byte id and data  (2 bytes + sizeof(data))
   *   ...
   *   |BSTRUCT_END|                         # end token                   (1 byte)
   *   |size1|size2|size3|size4|             # size of name table          (4 bytes)
   *   |len|s1|s2|...|sn|                    # name                        (1+ name length bytes)
   *   |id1|id2|                             # id                          (2 bytes )
   *   ....
   */
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
  bstruct->names2=NULL;
  bstruct->file=fopen(filename,"wb");
  if(bstruct->file==NULL)
  {
    printfcreateerr(filename);
    free(bstruct);
    return NULL;
  }
  /* Create empty hash to store/retrieve object names and their corresponding id */
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
