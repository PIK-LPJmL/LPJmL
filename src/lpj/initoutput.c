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

Bool initoutput(Outputfile *outputfile, /**< Output data */
                Cell grid[],            /**< LPJ grid */
                int npft,               /**< number of natural PFTs */
                int ncft,               /**< number of crop PFTs */
                Config *config          /**< LPJmL configuration */
               )                        /**\ return TRUE on error */
{
  int i,maxsize,index;
  Bool isall;
  maxsize=1;
  config->totalsize=0;
  isall=TRUE;
  /* calculate size of output storage */
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
  if(!isall)
  {
    /* not all output is writtem add trash */
    config->totalsize+=maxsize;
    index=maxsize;
  }
  else
    index=0;
  /* calculate indices into output storage */
  for(i=FPC;i<NOUT;i++)
  {
    if(isopen(outputfile,i))
    {
      config->outputmap[i]=index;
      index+=config->outputsize[i];
    }
    else
      config->outputmap[i]=0; /* no output used, point to trash */
   
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
  if(isroot(*config))
  {
    printf("Memory allocated for output: ");
    printintf((int)(config->totalsize*sizeof(Real)));
    printf(" bytes/cell\n");
  }
  for(i=0;i<config->ngridcell;i++)
  {
    grid[i].output.data=newvec(Real,config->totalsize);
    checkptr(grid[i].output.data);
    if(config->double_harvest)
    {
      grid[i].output.syear2=newvec(int,2*ncft);
      checkptr(grid[i].output.syear2);
    }
    else
      grid[i].output.syear2=NULL;
#if defined IMAGE && defined COUPLED
    grid[i].pft_harvest=newvec(Real,config->outputsize[PFT_HARVESTC]);
    checkptr(grid[i].pft_harvest);
#endif
  }
  return FALSE;
} /* of 'initoutput' */
