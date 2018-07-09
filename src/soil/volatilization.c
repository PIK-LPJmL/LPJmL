/**************************************************************************************/
/**                                                                                \n**/
/**         v  o  l  a  t  i  l  i  z  a  t  i  o  n  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**     Model for Ammonia volatilization                                           \n**/
/**                                                                                \n**/
/**     Parameterizations are based on the article:                                \n**/
/**     F. Montes, C.A. Rotz, and H. Chaoui, 2009. PROCESS MODELING OF             \n**/
/**     AMMONIA VOLATILIZATION FROM AMMONIUM SOLUTION AND MANURE                   \n**/
/**     SURFACES: A REVIEW WITH RECOMMENDED MODELS, Transaction of the             \n**/
/**     ASABE, Vol 52(5), 1707-1719                                                \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static Real f_nh3(Real temp, /**< temperature (deg C) */
                  Real pH    /**< soil pH */
                 )
{
  Real k_a; /* dissociation constant */
  k_a=pow(10,0.05-2788./degCtoK(temp)); /* Eq. (10) in F. Montes et al. (2009) */
  return 1.0/(1.0+pow(10,-pH)/k_a); /* Eq. (4)  */
}

static Real k_h(Real temp /**< temperature (deg C) */
               )          /** \return Henry's law constant */
{
  return 0.2138/degCtoK(temp)*pow(10,6.123-1825/degCtoK(temp)); /* Eq. (11) */
}

static Real h_m(Real wind,  /**< wind speed (m/s) */
                Real temp,  /**< temperature (deg C) */
                Real length /**< characteristic length (m) */
               )            /** \return convective mass coefficient (m/s) */
{
  return 0.000612*pow(wind,0.8)*pow(degCtoK(temp),0.382)*pow(length,-0.2); /* Eq. (19) */
} /* of 'h_m' */

Real volatilization(Real NH4,    /**< Ammonium in the soil (gN/m2) */
                    Real wind,   /**< wind speed (m/s) */
                    Real temp,   /**< air temperature (deg C) */
                    Real length, /**< characteristic length (m) */
                    Real pH      /**< soil pH */
                   )             /** \return volatilization flux (gN/m2/d) */
{
  Real nh3_solution;
  Real nh3_gas;
  nh3_solution=f_nh3(temp,pH)*NH4/soildepth[0]*1000;  /* convert gN/m2 to gN/m3 by dividing through soildepth */
  nh3_gas=k_h(temp)*nh3_solution;
  return day2sec(h_m(wind,temp,length)*nh3_gas); /* Eq. (1)  gN/m2/s -> gN/m2/d */
} /* of 'volatilization' */
