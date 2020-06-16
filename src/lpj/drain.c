/**************************************************************************************/
/**                                                                                \n**/
/**                     d  r  a  i  n  .  c                                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Mathematical description of the river routing algorithm can be             \n**/
/**     found in:                                                                  \n**/
/**     Rost, S., Gerten, D., Bondeau, A., Lucht, W., Rohwer, J.,                  \n**/
/**     Schaphoff, S.: Agricultural green and blue water consumption and           \n**/
/**     its influence on the global water system. Water Resources                  \n**/
/**     Research (in press).                                                       \n**/
/**                                                                                \n**/
/**     Function performs river routing using the parallel Pnet library            \n**/
/**                                                                                \n**/
/**     Description of pnet is given in:                                           \n**/
/**     W. von Bloh, 2008. Sequential and Parallel Implementation of               \n**/
/**     Networks. In P. beim Graben, C. Zhou, M. Thiel, and J. Kurths              \n**/
/**     (eds.), Lectures in Supercomputational Neuroscience, Dynamics in           \n**/
/**     Complex Brain Networks, Springer, New York, 279-318.                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static Real fillreservoir(Real *mass,Real in,Real mass_max)
{
  *mass+=in;
  if(*mass>mass_max)
  {
    in=*mass-mass_max;
    *mass=mass_max;
  }
  else
    in=0;
  return in;
} /* of 'fillreservoir' */

void drain(Cell grid[],         /**< Cell array */
           int month,           /**< month (0..11) */
           const Config *config /**< LPJmL configuration */
          )
{
  int count,cell,i,j,iter,t,ncoeff;
  Real fin,*out,*in;
  Real fout_lake,irrig_to_river;

  count=(int)(1.0/TSTEP); /* calculate number of iterations */
  out=(Real *)pnet_output(config->route);
  in=(Real *)pnet_input(config->route);

  for(cell=0;cell<config->ngridcell;cell++)
  {

    grid[cell].discharge.dfout=0.0;

    /* add runoff to reservoir or lake */
    if(!grid[cell].skip)
    {
      grid[cell].discharge.mfin+=grid[cell].discharge.drunoff*grid[cell].coord.area;
      if(grid[cell].ml.dam)
      {
        /* runoff fills reservoir, if full, the excess water is added to the lake*/
        grid[cell].discharge.dmass_lake+=fillreservoir(&grid[cell].ml.resdata->dmass,
                                                       grid[cell].discharge.drunoff*grid[cell].coord.area,
                                                       grid[cell].ml.resdata->reservoir.capacity);
      }
      else
        grid[cell].discharge.dmass_lake+=grid[cell].discharge.drunoff*grid[cell].coord.area;
    }

    /* release from reservoirs*/
    if(grid[cell].ml.dam)
    {

      grid[cell].ml.resdata->dfout_res=0.0; /* dfout_res = total outflow of reservoir [dm3] */

      if(grid[cell].discharge.next>=0)
        grid[cell].ml.resdata->dfout_res=outflow_reservoir(grid[cell].ml.resdata,month);

      /* irrigation reservoirs*/
      if(grid[cell].ml.resdata->reservoir.purpose[0]==2 || grid[cell].ml.resdata->reservoir.purpose[1]==1)
      {
        /* the water that was reserved for irrigation 5 days ago, but still not used is released to the river*/
        irrig_to_river=grid[cell].ml.resdata->dfout_irrigation_daily[0];
        for(i=0;i<(NIRRIGDAYS-1);i++)
          grid[cell].ml.resdata->dfout_irrigation_daily[i]=grid[cell].ml.resdata->dfout_irrigation_daily[i+1];

        grid[cell].ml.resdata->dfout_irrigation_daily[(NIRRIGDAYS-1)]=0.0;

        /* part of todays outflow goes directly to the river
         * (environmental flow) and part can be used for irrigation
         */

        if(grid[cell].ml.resdata->dfout_res>ENV_FLOW*grid[cell].ml.resdata->mean_inflow_month[month]/ndaymonth[month])
        {
          grid[cell].ml.resdata->dfout_irrigation_daily[(NIRRIGDAYS-1)]=grid[cell].ml.resdata->dfout_res
                                                        -ENV_FLOW*grid[cell].ml.resdata->mean_inflow_month[month]/ndaymonth[month];
          grid[cell].ml.resdata->dfout_res-=grid[cell].ml.resdata->dfout_irrigation_daily[(NIRRIGDAYS-1)];
        }
        grid[cell].ml.resdata->dfout_res+=irrig_to_river;
      } /* end of irrigation reservoirs */

      grid[cell].discharge.dmass_lake+=grid[cell].ml.resdata->dfout_res;
    }

  } /* of 'for(cell=...)' */


  grid-=config->startgrid-config->firstgrid; /* adjust first index of grid
                                              array needed for pnet library */
  for(iter=0;iter<count;iter++)
  {
    for(i=pnet_lo(config->route);i<=pnet_hi(config->route);i++)
    {
      /* calculate outflow */
      grid[i].discharge.fout=0;
      ncoeff=queuesize(grid[i].discharge.queue);
      for(t=0;t<ncoeff;t++)
        grid[i].discharge.fout+=getqueue(grid[i].discharge.queue,t)*grid[i].discharge.tfunct[t];

      grid[i].discharge.dmass_river-=grid[i].discharge.fout;
      grid[i].discharge.dfout+=grid[i].discharge.fout;
      grid[i].output.mdischarge+=grid[i].discharge.fout;
      grid[i].output.daily.discharge+=grid[i].discharge.fout*1e-3*dayseconds1; /*in m3 per second */
      grid[i].discharge.mfout+=grid[i].discharge.fout;
    }

    /* fill output buffer */
    for(i=0;i<pnet_outlen(config->route);i++)
      out[i]=grid[pnet_outindex(config->route,i)].discharge.fout;

    /* communication function fills input buffer */
    pnet_exchg(config->route);

    for(i=pnet_lo(config->route);i<=pnet_hi(config->route);i++)
    {
      /* sum up all inflows from other cells */
      fin=0.0;
      for(j=0;j<pnet_inlen(config->route,i);j++)
        fin+=in[pnet_inindex(config->route,i,j)];

      grid[i].discharge.mfin+=fin;

      if(grid[i].ml.dam)
        fin=fillreservoir(&grid[i].ml.resdata->dmass,fin,
                          grid[i].ml.resdata->reservoir.capacity);

      fin=fillreservoir(&grid[i].discharge.dmass_lake,fin,
                         grid[i].discharge.dmass_lake_max);


      /* lake outflow */
      if(grid[i].discharge.dmass_lake>0.0 && grid[i].discharge.next>=0)
      {
        fout_lake=kr/count*grid[i].discharge.dmass_lake*pow(grid[i].discharge.dmass_lake/grid[i].discharge.dmass_lake_max,1.5);
        grid[i].discharge.dmass_lake-=fout_lake;
        fin+=fout_lake;
      }

      /* water withdrawal */
      if(fin>grid[i].discharge.wd_demand/count)
      {
        grid[i].discharge.withdrawal+=grid[i].discharge.wd_demand/count;
        grid[i].discharge.mfout+=grid[i].discharge.wd_demand/count;
        fin-=grid[i].discharge.wd_demand/count;
      }
      else
      {
        grid[i].discharge.withdrawal+=fin;
        grid[i].discharge.mfout+=fin;
        fin=0.0;
      }
  
      /* the remainder enters the river system */
      grid[i].discharge.dmass_river+=fin;
      putqueue(grid[i].discharge.queue,fin);
      grid[i].discharge.dmass_sum+=grid[i].discharge.dmass_river+grid[i].discharge.dmass_lake+sumqueue(grid[i].discharge.queue);


    } /* of 'for(i=...)' */
  } /* of 'for(iter=...)' */

  grid+=config->startgrid-config->firstgrid; /* re-adjust first index of grid array */

  
  for(cell=0;cell<config->ngridcell;cell++)
  {
    grid[cell].output.mwateramount+=grid[cell].discharge.dmass_sum/count;
    grid[cell].discharge.dmass_sum=0.0;
    if(grid[cell].discharge.withdrawal<grid[cell].discharge.wd_demand)
    {
      if(grid[cell].discharge.dmass_lake>grid[cell].discharge.wd_demand-grid[cell].discharge.withdrawal)
      {
        grid[cell].discharge.dmass_lake-=grid[cell].discharge.wd_demand-grid[cell].discharge.withdrawal;
        grid[cell].discharge.mfout+=grid[cell].discharge.wd_demand-grid[cell].discharge.withdrawal;
        grid[cell].discharge.withdrawal=grid[cell].discharge.wd_demand;
      }
      else
      {
        grid[cell].discharge.mfout+=grid[cell].discharge.dmass_lake;
        grid[cell].discharge.withdrawal+=grid[cell].discharge.dmass_lake;
        grid[cell].discharge.dmass_lake=0.0;
      }
    }
    grid[cell].output.mwd_local+=grid[cell].discharge.withdrawal/grid[cell].coord.area; /* withdrawal in local cell */
  }

}  /* of 'drain' */
