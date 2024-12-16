/**************************************************************************************/
/**                                                                                \n**/
/**                   g  a  m  m  a  f  u  n  c  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     gamma functions for gamma distribution calcs based on numerical recipes    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <math.h>
#include "types.h"     /* Definition of datatype Real  */
#include "numeric.h"
#include "errmsg.h"

#define ITMAX 100      /* Maximum allowed number of iterations.*/
#define EPS 3.0e-7     /* Relative accuracy.                   */
#define FPMIN 1.0e-30  /* Number near the smallest representable fp number */

Real gammp(Real a, Real x)
// Returns the incomplete gamma function P (a, x).
{
  Real gamser, gammcf, gln;

  if (x < 0.0 || a <= 0.0)
  {
    fail(GAMMA_FUNC_ERR, TRUE,FALSE, "Invalid arguments in routine gammp: a=%g, x=%g",a,x);
  }
  if (x < (a + 1.0))
  {     //                     Use the series representation.
    gser(&gamser, a, x, &gln);
    return gamser;
  }
  else
  {    //                      Use the continued fraction representation
    gcf(&gammcf, a, x, &gln);
    return 1.0 - gammcf;  //   and take its complement.
  }
} /* of 'gammmp' */

Real gammq(Real a, Real x)
// Returns the incomplete gamma function Q(a, x) ? 1 ? P (a, x).
{
  Real gamser, gammcf, gln;

  if (x < 0.0 || a <= 0.0)
    fail(GAMMA_FUNC_ERR, TRUE,FALSE, "Invalid arguments in routine gammq: a=%g, x=%g",a,x);
  if (x < (a + 1.0))
  {     //                         Use the series representation
    gser(&gamser, a, x, &gln);
    return 1.0 - gamser;   //      and take its complement.
  }
  else
  {     //                         Use the continued fraction representation.
    gcf(&gammcf, a, x, &gln);
    return gammcf;
  }
} /* of 'gammmq' */

void gser(Real *gamser, Real a, Real x, Real *gln)
// Returns the incomplete gamma function P (a, x) evaluated by its series representation as gamser.
// Also returns ln ?(a) as gln.
{
  int n;
  Real sum, del, ap;

  *gln = lgamma(a);
  if (x <= 0.0)
  {
    if (x < 0.0)
      fail(GAMMA_FUNC_ERR, TRUE,FALSE, "x=%g less than 0 in routine gser()",x);
    *gamser = 0.0;
    return;
  }
  else
  {
    ap = a;
    del = sum = 1.0 / a;
    for (n = 1; n <= ITMAX; n++)
    {
      ++ap;
      del *= x / ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS)
      {
        *gamser = sum*exp(-x + a*log(x) - (*gln));
        return;
      }
    }
    fail(GAMMA_FUNC_ERR, TRUE,FALSE, "a=%g too large, ITMAX too small in routine gser()",a);
  }
} /* of 'gser' */

void gcf(Real *gammcf, Real a, Real x, Real *gln)
// Returns the incomplete gamma function Q(a, x) evaluated by its continued fraction represen-
// tation as gammcf. Also returns ln(a) as gln.
{
  int i;
  Real an, b, c, d, del, h;

  *gln = lgamma(a);
  b = x + 1.0 - a;               //     Set up for evaluating continued fraction
  c = 1.0 / FPMIN;               //     by modi?ed Lentz?s method (�5.2)
  d = 1.0 / b;                   //     with b0 = 0.
  h = d;
  for (i = 1; i <= ITMAX; i++)   //     Iterate to convergence.
  {
    an = -i*(i - a);
    b += 2.0;
    d = an*d + b;
    if (fabs(d) < FPMIN) d = FPMIN;
    c = b + an / c;
    if (fabs(c) < FPMIN) c = FPMIN;
    d = 1.0 / d;
    del = d*c;
    h *= del;
    if (fabs(del - 1.0) < EPS) break;
  }
  if (i > ITMAX)
  {
    fail(GAMMA_FUNC_ERR, TRUE,FALSE, "a too large, ITMAX too small in gcf(a=%g, x=%g)",a,x);
  }
  *gammcf = exp(-x + a*log(x) - (*gln))*h;   //  Put factors in front.
} /* of 'gcf' */
