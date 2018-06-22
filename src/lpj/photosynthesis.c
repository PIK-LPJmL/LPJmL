/**************************************************************************************/
/**                                                                                \n**/
/**                p  h  o  t  o  s  y  n  t  h  e  s  i  s  .  c                  \n**/
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

#include "lpj.h"

#define po2 20.9e3/* O2 partial pressure in Pa */
#define p 1.0e5   /* atmospheric pressure in Pa */
#define q10ko 1.2 /* q10 for temperature-sensitive parameter ko */
#define q10kc 2.1 /* q10 for temperature-sensitive parameter kc */
#define q10tau 0.57 /* q10 for temperature-sensitive parameter tau */
#define tau25 2600.0/* value of tau at 25 deg C */
#define cmass 12.0    /* atomic mass of carbon */
#define cq 4.6e-6   /* conversion factor for solar radiation at 550 nm */
                              /* from J/m2 to E/m2 (E mol quanta) */
#define lambdamc4 0.4/* optimal ratio of intercellular to ambient CO2 */
                               /* concentration (lambda) in C4 plants */
#define lambdamc3 0.8/* optimal (maximum) lambda in C3 plants */
#define n0 7.15     /* leaf N concentration (mg/g) not involved in */
                              /* photosynthesis */
#define m 25.0      /* corresponds to #define p in Eqn 28, Haxeltine */
                              /*  Prentice 1996 */
#define t0c3 250.0 /* base temperature (K) in Arrhenius temperature */
                              /* response function for C3 plants */
#define t0c4 260.0  /* base temperature in Arrhenius func for C4 plants */
#define tk25 298.15 /* 25 deg C in Kelvin */

/*
 *     Function photosynthesis 
 *
 *     Adapted from Farquhar (1982) photosynthesis model, as simplified by
 *     Collatz et al 1991, Collatz et al 1992 and Haxeltine & Prentice 1996
 *
 */

Real photosynthesis(Real *agd,     /**<  total daytime net photosynthesis (gC/m2/day */
                    Real *rd,      /**< daily leaf respiration (gC/m2/day) */
                    int path,      /**< Path (C3/C4) */ 
                    Real lambda,
                    Real tstress,  /**< temperature stress factor */
                    Real co2,      /**< atmospheric CO2 partial pressure (Pa) */
                    Real temp,     /**< temperature (deg C) */
                    Real apar,     /**< absorbed PAR */
                    Real daylength /**< daylength (h) */
                   )
{
  static Real ko,kc,tau,pi,c1,c2;
  Real je,jc,phipi,and,adt,b,s,sigma,vm;
  static Real fac,gammastar;
  if(tstress<1e-2) 
  {
    *agd=0;
    *rd=0;
    return 0;
  }
  else
  {
    if(path==C3)
    {
      ko=param.ko25*pow(q10ko,(temp-25)*0.1);
      kc=param.kc25*pow(q10kc,(temp-25)*0.1);
      fac=kc*(1+po2/ko);
      tau=tau25*pow(q10tau,(temp-25)*0.1); /*reflects the abiltiy of Rubisco to discriminate between CO2 and O2*/
      gammastar=po2/(2*tau);
      pi=lambdamc3*co2;
      c1=tstress*param.alphac3*((pi-gammastar)/(pi+2.0*gammastar));
 
      /* Calculation of C2C3, Eqn 6, Haxeltine & Prentice 1996 */

      c2=(pi-gammastar)/(pi+fac);
      
      s=(24/daylength)*param.bc3;
      sigma=1-(c2-s)/(c2-param.theta*s);
      sigma= (sigma<=0) ? 0 : sqrt(sigma);
      b=param.bc3;   /* Choose C3 value of b for Eqn 10, Haxeltine & Prentice 1996 */
      /*
       *       Intercellular CO2 partial pressure in Pa
       *       Eqn 7, Haxeltine & Prentice 1996
       */

      vm=(1.0/param.bc3)*(c1/c2)*((2.0*param.theta-1.0)*s-(2.0*param.theta*s-c2)*sigma)*apar*
         cmass*cq;

      pi=lambda*co2;

      /*       Recalculation of C1C3, C2C3 with actual pi */

      c1=tstress*param.alphac3*((pi-gammastar)/(pi+2.0*gammastar));

      c2=(pi-gammastar)/(pi+fac);
    }
    else /* C4 photosynthesis */
    {
      c1=tstress*param.alphac4;
      c2=1.0;
      b=param.bc4;
      s=(24/daylength)*param.bc4;
      sigma=1-(c2-s)/(c2-param.theta*s);
      sigma= (sigma<=0) ? 0 : sqrt(sigma);
      vm=(1.0/param.bc4)*c1/c2*((2.0*param.theta-1.0)*s-(2.0*param.theta*s-c2)*sigma)*apar*
         cmass*cq;
      
      /*
       *       Parameter accounting for effect of reduced intercellular CO2
       *       concentration on photosynthesis, Phipi.
       *       Eqn 14,16, Haxeltine & Prentice 1996
       *       Fig 1b, Collatz et al 1992
       */
      phipi=lambda/lambdamc4;
      if(phipi<1)
        c1=tstress*phipi*param.alphac4;
    }
    
    /*
     *     je is PAR-limited photosynthesis rate molC/m2/h, Eqn 3
     *     Convert je from daytime to hourly basis
     *
     *     Calculation of PAR-limited photosynthesis rate, JE, molC/m2/h
     *     Eqn 3, Haxeltine & Prentice 1996
     */
    je=c1*apar*cmass*cq/daylength;

    /*
     * Calculation of rubisco-activity-limited photosynthesis rate JC, molC/m2/h
     *    Eqn 5, Haxeltine & Prentice 1996
     */
    jc=c2*hour2day(vm);

    /*
     *    Calculation of daily gross photosynthesis, Agd, gC/m2/day
     *    Eqn 2, Haxeltine & Prentice 1996
     */
    
    *agd=(je+jc-sqrt((je+jc)*(je+jc)-4.0*param.theta*je*jc))/(2.0*param.theta)*daylength;

    /*    Daily leaf respiration, Rd, gC/m2/day
     *    Eqn 10, Haxeltine & Prentice 1996
     */
    *rd=b*vm;

    /*    Daily net photosynthesis (at leaf level), And, gC/m2/day */

    and=*agd-*rd;
  
    /*     Total daytime net photosynthesis, Adt, gC/m2/day
     *     Eqn 19, Haxeltine & Prentice 1996
     */
 
    adt=and+(1.0-hour2day(daylength))*(*rd); 
       
    /*     Convert adt from gC/m2/day to mm/m2/day using
     *     ideal gas equation
     */
    return adt/cmass*8.314*degCtoK(temp)/p*1000.0;
  }
} /* of 'photosynthesis' */
