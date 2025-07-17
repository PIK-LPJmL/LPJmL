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

extern Queue newqueue(int,int);
extern void freequeue(Queue);
extern void getqueue(const Queue,Real [],int);
extern void setqueue(Queue,const Real [],int);
extern void putqueue(Queue,const Real []);
extern Real sumqueue(const Queue,int);
extern Real weighted_fireduration_from_queue(const Queue);
extern Real max_fireduration_from_queue(const Queue);
extern void allsumqueue(const Queue,Real []);
extern Bool fwritequeue(Bstruct,const char *,const Queue);
extern Bool skipqueue(FILE *,Bool);
extern Queue freadqueue(Bstruct,const char *);
extern void freequeue(Queue);
extern void fprintqueue(FILE *,const Queue);
extern int queuesize(const Queue);
extern int lengthqueue(const Queue);

/* Definition of macros */

#endif
