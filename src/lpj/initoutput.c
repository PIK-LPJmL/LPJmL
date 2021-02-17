/**************************************************************************************/
/**                                                                                \n**/
/**                    i  n  i  t  o  u  t  p  u  t  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates output data                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}

Bool initoutput(Outputfile *outputfile,      /**< Output data */
                Cell grid[],
                int npft,            /**< number of natural PFTs */
                int ncft,            /**< number of crop PFTs */
                Config *config /**< LPJmL configuration */
               )                     /**\ return TRUE on error */
{
  int i,maxsize,index;
  Bool isall;
  maxsize=1;
  config->totalsize=0;
  isall=TRUE;
  for(i=FPC;i<NOUT;i++)
  {
    config->outputsize[i]=outputsize(i,npft,ncft,config);
    if(config->outputsize[i]>maxsize)
      maxsize=config->outputsize[i];
    if(isopen(outputfile,i))
       config->totalsize+=config->outputsize[i];
    else
     isall=FALSE;
  }
  config->outputsize[PFT_GCGP_COUNT]=config->outputsize[PFT_GCGP];
  config->outputsize[NDAY_MONTH]=config->outputsize[CFT_SWC];
  if(isopen(outputfile,PFT_GCGP))
    config->totalsize+=config->outputsize[PFT_GCGP];
  if(isopen(outputfile,CFT_SWC))
    config->totalsize+=config->outputsize[CFT_SWC];
  if(config->double_harvest && !isopen(outputfile,SYEAR2))
    config->totalsize+=config->outputsize[SYEAR2];
  if(!isall)
  {
    config->totalsize+=maxsize;
    index=maxsize;
  }
  else
    index=0;
  for(i=FPC;i<NOUT;i++)
  {
    if(isopen(outputfile,i))
    {
      config->outputmap[i]=index;
      index+=config->outputsize[i];
    }
    else
      config->outputmap[i]=0;
   
  }
  if(isopen(outputfile,PFT_GCGP))
  {
    config->outputmap[PFT_GCGP_COUNT]=index;
    index+=config->outputsize[PFT_GCGP];
  }
  else
    config->outputmap[PFT_GCGP_COUNT]=0;
  if(isopen(outputfile,CFT_SWC))
  {
    config->outputmap[NDAY_MONTH]=index;
    index+=config->outputsize[CFT_SWC];
  }
  else
    config->outputmap[NDAY_MONTH]=0;
  if(config->double_harvest && !isopen(outputfile,SYEAR2))
    config->outputmap[SYEAR2]=index;
    
  for(i=0;i<config->ngridcell;i++)
  {
    grid[i].output.data=newvec(Real,config->totalsize);
    checkptr(grid[i].output.data);
  }
  return FALSE;
} /* of 'initoutput' */
