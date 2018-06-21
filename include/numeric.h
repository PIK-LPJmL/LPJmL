/**************************************************************************************/
/**                                                                                \n**/
/**                   n  u  m  e  r  i  c  .  h                                    \n**/
/**                                                                                \n**/
/**     Header for numerical utility routines                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef NUMERIC_H /* Already included? */
#define NUMERIC_H

/* Definition of datatypes */

typedef Real (*Bisectfcn)(Real,void *);

/* Declaration of functions */

extern Real bisect(Real (*)(Real,void *),Real,Real,void *,Real,Real,int); /* find zero */
extern Real leftmostzero(Real (*)(Real,void *),Real,Real,void *,Real,Real,int); /* find leftmost zero */
extern void linreg(Real *,Real *,const Real[],int); /* linear regression */
extern void setseed(int); /* set seed of random number generator */
extern int getseed(void); /* get seed of random number generator */
extern Real randfrac(void); /* random number generator */
extern void petpar(Real *,Real *,Real *,Real *,Real,int,Real,Real,Real);
extern void petpar2(Real *,Real *,Real *,Real,int,Real,Real,Real,Bool,Real);
extern void petpar3(Real *,Real *,Real *,Real,int,Real,Real,Real);
extern int ivec_sum(const int[],int); /* vector sum of integers */

#ifndef USE_RAND48
/* if drand48(),srand48() functions are not defined, use randfrac,setseed instead */
#define drand48() randfrac()
#define srand48(seed) setseed(seed)
#endif

#endif
