/**************************************************************************************/
/**                                                                                \n**/
/**                 w i t h d r a w a l _ d e m a n d . c                          \n**/
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
/**     Parallel version using the Pnet library                                    \n**/
/**     Pnet is described in:                                                      \n**/
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
#include "agriculture.h"

void withdrawal_demand(Cell *grid,          /**< LPJ grid */
                       const Config *config /**< LPJ configuration */
                      )
{
  int cell,i,s;
  Real *in,*out;
  Irrigation *data;
  Stand *stand;

  in=(Real *)pnet_input(config->irrig_neighbour);
  out=(Real *)pnet_output(config->irrig_neighbour);

  /* calculate water requirements */
  for(cell=0;cell<config->ngridcell;cell++)
    if(!grid[cell].skip)
    {
      grid[cell].discharge.gir=0.0;
      /* wateruse for irrigation */
      foreachstand(stand,s,grid[cell].standlist)
        if(stand->type->landusetype==AGRICULTURE || stand->type->landusetype==GRASSLAND || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE)
        {
          data=stand->data;
          if(data->irrigation)
          {
            grid[cell].discharge.gir+=max((data->net_irrig_amount+data->dist_irrig_amount-data->irrig_stor)/data->ec,0)*stand->frac*grid[cell].coord.area; /* interception losses is fraction (GIR-interception) / GIR from day before */
          }
        }

      /* wateruse for industry, household and livestock */
      grid[cell].discharge.waterdeficit+=grid[cell].discharge.wateruse;
      /* constrain build up of deficit */
      grid[cell].discharge.waterdeficit=min(grid[cell].discharge.waterdeficit,30*grid[cell].discharge.wateruse);

      grid[cell].discharge.wd_demand=grid[cell].discharge.waterdeficit+grid[cell].discharge.gir;

      /* estimate how much could be fulfilled locally and how much is needed from neighbour cell */
      grid[cell].discharge.wd_deficit=grid[cell].discharge.wd_demand-grid[cell].discharge.dfout;
      if(grid[cell].discharge.wd_deficit>0)
        grid[cell].discharge.wd_demand-=grid[cell].discharge.wd_deficit;
      else
        grid[cell].discharge.wd_deficit=0;

    }
    else
      grid[cell].discharge.wd_deficit=grid[cell].discharge.wd_demand=0;



  /* fill output buffer with water withdrawal deficits */
  for(i=0;i<pnet_outlen(config->irrig_neighbour);i++)
    out[i]=grid[pnet_outindex(config->irrig_neighbour,i)-config->startgrid+config->firstgrid].discharge.wd_deficit;

  pnet_exchg(config->irrig_neighbour);

  for(cell=0;cell<config->ngridcell;cell++)
  {
    /* add water withdrawal deficit from other cells */
    grid[cell].discharge.wd_neighbour=0;
    for(i=0;i<pnet_inlen(config->irrig_neighbour,cell+config->startgrid-config->firstgrid);i++)
    {
      grid[cell].discharge.wd_neighbour+=in[pnet_inindex(config->irrig_neighbour,
      cell+config->startgrid-config->firstgrid,i)];
    }
    grid[cell].discharge.wd_demand+=grid[cell].discharge.wd_neighbour;
  }

} /* of 'withdrawal_demand' */
