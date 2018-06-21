/**************************************************************************************/
/**                                                                                \n**/
/**         i r r i g _ a m o u n t _ r e s e r v o i r . c                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void irrig_amount_reservoir(Cell grid[],          /**< LPJ grid */
                            const Config *config  /**< LPJ configuration */
                            )                     /** \return void */
{
  Real *in,*out;
  int i,cell,k;

  grid-=config->startgrid;
  out=(Real *)pnet_output(config->irrig_res);
  in=(Real *)pnet_input(config->irrig_res);
  for(i=0;i<pnet_outlen(config->irrig_res);i++)
    out[i]=grid[pnet_outindex(config->irrig_res,i)].discharge.irrig_unmet;
  pnet_exchg(config->irrig_res);
  for(cell=pnet_lo(config->irrig_res);cell<=pnet_hi(config->irrig_res);cell++)
    if(grid[cell].ml.dam)
    {
      grid[cell].ml.resdata->ddemand=0;

      /* calculate ddemand by summing up requests */
      for(i=0;i<pnet_inlen(config->irrig_res,cell);i++)
      {
        grid[cell].ml.resdata->ddemand+=in[pnet_inindex(config->irrig_res,cell,i)]*grid[cell].ml.resdata->fraction[i];
        grid[cell].ml.resdata->mdemand+=in[pnet_inindex(config->irrig_res,cell,i)]*grid[cell].ml.resdata->fraction[i];
      }
      /* calculate fraction of ddemand that can be fulfilled */
      if(grid[cell].ml.resdata->ddemand>0)
      {
        grid[cell].ml.resdata->dfout_irrigation=0.0;
        for(k=0;k<NIRRIGDAYS;k++)
          grid[cell].ml.resdata->dfout_irrigation+=grid[cell].ml.resdata->dfout_irrigation_daily[k]; /*sum buffers to get total available water for irrigation*/

        if(grid[cell].ml.resdata->dfout_irrigation<0 && grid[cell].ml.resdata->dfout_irrigation>=-0.0001)
          grid[cell].ml.resdata->dfout_irrigation=0.0;
        if(grid[cell].ml.resdata->dfout_irrigation<-0.0001)
          fail(OUTFLOW_RESERVOIR_ERR,TRUE,"dfout_irrigation<-0.0001");

        /*calculate how much of todays demand is available*/
        grid[cell].ml.resdata->demand_fraction=grid[cell].ml.resdata->dfout_irrigation/grid[cell].ml.resdata->ddemand;

        if(grid[cell].ml.resdata->demand_fraction>1)
          grid[cell].ml.resdata->demand_fraction=1;

        /* the demand is subtracted from storage for irrigation, the 'oldest' water is used first*/
        grid[cell].ml.resdata->dfout_irrigation_daily[0]-=grid[cell].ml.resdata->demand_fraction*grid[cell].ml.resdata->ddemand;

        for(k=0;k<NIRRIGDAYS-1;k++)
        {
          if(grid[cell].ml.resdata->dfout_irrigation_daily[k]<0)
          {
            grid[cell].ml.resdata->dfout_irrigation_daily[k+1]+=grid[cell].ml.resdata->dfout_irrigation_daily[k];
            grid[cell].ml.resdata->dfout_irrigation_daily[k]=0.0;
          }
        }
        if(grid[cell].ml.resdata->dfout_irrigation_daily[NIRRIGDAYS-1]<0 && grid[cell].ml.resdata->dfout_irrigation_daily[NIRRIGDAYS-1]>=-0.0005)
          grid[cell].ml.resdata->dfout_irrigation_daily[NIRRIGDAYS-1]=0.0;
        if(grid[cell].ml.resdata->dfout_irrigation_daily[NIRRIGDAYS-1]<-0.005)
          fail(OUTFLOW_RESERVOIR_ERR,TRUE,"dfout_irrigation_daily[%d] <-0.005: %.5f",NIRRIGDAYS-1,grid[cell].ml.resdata->dfout_irrigation_daily[NIRRIGDAYS-1]);

        grid[cell].discharge.mfout+=grid[cell].ml.resdata->demand_fraction*grid[cell].ml.resdata->ddemand;

      }
      else
        grid[cell].ml.resdata->demand_fraction=0;
    }

  out=(Real *)pnet_output(config->irrig_res_back);
  in=(Real *)pnet_input(config->irrig_res_back);

  /* write demand_fraction to outbuffer */
  for(i=0;i<pnet_outlen(config->irrig_res_back);i++)
    out[i]=grid[pnet_outindex(config->irrig_res_back,i)].ml.resdata->demand_fraction;

  pnet_exchg(config->irrig_res_back);
  for(cell=pnet_lo(config->irrig_res_back);cell<=pnet_hi(config->irrig_res_back);cell++)
  {
    grid[cell].discharge.act_irrig_amount_from_reservoir=0;
    for(i=0;i<pnet_inlen(config->irrig_res_back,cell);i++)
    {
      grid[cell].discharge.act_irrig_amount_from_reservoir+=grid[cell].ml.fraction[i]*in[pnet_inindex(config->irrig_res_back,cell,i)]*grid[cell].discharge.irrig_unmet;
    }
   }
} /* of 'irrig_amount_reservoir' */
