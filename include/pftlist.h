/**************************************************************************************/
/**                                                                                \n**/
/**                    p  f  t  l  i  s  t  .  h                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef PFTLIST_H /* Already included? */
#define PFTLIST_H

/* Definition of datatypes */

typedef struct
{
  Pft *pft; /* PFT array */
  int n;    /* size of PFT array */
} Pftlist;

/* Declaration of functions */

extern int delpft(Pftlist *,int);
extern void freepftlist(Pftlist *);
extern Real gp_sum(const Pftlist *,Real,Real,Real,Real,Real *,Real [],Real *);
extern Real fpc_sum(Real [],int,const Pftlist *);
extern int fwritepftlist(FILE *,const Pftlist *);
extern void fprintpftlist(FILE *,const Pftlist *);
extern Bool freadpftlist(FILE *,Stand *,Pftlist *,const Pftpar[],int,
                         Bool);
extern Real firepft(Litter *,Pftlist *,Real);
extern void newpftlist(Pftlist *);
extern int addpft(Stand *,const Pftpar *,int,int);

/* Definitions of macros */

#define getpft(pftlist,i) ((pftlist)->pft+(i))
#define foreachpft(pft,i,pftlist) for(i=0;i<getnpft(pftlist) && (pft=getpft(pftlist,i));i++)
#define getnpft(pftlist) (pftlist)->n
#define printpftlist(pftlist) fprintpftlist(stdout,pftlist)

#endif
