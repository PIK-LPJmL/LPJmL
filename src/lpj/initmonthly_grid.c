/**************************************************************************************/
/**                                                                                \n**/
/**               i n i t m o n t h l y _ g r i d . c                              \n**/
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

void initmonthly_grid(Cell grid[],         /**< cell array */
                      int month,           /**< month (0..11) */
                      int year,            /**< simulation year (AD) */
                      Climate *climate,    /**< pointer to climate data */
                      const Config *config /**< LPJ configuration */
                     )
{
  int cell;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    grid[cell].discharge.mfin=grid[cell].discharge.mfout=grid[cell].ml.mdemand=0.0;
    grid[cell].hydrotopes.wetland_wtable_monthly=0;
    grid[cell].hydrotopes.wtable_monthly=0;
    grid[cell].output.mpet=0;
    if(grid[cell].ml.dam)
      grid[cell].ml.resdata->mprec_res=0;
    initoutputdata(&((grid+cell)->output),MONTHLY,year,config);
    if(!grid[cell].skip)
    {
      initclimate_monthly(climate,&grid[cell].climbuf,cell,month,grid[cell].seed);

#if defined IMAGE && defined COUPLED
      monthlyoutput_image(&grid[cell].output,climate,cell,month,config);
#endif

#ifdef DEBUG
     printf("temp = %.2f prec = %.2f wet = %.2f",
           (getcelltemp(climate,cell))[month],
           (getcellprec(climate,cell))[month],
           (israndomprec(climate)) ? (getcellwet(climate,cell))[month] : 0);
     if(config->with_radiation)
     {
       if(config->with_radiation==RADIATION)
         printf("lwnet = %.2f ",(getcelllwnet(climate,cell))[month]);
       else if(config->with_radiation==RADIATION_LWDOWN)
         printf("lwdown = %.2f ",(getcelllwnet(climate,cell))[month]);
       printf("swdown = %.2f\n",(getcellswdown(climate,cell))[month]);
     }
     else
       printf("sun = %.2f\n",(getcellsun(climate,cell))[month]);
     if(config->prescribe_burntarea)
       printf("burntarea = %.2f \n",
              (getcellburntarea(climate,cell))[month]);
#endif
    } /* if(!grid[cell].skip) */
  } /* of 'for(cell=...)' */
} /* of 'initmonthly_grid' */
