/**************************************************************************************/
/**                                                                                \n**/
/**      a  l  l  o  c  a  t  e  _  r  e  s  e  r  v  o  i  r  .  c                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void allocate_reservoir(Cell grid[],         /**< LPJ grid */
                        int year,            /**< simulation year (AD) */
                        const Config *config /**< LPJ configuration */
                       )                     /** \return void */
{
  int i,cell,index;
  Real *out,*in;
  Real *sum;
  out=(Real *)pnet_output(config->irrig_res_back);
  in=(Real *)pnet_input(config->irrig_res_back);
  grid-=config->startgrid-config->firstgrid; /* adjust index */
  for(i=0;i<pnet_outlen(config->irrig_res_back);i++)
  {
    index=pnet_outindex(config->irrig_res_back,i);
    if(grid[index].ml.resdata->reservoir.year<=year)
    {
      if(!grid[index].ml.dam)
        initresdata(grid+index); /* initialize reservoir data; replaced to before out[i] is filled */
      /*main purpose irrigation OR there is irrigation as other purpose */
      if(grid[index].ml.resdata->reservoir.purpose[0]==2 ||
         grid[index].ml.resdata->reservoir.purpose[1]==1)
        out[i]=grid[index].ml.resdata->mean_volume;
      else
        out[i]=0;
      grid[index].ml.dam=TRUE;
    }
    else
      out[i]=0;
  }
  pnet_exchg(config->irrig_res_back);
  sum=newvec2(Real,pnet_lo(config->irrig_res),pnet_hi(config->irrig_res));
  check(sum+pnet_lo(config->irrig_res));
  for(cell=pnet_lo(config->irrig_res_back);cell<=pnet_hi(config->irrig_res_back);cell++)
  {
    sum[cell]=0;
    for(i=0;i<pnet_inlen(config->irrig_res_back,cell);i++)
      sum[cell]+=in[pnet_inindex(config->irrig_res_back,cell,i)];
    for(i=0;i<pnet_inlen(config->irrig_res_back,cell);i++)
      grid[cell].ml.fraction[i]=(sum[cell]==0) ? 0 : in[pnet_inindex(config->irrig_res_back,cell,i)]/sum[cell];
  }
  out=(Real *)pnet_output(config->irrig_res);
  in=(Real *)pnet_input(config->irrig_res);
  for(i=0;i<pnet_outlen(config->irrig_res);i++)
    out[i]=sum[pnet_outindex(config->irrig_res,i)];
  pnet_exchg(config->irrig_res);
  for(cell=pnet_lo(config->irrig_res);cell<=pnet_hi(config->irrig_res);cell++)
    if(grid[cell].ml.resdata!=NULL && grid[cell].ml.resdata->reservoir.year<=year)
      for(i=0;i<pnet_inlen(config->irrig_res,cell);i++)
        grid[cell].ml.resdata->fraction[i]=in[pnet_inindex(config->irrig_res,cell,i)] ? grid[cell].ml.resdata->mean_volume/in[pnet_inindex(config->irrig_res,cell,i)] :0;/* Hb 15-09-09 */
  freevec(sum,pnet_lo(config->irrig_res),pnet_hi(config->irrig_res));
} /* of 'allocate_reservoir' */
