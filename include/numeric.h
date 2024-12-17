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

/* Definition of constants */

#ifdef USE_RAND48
#define NSEED 3 /* length of seed vector */
#else
#define NSEED 1 /* length of seed vector */
#endif

/* Definition of datatypes */

typedef Real (*Bisectfcn)(Real,void *);

#ifdef USE_RAND48
typedef unsigned short int Seed[NSEED]; /* Seed for erand48() random number generator */
#else
typedef int Seed[NSEED]; /* Seed for randfrac() random number generator */
#endif

/* Declaration of functions */

extern Real bisect(Real (*)(Real,void *),Real,Real,void *,Real,Real,int,int *); /* find zero */
extern Real leftmostzero(Real (*)(Real,void *),Real,Real,void *,Real,Real,int); /* find leftmost zero */
extern void linreg(Real *,Real *,const Real[],int); /* linear regression */
extern void setseed(Seed,int); /* set seed of random number generator */
extern Bool freadseed(FILE *,Seed, Bool);
extern Real randfrac(int *); /* random number generator */
extern void petpar(Real *,Real *,Real *,Real *,Real,int,Real,Real,Real);
extern void petpar2(Real *,Real *,Real *,Real,int,Real,Real,Real,Bool,Real);
extern void petpar3(Real *,Real *,Real *,Real,int,Real,Real,Real);
extern int ivec_sum(const int[],int); /* vector sum of integers */
extern Real gammq(Real,Real);
extern void gcf(Real *, Real, Real, Real *);
extern void gser(Real *, Real, Real, Real *);
extern Real runmean_add(Real, Real, Real);
extern Real gammp(Real, Real);
extern Real itersolve(Real, Real, Real, Real, Real, Real, Real, Real, int);
extern void interpolate_data(Real *, Real *, Real *, int n, Real);
extern void permute(int [],int,Seed);

#ifndef USE_RAND48
/* if erand48() function is not defined, use randfrac instead */
#define erand48(seed) randfrac(seed)
#endif

/* weighted average */
#define weightedaverage(v1,v2,w1,w2) ((v1*w1+v2*w2)/(w1+w2))

#endif
