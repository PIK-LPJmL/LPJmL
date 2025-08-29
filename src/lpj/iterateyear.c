/**************************************************************************************/
/**                                                                                \n**/
/**               i  t  e  r  a  t  e  y  e  a  r  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs iteration over the cell grid for one year with           \n**/
/**     river routing enabled.                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool iterateyear(Outputfile *output,  /**< Output file data */
                 Cell grid[],         /**< cell array */
                 Input input,         /**< input data */
                 Real co2,            /**< atmospheric CO2 (ppmv) */
                 int npft,            /**< number of natural PFTs */
                 int ncft,            /**< number of crop PFTs */
                 int year,            /**< simulation year (AD) */
                 const Config *config /**< LPJ configuration */
                )                     /** \return TRUE on error */
{
  Dailyclimate daily;
  Bool intercrop;
  int month,dayofmonth,day;
  int cell;
  intercrop=getintercrop(input.landuse);
  if(setupannual_grid(output,grid,&input,year,npft,ncft,intercrop,config))
    return TRUE;
  day=1;
  foreachmonth(month)
  {
    initmonthly_grid(grid,month,year,input.climate,config);
    foreachdayofmonth(dayofmonth,month)
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        update_daily_cell(grid+cell,cell,&daily,co2,&input,day,dayofmonth,month,year,
                          npft,ncft,intercrop,config);
      }
      updatedaily_grid(output,grid,input.extflow,day,month,year,npft,ncft,config);
      day++;
    } /* of 'foreachdayofmonth */
    update_monthly_grid(output,grid,input.climate,month,year,npft,ncft,config);
  } /* of 'foreachmonth */
  updateannual_grid(output,grid,input.landcover,year,npft,ncft,intercrop,daily.isdailytemp,config);
  return FALSE;
} /* of 'iterateyear' */
