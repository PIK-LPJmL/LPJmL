/**************************************************************************************/
/**                                                                                \n**/
/**              u p d a t e d a i l y _ g r i d . c                               \n**/
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

void updatedaily_grid(Outputfile *output,  /**< Output file data */
                      Cell grid[],         /**< cell array */
                      Extflow extflow,     /**< Pointer to extflow data */
                      int day,             /**< day (1..365) */
                      int month,           /**< month (0..11) */
                      int year,            /**< simulation year (AD) */
                      int npft,            /**< number of natural PFTs */
                      int ncft,            /**< number of crop PFTs */
                      const Config *config /**< LPJ configuration */
                     )
{
#ifdef USE_TIMING
  double tstart;
  tstart=mrun();
#endif
  if(config->river_routing)
  {
    if(config->withlanduse)
      withdrawal_demand(grid,config);
    if(config->extflow)
    {
      if(getextflow(extflow,grid,day-1,year))
        fail(INVALID_EXTFLOW_ERR,FALSE,FALSE,"Cannot read external flow data");
    }
    drain(grid,month,config);

    if(config->withlanduse)
      wateruse(grid,npft,ncft,month,config);
  }

  if(config->withdailyoutput && day<NDAYYEAR && year>=config->outputyear)
    /* postpone last timestep until after annual processes */
  if(fwriteoutput(output,grid,year,day-1,DAILY,npft,ncft,config))
  {
    if(isroot(*config))
      printfailerr(WRITE_OUTPUT_ERR,TRUE,"Cannot write output");
    exit(WRITE_OUTPUT_ERR);
  }
#ifdef USE_TIMING
  timing[UPDATEDAILY_GRID_FCN]+=mrun()-tstart;
#endif
} /* of 'updatedaily_grid' */
