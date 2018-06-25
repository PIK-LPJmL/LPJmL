/**************************************************************************************/
/**                                                                                \n**/
/**                     s  w  a  p  .  h                                           \n**/
/**                                                                                \n**/
/**     Converts big endian into little endian and vice versa                      \n**/
/**     Needed for reading data under IA32                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef SWAP_H /* Already included? */
#define SWAP_H

/* Definition of datatypes */

typedef struct
{
  int lo,hi;
} Num;

/* Declaration of functions */

extern short int swapshort(short int);
extern unsigned short swapushort(unsigned short);
extern int swapint(int);
extern unsigned int swapuint(unsigned int);
extern long long swaplong(long long);
extern double swapdouble(Num);
extern float swapfloat(int);
extern size_t freadint(int *,size_t,Bool,FILE *);
extern size_t freaduint(unsigned int *,size_t,Bool,FILE *);
extern size_t freadlong(long long *,size_t,Bool,FILE *);
extern size_t freadreal(Real *,size_t,Bool,FILE *);
extern size_t freadfloat(float *,size_t,Bool,FILE *);
extern size_t freaddouble(double *,size_t,Bool,FILE *);
extern size_t freadshort(short *,size_t,Bool,FILE *);
extern size_t freadushort(unsigned short *,size_t,Bool,FILE *);
extern Bool bigendian(void);

/* Definitions of macros */

#define freadint1(data,swap,file) freadint(data,1,swap,file)
#define freadreal1(data,swap,file) freadreal(data,1,swap,file)
#define freadshort1(data,swap,file) freadshort(data,1,swap,file)
#define freadlong1(data,swap,file) freadlong(data,1,swap,file)


#endif
