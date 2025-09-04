/**************************************************************************************/
/**                                                                                \n**/
/**                       q  u  e  u  e  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of a delay queue                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef QUEUE_T  /* Already included? */
#define QUEUE_T

/* Definition of datatypes */

typedef struct queue *Queue;

/* Declarations of functions */

extern Queue newqueue(int);
extern void freequeue(Queue);
extern Real getqueue(const Queue,int);
extern void putqueue(Queue,Real);
extern Real sumqueue(const Queue);
extern Bool fwritequeue(Bstruct,const char *,const Queue);
extern Queue freadqueue(Bstruct,const char *);
extern void freequeue(Queue);
extern void fprintqueue(FILE *,const Queue);
extern int queuesize(const Queue);

/* Definition of macros */

#endif
