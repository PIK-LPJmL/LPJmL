/**************************************************************************************/
/**                                                                                \n**/
/**                          h  a  s  h  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of a hash table to store key/value items in a dictionary  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef HASH_H
#define HASH_H

typedef struct hash *Hash;

typedef struct
{
  char *key;  /**< key to store/retrieve data */
  void *data; /**< pointer to data stored for key */
} Hashitem;

extern Hash newhash(int,int (*)(const char *,int),void (*)(void *));
extern int addhashitem(Hash,char *,void *);
extern Bool removehashitem(Hash,const char *);
extern int gethashcount(const Hash);
extern void *gethashitem(Hash,const char *);
extern Hashitem *hash2array(const Hash);
extern void deletehash(Hash);
extern void freehash(Hash);

#endif
