/************************************************************************/
/*                                                                      */
/*  gamma.c -- computation of gamma function.                           */
/*      Algorithms and coefficient values from "Computation of Special  */
/*      Functions", Zhang and Jin, John Wiley and Sons, 1996.           */
/*                                                                      */
/*  (C) 2003, C. Bond. All rights reserved. http://www.crbond.com/      */
/*                                                                      */
/* Returns gamma function of argument 'x'.                              */
/*                                                                      */
/* NOTE: Returns HUGE_VAL if argument is a negative integer or 0,       */
/*      or if argument exceeds 171.                                     */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <math.h>
#include "types.h"
#include "numeric.h"

Real gamma(Real x)
{
    int i,k,m;
    Real ga,gr,r,z;

    static Real g[] = {
        1.0,
        0.5772156649015329,
       -0.6558780715202538,
       -0.420026350340952e-1,
        0.1665386113822915,
       -0.421977345555443e-1,
       -0.9621971527877e-2,
        0.7218943246663e-2,
       -0.11651675918591e-2,
       -0.2152416741149e-3,
        0.1280502823882e-3,
       -0.201348547807e-4,
       -0.12504934821e-5,
        0.1133027232e-5,
       -0.2056338417e-6,
        0.6116095e-8,
        0.50020075e-8,
       -0.11812746e-8,
        0.1043427e-9,
        0.77823e-11,
       -0.36968e-11,
        0.51e-12,
       -0.206e-13,
       -0.54e-14,
        0.14e-14};

    if (x > 171.0) return HUGE_VAL;    /* This value is an overflow flag. */
    if (x == (int)x) {
        if (x > 0.0) {
            ga = 1.0;               /* use factorial */
            for (i=2;i<x;i++) {
               ga *= i;
            }
         }
         else
            ga = HUGE_VAL;
     }
     else {
        if (fabs(x) > 1.0) {
            z = fabs(x);
            m = (int)z;
            r = 1.0;
            for (k=1;k<=m;k++) {
                r *= (z-k);
            }
            z -= m;
        }
        else
            z = x;
        gr = g[24];
        for (k=23;k>=0;k--) {
            gr = gr*z+g[k];
        }
        ga = 1.0/(gr*z);
        if (fabs(x) > 1.0) {
            ga *= r;
            if (x < 0.0) {
                ga = -M_PI/(x*ga*sin(M_PI*x));
            }
        }
    }
    return ga;
}
