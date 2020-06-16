/**************************************************************************************/
/**                                                                                \n**/
/**                    l  a  k  e  t  e  m  p  .  c                                \n**/
/**                                                                                \n**/
/**     lake temperature scheme, modeled after soiltemp.c                          \n**/
/**                                                                                \n**/
/**     soiltemp() is invoked only for the stands of each cell,                    \n**/
/**     i.e. excluding the lake areas. This function fills that gap.               \n**/
/**     Parameterization taken from Piccolroaz et al.,                             \n**/
/**     Hydrol. Earth Syst. Sci., 17, 3323-3338, 2013                              \n**/
/**     in a simplified form:                                                      \n**/
/**                                                                                \n**/
/**     dT/dt=rho_4/delta(T) (T_air-T),                                            \n**/
/**                                                                                \n**/
/**     where delta(T) is the thickness of the surface well-mixed layer            \n**/
/**     A simple Euler scheme is used with adaptive time steps                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifdef COUPLING_WITH_FMS

#include "lpj.h"

#define T_I 4.0  /* (deg C) */
#define rho_4 6.18e-3
#define rho_6 3.08
#define rho_7 14.41
#define rho_8 0.31

#ifdef USE_FMSCLOCK
extern void fmsclock_begin_(const int *id);
extern void fmsclock_end_(const int *id);
extern int lpj_laketemp_clock_id;
#endif

/* Function for calculating the dimensionless thickness delta of the 
 * surface well-mixed layer
 */

static Real delta(Real laketemp)
{
  Real res;
  if(laketemp>=T_I)
  {
    res=exp((T_I-laketemp)/rho_6);
    if(res<1e-5)
      res=1e-5;
  }
  else
  {
    res=exp((laketemp-T_I)/rho_7)+exp(-laketemp/rho_8);
    if(res>1)
      res=1;
  }
  return res;
} /* of 'delta' */

void laketemp(Cell *cell, /**< pointer to LPJ cell */
              const Dailyclimate *climate /**< daily climate data */
             )
{
  int i,k;
  Real deltat,t,t_old;
#ifdef USE_FMSCLOCK
  fmsclock_begin_(&lpj_laketemp_clock_id);
#endif
  /* calculate new lake temp for time step 1 day */
  deltat=rho_4/delta(cell->laketemp)*(climate->temp-cell->laketemp);
  t=cell->laketemp+deltat;
  k=1;
  do
  {
    t_old=t; /* save lake temperature from previous time steps */
    k*=2; /* double the time steps */
    t=cell->laketemp;
    for(i=0;i<k;i++)
    {
      deltat=rho_4/delta(t)*(climate->temp-t)/k;
      t+=deltat;
    }
  }
  while(fabs(t-t_old)>1e-2); /* repeat until error<0.01 */
  cell->laketemp=t;
#ifdef USE_FMSCLOCK
  fmsclock_end_(&lpj_laketemp_clock_id);
#endif
} /* of 'laketemp' */

#endif /* COUPLING_WITH_FMS */
