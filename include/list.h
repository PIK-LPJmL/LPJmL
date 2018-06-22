/**************************************************************************************/
/**                                                                                \n**/
/**                          l  i  s  t  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of a resizeable array                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef LIST_H /* Already included? */
#define LIST_H

/* Definition of datatypes */

typedef struct
{
  void **data; /* array of pointers to data */
  int n;       /* Length of list */
} List;

/* Declaration of functions */

extern List *newlist(void);
extern int addlistitem(List *,void *);
extern int dellistitem(List *,int);
extern void freelist(List *);

/* Definition of macros */

#define foreachlistitem(i,list) for(i=0;i<(list)->n;i++)
#define getlistitem(list,index) (list)->data[index]
#define isempty(list) ((list)->n==0)
#define getlistlen(list) (list)->n

#endif
