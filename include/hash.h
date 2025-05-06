/**************************************************************************************/
/**                                                                                \n**/
/**                          h  a  s  h  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of a hash                                                 \n**/
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
  void *key;
  void *data;
} Hashitem;

extern Hash newhash(int,int (*)(const void *,int),int (*)(const void *,const void *),
                    void (*)(void *));
extern int addhashitem(Hash,void *,void *);
extern Bool removehashitem(Hash,const void *);
extern int gethashcount(const Hash);
extern void *gethashitem(Hash,const void *);
extern Hashitem *hash2array(const Hash);
extern void deletehash(Hash);
extern void freehash(Hash);

#endif
