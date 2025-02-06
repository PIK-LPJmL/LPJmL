/**************************************************************************************/
/**                                                                                \n**/
/**                    thomas_algorithm.c                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/* This function performs the standard thomas algorithm to solve a
   tridiagonal matrix system. */
void thomas_algorithm(const Real *a, /**< sub diagonal elements */
                      const Real *b, /**< main diagonal elements */
                      const Real *c, /**< super diagonal elements */
                      const Real *d, /**< right hand side */
                      Real *x,       /**< solution */
                      const int n    /**< number of grid points */
                     )
{
  Real c_prime[n-1];
  Real d_prime[n];
  int i;

  /* modify coefficients by progressing in forward direction */
  /* this codes eliminiates the sub diagnal a an norms the diagonal b 1 */
  c_prime[0] = c[0] / b[0];
  for (i=1; i<n-1; i++)
    c_prime[i] = c[i] / (b[i] - a[i] * c_prime[i-1]);

  d_prime[0] = d[0] / b[0];
  for (i=1; i<n; i++)
    d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) / (b[i] - a[i] * c_prime[i-1]);

  /* back substitution */
  x[n-1] = d_prime[n-1];
  for (i = n-2; i>=0; i--)
    x[i] = d_prime[i] - c_prime[i] * x[i+1];
} /* of 'thomas_algorithm' */
