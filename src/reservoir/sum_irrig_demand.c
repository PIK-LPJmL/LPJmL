/**************************************************************************************/
/**                                                                                \n**/
/**     s  u  m  _  i  r  r  i  g  _  d  e  m  a  n  d  .  c                       \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/**     Function sums irrigation demands from cells to irrigation demand           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void sum_irrig_demand(Cell grid[],         /* LPJ grid */
                      const Config *config /* LPJ configuration */
                     )
{
  int i,cell;
  Real *in,*out;
  for(cell=0;cell<config->ngridcell;cell++)
    if(grid[cell].ml.mdemand<0)
      grid[cell].ml.mdemand=0;
  grid-=config->startgrid-config->firstgrid;
  out=(Real *)pnet_output(config->irrig_res);
  in=(Real *)pnet_input(config->irrig_res);
  for(i=0;i<pnet_outlen(config->irrig_res);i++)
    out[i]=grid[pnet_outindex(config->irrig_res,i)].ml.mdemand*
           grid[pnet_outindex(config->irrig_res,i)].coord.area;
  pnet_exchg(config->irrig_res);
  for(cell=pnet_lo(config->irrig_res);cell<=pnet_hi(config->irrig_res);cell++)
    if(grid[cell].ml.dam)
    {
      grid[cell].ml.resdata->mdemand=0;
      for(i=0;i<pnet_inlen(config->irrig_res,cell);i++)
        grid[cell].ml.resdata->mdemand+=in[pnet_inindex(config->irrig_res,cell,i)]*
                                     grid[cell].ml.resdata->fraction[i];
    }
} /* of 'sum_irrig_demand' */
