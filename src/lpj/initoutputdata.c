/**************************************************************************************/
/**                                                                                \n**/
/**                i  n  i  t  o  u  t  p  u  t  d  a  t  a  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes output data to zero                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define isinit(index) (isinit2(index,timestep,year,config))

static Bool isinit2(int index,int timestep,int year,const Config *config)
{
  if(timestep==ANNUAL && config->outnames[index].timestep>0)
    return ((year-config->outputyear) %  config->outnames[index].timestep==0);
  else
    return config->outnames[index].timestep==timestep;
} /* of 'isinit2' */

void initoutputdata(Output *output,      /**< output data */
                    int timestep,        /**< time step (ANNUAL, MONTHLY, DAILY) */
                    int year,            /**< simulation year */
                    const Config *config /**< LPJ configuration */
                   )
{
  int i,index;
  /* set output data to zero */
  for(index=FPC;index<NOUT;index++)
    if(isinit(index))
      for(i=0;i<config->outputsize[index];i++)
        output->data[config->outputmap[index]+i]=0;
  for(index=WPC;index<NOUT;index++)
    if(isinit(index))
      for(i=0;i<config->outputsize[index];i++)
        output->data[config->outputmap[index]+i]=0;
   if(isinit(PFT_GCGP))
     for(i=0;i<config->outputsize[PFT_GCGP];i++)
       output->data[config->outputmap[PFT_GCGP_COUNT]+i]=0;
   if(isinit(CFT_SWC))
     for(i=0;i<config->outputsize[CFT_SWC];i++)
       output->data[config->outputmap[NDAY_MONTH]+i]=0;
  /* set specific output data to one */
  if(isinit(DECAY_WOOD_AGR))
     output->data[config->outputmap[DECAY_WOOD_AGR]]=1;
  if(isinit(DECAY_WOOD_NV))
     output->data[config->outputmap[DECAY_WOOD_NV]]=1;
  if(isinit(DECAY_LEAF_AGR))
     output->data[config->outputmap[DECAY_LEAF_AGR]]=1;
  if(isinit(DECAY_LEAF_NV))
     output->data[config->outputmap[DECAY_LEAF_NV]]=1;
} /* of 'initoutputdata' */
