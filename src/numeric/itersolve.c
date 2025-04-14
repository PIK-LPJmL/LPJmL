/**************************************************************************************/
/**                                                                                \n**/
/**                   i  t  e  r  s  o  l  v  e  .  c                              \n**/
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

#include <stdio.h>
#include <math.h>
#include "types.h"   /* Definition of datatype Real  */
#include "bstruct.h"
#include "numeric.h"
#include "errmsg.h"

#define ITMAX 100            /* Maximum allowed number of iterations */
#define EPSITER 1.0e-4       /* Relative accuracy in itersolve       */

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

  cti_mid = (cti_min + cti_max) * 0.5;
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
    // 		fail("itersolve.c: itercount exceeds max iterations");
    fprintf(stderr,"ERROR258: itercount exceeds max iterations in itersolve(). itercount=%d ctimin=%g ctimid=%g ctimax=%g pmin=%g pmid=%g pmax=%g target=%g\n",
            itercount, cti_min, cti_mid, cti_max, p_min, p_mid, p_max, target);
  }
  return cti_mid;
} /* of 'itersolve' */
