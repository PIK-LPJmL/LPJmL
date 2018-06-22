/**************************************************************************************/
/**                                                                                \n**/
/**                    c  f  l  u  x  _  s  u  m  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates total carbon flux.                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real cflux_sum(const Cell grid[],   /**< array of grid cells */
               const Config *config /**< LPJ configuration */
              )                     /** \return total carbon flux (gC) */
{
  int cell;
  Real sum,sum_total;
  sum=sum_total=0;
  for(cell=0;cell<config->ngridcell;cell++)
    sum+=(grid[cell].balance.nep-grid[cell].output.firec-grid[cell].output.flux_firewood+grid[cell].output.flux_estab
          -grid[cell].output.flux_harvest-grid[cell].balance.biomass_yield)*grid[cell].coord.area;
#ifdef USE_MPI
  /*
   * Use global reduce operation in order to sum carbon fluxes of all
   * tasks
   */
  MPI_Reduce(&sum,&sum_total,1,
             (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
             MPI_SUM,0,config->comm);
  return sum_total;
#else
  return sum;
#endif
} /* of 'cflux_sum' */
