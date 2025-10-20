/**************************************************************************************/
/**                                                                                \n**/
/**         u  p  d  a  t  e  _  m  o  n  t  h  l  y  _  g  r  i  d  .  c          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     month                                                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void update_monthly_grid(Outputfile *output,  /**< Output file data */
                         Cell grid[],         /**< cell array */
                         Climate *climate,    /**< pointer to climate data */
                         int month,           /**< month (0..11) */
                         int year,            /**< simulation year (AD) */
                         int npft,            /**< number of natural PFTs */
                         int ncft,            /**< number of crop PFTs */
                         const Config *config /**< LPJ configuration */
                        )
{
  Pft *pft;
  Stand *stand;
  Real mtemp,mprec;
  int p,s,cell;
#ifdef CHECK_BALANCE
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  foreachstand(stand, s, cell->standlist)
  {
    st= standstocks(stand);
    start.carbon+=(st.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    start.nitrogen+=st.nitrogen*stand->frac;
  }
#endif
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      mtemp=getmtemp(climate,&grid[cell].climbuf,cell,month);
      mprec=getmprec(climate,&grid[cell].climbuf,cell,month);
      monthly_climbuf(&grid[cell].climbuf,mtemp,mprec,grid[cell].output.mpet,month);
      if(grid[cell].ml.dam) /* to store the monthly inflow and demand */
        update_reservoir_monthly(grid+cell,month,config);
      foreachstand(stand,s,grid[cell].standlist)
      {
        getlag(&stand->soil,month);
        foreachpft(pft,p,&stand->pftlist)
          turnover_monthly(&stand->soil.litter,pft,config);
      } /* of foreachstand */
#if defined IMAGE && defined COUPLED
      if(grid[cell].ml.image_data!=NULL)
      {
        //grid[cell].ml.image_data->mirrwatdem[month]+=grid[cell].output.irrig+grid[cell].output.mconv_loss_evap+grid[cell].output.mconv_loss_drain;
        //grid[cell].ml.image_data->mevapotr[month] += (grid[cell].output.transp + grid[cell].output.evap + grid[cell].output.interc + grid[cell].output.mevap_lake + grid[cell].output.mevap_res + grid[cell].output.mconv_loss_evap + grid[cell].output.mconv_loss_drain);
        grid[cell].ml.image_data->mpetim[month] += grid[cell].output.mpet;
      }
#endif
      grid[cell].hydrotopes.wetland_wtable_monthly*=ndaymonth1[month];
      grid[cell].hydrotopes.wtable_monthly*=ndaymonth1[month];
      if(month==0)
      {
        grid[cell].hydrotopes.wtable_min=grid[cell].hydrotopes.wtable_monthly;
        grid[cell].hydrotopes.wtable_max=grid[cell].hydrotopes.wtable_monthly;
        grid[cell].hydrotopes.wetland_wtable_max=grid[cell].hydrotopes.wetland_wtable_monthly;
      }
      else
      {
        grid[cell].hydrotopes.wtable_min=min(grid[cell].hydrotopes.wtable_min,grid[cell].hydrotopes.wtable_monthly);
        grid[cell].hydrotopes.wtable_max=max(grid[cell].hydrotopes.wtable_max,grid[cell].hydrotopes.wtable_monthly);
        grid[cell].hydrotopes.wetland_wtable_max=max(grid[cell].hydrotopes.wetland_wtable_max,grid[cell].hydrotopes.wetland_wtable_monthly);
      }
      grid[cell].hydrotopes.wetland_wtable_mean+=grid[cell].hydrotopes.wetland_wtable_monthly;
      grid[cell].hydrotopes.wtable_mean+=grid[cell].hydrotopes.wtable_monthly;
      /* for water balance check */
      grid[cell].balance.awater_flux+=((grid[cell].discharge.mfout-grid[cell].discharge.mfin)/grid[cell].coord.area);
    } /* if(!grid[cell].skip) */
  } /* of 'for(cell=0;...)' */
#ifdef DEBUG
  printf("year=%d,month=%d\n",year,month+1);
  printf("cell=%d\n",cell+config->startgrid);
  printcell(grid+cell,1,npft,ncft,config);
#endif
  if(year>=config->outputyear && month<NMONTH-1)
  {
    /* write out monthly output, postpone last timestep until after annual processes */
    if(fwriteoutput(output,grid,year,month,MONTHLY,npft,ncft,config))
    {
        if(isroot(*config))
          printfailerr(WRITE_OUTPUT_ERR,TRUE,"Cannot write output");
        exit(WRITE_OUTPUT_ERR);
    }
  }
#ifdef CHECK_BALANCE
  foreachstand(stand, s, cell->standlist)
  {
    st= standstocks(stand);
    end.carbon+=(st.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
  }
  if(fabs(start.carbon-end.carbon)>0.0001)
      fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s at the end: month=%d: C_ERROR=%g start : %g end : %g ",
           __FUNCTION__,month,start.carbon-end.carbon,start.carbon,end.carbon);
  if(fabs(start.nitrogen-end.nitrogen)>0.001)
      fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s at the end: month=%d: N_ERROR=%g start : %g end : %g ",
           __FUNCTION__,month,start.nitrogen-end.nitrogen,start.nitrogen,end.nitrogen);
#endif
} /* of 'update_monthly_grid' */
