/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  i  n  t  e  r  n  .  h               \n**/
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "types.h"
#include "errmsg.h"
#include "swap.h"
#include "list.h"
#include "hash.h"
#include "bstruct.h"

#define BSTRUCT_HEADER "BSTRUCT"
#define BSTRUCT_VERSION 1
#define BSTRUCT_MAXTOKEN BSTRUCT_END
#define BSTRUCT_HASHSIZE 1023
#define MAXLEVEL 15 /**< maximum number of nested structs */

extern const size_t bstruct_typesizes[];

#define getname(name) (name==NULL) ? "unnamed" : name
#define isinvalidtoken(token) (((token) & 63)>BSTRUCT_MAXTOKEN)

struct bstruct
{
  FILE *file; /**< pointer to binary file */
  Bool swap;  /**< byte order has to be changed at reading */
  Bool isout; /**< error output on stderr enabled */
  int level;  /**< number of nested structs */
  int imiss;  /**< number of objects not in right order */
  struct
  {
    int size;            /**< size of array */
    int nr;              /**< object number */
    Byte type;           /**< type of object (BSTRUCT_ARRAY, BSTRUCT_STRUCT) */
    char *name;          /**< name of struct */
    List *varnames;      /**< list of objects in struct */
  } namestack[MAXLEVEL]; /**< list of objects names for each nested level */
  Hash hash;             /**< hash for object names used for writing restart files */
  int count;             /**< size of name table */
  Hashitem *names;       /**< name table used for reading restart files */
};                       /**< Definition of opaque datatype Bstruct */

typedef struct
{
  long long filepos; /**< position of object in file */
  short id;          /**< name of object */
  Byte token;        /**< token of object */
} Var;

extern int bstruct_gethashkey(const char *,int);
extern int bstruct_cmpname(const void *,const void *);
extern void freenamestack(Bstruct);
extern void printnamestack(const Bstruct);
extern Bool bstruct_readtoken(Bstruct,Byte *,Byte,const char *);
extern Bool bstruct_skipdata(Bstruct,Byte);
extern Bool bstruct_findobject(Bstruct,Byte *,Byte,const char *);
extern Bool bstruct_writename(Bstruct bstr,Byte,const String);
