/**************************************************************************************/
/**                                                                                \n**/
/**          f  p  r  i  n  t  r  e  s  d  a  t  a  .  c                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static void fprinthist(FILE *file,                   /**< file pointer to print to */
                       Real hist[HIST_YEARS][NMONTH] /**< history data to print */
                      )                              /** \return void */
{
  int i,j;
  fprintf(file,"Year");
  for(j=0;j<NMONTH;j++)
    fprintf(file," %-6s",months[j]);
  fprintf(file,"\n");
  for(i=0;i<HIST_YEARS;i++)
  {
    fprintf(file,"%4d",i);
    for(j=0;j<NMONTH;j++)
      fprintf(file," %6.1f",hist[i][j]);
    fprintf(file,"\n");
  }
} /* of 'fprinthist' */

void fprintresdata(FILE *file,       /**< filepointer to print to */
                   const Cell *cell  /**< pointer to cell */
                   )                 /** \return void */
{
  int i;
  fprintf(file,"Reservoir frac:\t%g\n"
               "C pool:\t%g\n"
               "water mass:\t%g\n" 
               "k_rls:\t%g\n"
               "target release for coming year:\t%g\n",
          cell->ml.reservoirfrac,cell->ml.resdata->c_pool,
          cell->ml.resdata->dmass,cell->ml.resdata->k_rls,
          cell->ml.resdata->target_release_year);
  fprintf(file, "target release for the coming months:");
  for(i=0;i<NMONTH;i++)
    fprintf(file, "\t%g", cell->ml.resdata->target_release_month[i]);
  fprintf(file, "\n");
  fprintf(file, "buffer for irrigation outflow:");
  for(i=0;i<NIRRIGDAYS;i++)
    fprintf(file, "\t%g", cell->ml.resdata->dfout_irrigation_daily[i]);
  fprintf(file, "\n");
  fprintf(file,"demand history:\n");
  fprinthist(file,cell->ml.resdata->demand_hist);
  fprintf(file,"inflow history:\n");
  fprinthist(file,cell->ml.resdata->inflow_hist);
  fprintf(file,"level history:\n");
  fprinthist(file,cell->ml.resdata->level_hist);
} /* of 'fprintresdata' */
