/**************************************************************************************/
/**                                                                                \n**/
/**             w  a  t  e  r  u  s  e  .  c                                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Mathematical description  can be found in:                                 \n**/
/**     Rost, S., Gerten, D., Bondeau, A., Lucht, W., Rohwer, J.,                  \n**/
/**     Schaphoff, S.: Agricultural green and blue water consumption and           \n**/
/**     its influence on the global water system. Water Resources                  \n**/
/**     Research (in press).                                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void wateruse(Cell *grid,          /**< LPJ grid */
              int npft,            /**< number of natural PFTs */
              int ncft,            /**< number of crop PFTs */
              const Config *config /**< LPJ configuration */
             )
{
  int cell,i;
  Real surplus;
  Real *in,*out;
  Real wd_neighbour;

  in=(Real *)pnet_input(config->irrig_back);
  out=(Real *)pnet_output(config->irrig_back);

  /* calculate fraction of satisfiable neighbour demand */
  for(cell=0;cell<config->ngridcell;cell++)
  {
    surplus=grid[cell].discharge.withdrawal-(grid[cell].discharge.wd_demand-grid[cell].discharge.wd_neighbour);
    surplus=surplus>0 ? surplus : 0.0;
    grid[cell].discharge.withdrawal-=surplus;
    /*withdrawl which can be used for neighbour irrigation*/
    grid[cell].output.mwd_neighb-=surplus/grid[cell].coord.area;
    grid[cell].discharge.wd_neighbour=grid[cell].discharge.wd_neighbour>0 ? surplus/grid[cell].discharge.wd_neighbour : 0.0;
  }
  /* fill output buffer with fraction of satisfiable neighbour demand */
  for(i=0;i<pnet_outlen(config->irrig_back);i++)
    out[i]=grid[pnet_outindex(config->irrig_back,i)-config->startgrid+config->firstgrid].discharge.wd_neighbour;

  pnet_exchg(config->irrig_back);

  for(cell=0;cell<config->ngridcell;cell++)
  {
    /* collect water from neighbour cell and add to local withdrawal */
    wd_neighbour=0.0;
    for(i=0;i<pnet_inlen(config->irrig_back,cell+config->startgrid-config->firstgrid);i++)
      wd_neighbour+=in[pnet_inindex(config->irrig_back,cell+config->startgrid-config->firstgrid,i)]*grid[cell].discharge.wd_deficit;

    grid[cell].output.mwd_neighb+=wd_neighbour/grid[cell].coord.area;
    grid[cell].discharge.withdrawal+=wd_neighbour;

    /* water use for household, industry and livestock */
    if(grid[cell].discharge.withdrawal<grid[cell].discharge.waterdeficit)
    {
      grid[cell].discharge.waterdeficit-=grid[cell].discharge.withdrawal;
      grid[cell].output.awateruse_hil+=grid[cell].discharge.withdrawal;
      grid[cell].discharge.withdrawal=0.0;
    }
    else
    {
      grid[cell].discharge.withdrawal-=grid[cell].discharge.waterdeficit;
      grid[cell].output.awateruse_hil+=grid[cell].discharge.waterdeficit;
      grid[cell].discharge.waterdeficit=0.0;

    }

    grid[cell].discharge.irrig_unmet=grid[cell].discharge.gir>grid[cell].discharge.withdrawal ?
                                     grid[cell].discharge.gir-grid[cell].discharge.withdrawal : 0.0;

  }/* of for each cell */

  /* get additional water from reservoirs */
  if(config->reservoir)
    irrig_amount_reservoir(grid,config);

  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      if(grid[cell].ml.dam)
        grid[cell].balance.total_reservoir_out+=grid[cell].ml.resdata->demand_fraction*grid[cell].ml.resdata->ddemand;

      grid[cell].balance.total_irrig_from_reservoir+=grid[cell].discharge.act_irrig_amount_from_reservoir;
      grid[cell].discharge.withdrawal+=grid[cell].discharge.act_irrig_amount_from_reservoir;
      grid[cell].output.mwd_res+=grid[cell].discharge.act_irrig_amount_from_reservoir/grid[cell].coord.area;

      distribute_water(&grid[cell],config->irrig_scenario,config->pft_output_scaled,npft,ncft);
    }
  }

} /* of 'wateruse' */
