/**************************************************************/
/*                                                            */
/*  gammafunc.c                                               */
/*  gamma function for gamma distribution calcs               */
/*                                                            */
/*  based on numerical recipes                                */
/*                                                            */
/*  Thomas Kleinen, 07/12/2009                                */
/*                                                            */
/**************************************************************/

#include <stdio.h>
#include <math.h>
#include "types.h"   /* Definition of datatype Real  */
#include "numeric.h"
#include "errmsg.h"

#define ITMAX 100            /*                      Maximum allowed number of iterations.*/
#define EPS 3.0e-7           /*                      Relative accuracy.                   */
#define EPSITER 1.0e-4       /*                      Relative accuracy in itersolve.                  */
#define FPMIN 1.0e-30        /*                      Number near the smallest representable fp number */

Real itersolve(Real p_min,
  Real p_max,
  Real cti_min,
  Real cti_max,
  Real target,
  Real phi,
  Real chi,
  Real mu,
  int itercount)
{
  Real cti_mid, p_mid;

  cti_mid = (cti_min + cti_max) / 2.;
  p_mid = gammp(phi, ((cti_mid - mu) / chi));

  if (itercount < ITMAX)
  {
    if (fabs(p_mid - target) > EPSITER)
    {
      if (p_mid < target)
        cti_mid = itersolve(p_mid, p_max, cti_mid,
          cti_max, target, phi, chi, mu, itercount + 1);
      else
        cti_mid = itersolve(p_min, p_mid, cti_min,
          cti_mid, target, phi, chi, mu, itercount + 1);
    }
  }
  else
  {
    // 		fail("gammafunc.c: itercount exceeds max iterations");
    printf("gammafunc.c: itercount exceeds max iterations. itercount ctimin ctimid ctimax, pmin pmid pmax target %d %g %g %g %g %g %g %g\n",
      itercount, cti_min, cti_mid, cti_max, p_min, p_mid, p_max, target);
  }
  return cti_mid;
}

Real runmean_add(Real runmean_current, Real add, Real avtime)
{
  Real tmp;

  tmp = runmean_current * ((avtime - 1.) / avtime);
  tmp += add * (1. / avtime);

  return tmp;
}

Real gammln(Real xx)
// Returns the value ln[?(xx)] for xx > 0.
{
  //     Internal arithmetic will be done in double precision, a nicety that you can omit if ?ve-?gure
  //     accuracy is good enough.
  double x, y, tmp, ser;
  static double cof[6] = { 76.18009172947146,-86.50532032941677,
    24.01409824083091,-1.231739572450155,
    0.1208650973866179e-2,-0.5395239384953e-5 };
  int j;
  y = x = xx;
  tmp = x + 5.5;
  tmp -= (x + 0.5)*log(tmp);
  ser = 1.000000000190015;
  for (j = 0; j <= 5; j++) ser += cof[j] / ++y;
  return -tmp + log(2.5066282746310005*ser / x);
}

Real gammp(Real a, Real x)
// Returns the incomplete gamma function P (a, x).
{
  Real gamser, gammcf, gln;

  if (x < 0.0 || a <= 0.0)
  {
    fprintf(stderr, "gammafunc.c: gammp. a x %g %g\n", a, x);
    fail(GAMMA_FUNC_ERR, FALSE, "Invalid arguments in routine gammp");
  }
  if (x < (a + 1.0))
  {     //           Use the series representation.
    gser(&gamser, a, x, &gln);
    return gamser;
  }
  else
  {    //                      Use the continued fraction representation
    gcf(&gammcf, a, x, &gln);
    return 1.0 - gammcf;  //          and take its complement.
  }
}

Real gammq(Real a, Real x)
// Returns the incomplete gamma function Q(a, x) ? 1 ? P (a, x).
{
  Real gamser, gammcf, gln;

  if (x < 0.0 || a <= 0.0) fail(GAMMA_FUNC_ERR, FALSE, "Invalid arguments in routine gammq");
  if (x < (a + 1.0))
  {     //           Use the series representation
    gser(&gamser, a, x, &gln);
    return 1.0 - gamser;   //         and take its complement.
  }
  else
  {     //                     Use the continued fraction representation.
    gcf(&gammcf, a, x, &gln);
    return gammcf;
  }
}


void gser(Real *gamser, Real a, Real x, Real *gln)
// Returns the incomplete gamma function P (a, x) evaluated by its series representation as gamser.
// Also returns ln ?(a) as gln.
{
  int n;
  Real sum, del, ap;

  *gln = gammln(a);
  if (x <= 0.0) {
    if (x < 0.0) fail(GAMMA_FUNC_ERR, FALSE, "x less than 0 in routine gser");
    *gamser = 0.0;
    return;
  }
  else
  {
    ap = a;
    del = sum = 1.0 / a;
    for (n = 1; n <= ITMAX; n++) {
      ++ap;
      del *= x / ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS) {
        *gamser = sum*exp(-x + a*log(x) - (*gln));
        return;
      }
    }
    fail(GAMMA_FUNC_ERR, FALSE, "a too large, ITMAX too small in routine gser");
    return;
  }
}

void gcf(Real *gammcf, Real a, Real x, Real *gln)
// Returns the incomplete gamma function Q(a, x) evaluated by its continued fraction represen-
// tation as gammcf. Also returns ln ?(a) as gln.
{
  int i;
  Real an, b, c, d, del, h;

  *gln = gammln(a);
  b = x + 1.0 - a;                    //                Set up for evaluating continued fraction
  c = 1.0 / FPMIN;                  //                     by modi?ed Lentz?s method (�5.2)
  d = 1.0 / b;                      //                     with b0 = 0.
  h = d;
  for (i = 1; i <= ITMAX; i++) {        //              Iterate to convergence.
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
    printf("gammafunc.c: gcf. a x %g %g\n", a, x);
    fail(GAMMA_FUNC_ERR, FALSE, "a too large, ITMAX too small in gcf");
  }
  *gammcf = exp(-x + a*log(x) - (*gln))*h;   //            Put factors in front.
}
