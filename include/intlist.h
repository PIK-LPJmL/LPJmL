/**************************************************************************************/
/**                                                                                \n**/
/**                   i  n  t  l  i  s  t  .  h                                    \n**/
/**                                                                                \n**/
/**     C implementation of a resizeable integer array                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef INTLIST_H /* Already included? */
#define INTLIST_H

/* Definition of datatypes */

typedef struct
{
  int *index; /* Content of array or NULL for empty array */
  int n;      /* Length of array */
} Intlist;

/* Declaration of functions */

extern void initintlist(Intlist *);
extern Bool copyintlist(Intlist *,const Intlist *);
extern int  addintlistitem(Intlist *,int);
extern int  delintlistitem(Intlist *,int);
extern void emptyintlist(Intlist *);

/* Definition of macros */

#define isemptyintlist(list) ((list)->n==0)

#endif
