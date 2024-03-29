/**************************************************************************************/
/**                                                                                \n**/
/**               f  p  r  i  n  t  c  e  l  l  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints cell variables in text file (used by lpjprint              \n**/
/**     to print contents of restart file)                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintcell(FILE *file,            /**< file pointer to text file */
                const Cell grid[],     /**< cell array */
                int ncell,             /**< number of cells */
                int npft,              /**< number of natural PFTs */
                int ncft,              /**< number of crop PFTs */
                const Config *config   /**< LPJ configuration */
               )
{
  int i,cell,cft;
  for(cell=0;cell<ncell;cell++)
  {
    fputs("Coord:\t\t",file);
    fprintcoord(file,&grid[cell].coord);
    fprintf(file,"\nLand fraction:\t%g\n",grid[cell].landfrac);
    if(config->with_lakes)
      fprintf(file,"Lake fraction:\t%g\n",grid[cell].lakefrac);
    fputs("Random seed:\t",file);
    for(i=0;i<NSEED;i++)
      printf(" %d",grid[cell].seed[i]);
    fputc('\n',file);
    if(config->with_lakes)
      fprintf(file,"dmass_lake:\t%g (dm3)\n",grid[cell].discharge.dmass_lake);
    if(config->river_routing)
    {
       fprintf(file,"dfout:\t\t%g (dm3/d)\n"
                    "dmass_river:\t%g (dm3)\n"
                    "dmass_sum:\t%g (dm3)\n",
               grid[cell].discharge.dfout,
               grid[cell].discharge.dmass_river,grid[cell].discharge.dmass_sum);
       fputs("Queue:\t\t",file);
       fprintqueue(file,grid[cell].discharge.queue);
       fputc('\n',file);
       if(grid[cell].ml.dam)
         fprintresdata(file,grid+cell);
    }
    if(grid[cell].skip)
      fputs("Invalid soil\n",file);
    else
    {
      fputs("GDD:\t\t",file);
      for(cft=0;cft<npft;cft++)
        fprintf(file," %6.1f",grid[cell].gdd[cft]);
      fputc('\n',file);
      if(config->withlanduse!=NO_LANDUSE)
      {
        if(grid[cell].ml.sdate_fixed!=NULL)
        {
          fputs("CFT     ",file);
          for(cft=0;cft<ncft;cft++)
            fprintf(file," %5d",cft);
          fputs("\n--------",file);
          for(cft=0;cft<ncft;cft++)
            fputs(" -----",file);
          fputs("\nrfsdate ",file);
          for(cft=0;cft<ncft;cft++)
            fprintf(file," %5d",grid[cell].ml.sdate_fixed[cft]);
          fputs("\nirrsdate",file);
          for(cft=ncft;cft<2*ncft;cft++)
            fprintf(file," %5d",grid[cell].ml.sdate_fixed[cft]);
          fputc('\n',file);
        }
        if(grid[cell].ml.crop_phu_fixed!=NULL)
        {
          fputs("\nfphu    ",file);
          for(cft=0;cft<ncft;cft++)
            fprintf(file," %5g",grid[cell].ml.crop_phu_fixed[cft]);
          fputs("\nirrphu  ",file);
          for(cft=ncft;cft<2*ncft;cft++)
            fprintf(file," %5g",grid[cell].ml.crop_phu_fixed[cft]);
          fputc('\n',file);
        }
        fputs("--------",file);
        for(cft=0;cft<ncft;cft++)
          fputs(" -----",file);
        fprintf(file,"\nCropfrac (rf/ir):\t%g\t%g\n",grid[cell].ml.cropfrac_rf,grid[cell].ml.cropfrac_ir);
        fprintcropdates(file,grid[cell].ml.cropdates,config->pftpar+npft,ncft);
      }
      if(config->with_nitrogen)
      {
        fputs("Establ. stock for biomass:\n"
              "Type  RF     IR              RF     IR\n",file);
        fprintf(file,"tree  %6.2f %6.2f (gC/m2) %6.2f %6.2f (gN/m2)\n",
                grid[cell].balance.estab_storage_tree[0].carbon,
                grid[cell].balance.estab_storage_tree[1].carbon,
                grid[cell].balance.estab_storage_tree[0].nitrogen,
                grid[cell].balance.estab_storage_tree[1].nitrogen);
        fprintf(file,"grass %6.2f %6.2f (gC/m2) %6.2f %6.2f (gN/m2)\n",
                grid[cell].balance.estab_storage_grass[0].carbon,
                grid[cell].balance.estab_storage_grass[1].carbon,
                grid[cell].balance.estab_storage_grass[0].nitrogen,
                grid[cell].balance.estab_storage_grass[1].nitrogen);
#ifndef IMAGE
        fprintf(file,"fast product:\t%g (gC/m2) %g (gN/m2)\n",
                grid[cell].ml.product.fast.carbon,
                grid[cell].ml.product.fast.nitrogen);
        fprintf(file,"slow product:\t%g (gC/m2) %g (gN/m2)\n",
                grid[cell].ml.product.slow.carbon,
                grid[cell].ml.product.slow.nitrogen);
#endif
      }
      else
      {
        fputs("Establ. stock for biomass:\n"
              "Type  RF     IR\n",file);
        fprintf(file,"tree  %6.2f %6.2f (gC/m2)\n",
                grid[cell].balance.estab_storage_tree[0].carbon,
                grid[cell].balance.estab_storage_tree[1].carbon);
        fprintf(file,"grass %6.2f %6.2f (gC/m2)\n",
                grid[cell].balance.estab_storage_grass[0].carbon,
                grid[cell].balance.estab_storage_grass[1].carbon);
#ifndef IMAGE
        fprintf(file,"fast product:\t%g (gC/m2)\n",
                grid[cell].ml.product.fast.carbon);
        fprintf(file,"slow product:\t%g (gC/m2)\n",
                grid[cell].ml.product.slow.carbon);
#endif
      }
      if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
        fprintignition(file,&grid[cell].ignition);
      fprintf(file,"excess water:\t%g (mm)\n",grid[cell].balance.excess_water);
      fprintstandlist(file,grid[cell].standlist,config->pftpar,npft+ncft,config->with_nitrogen);
    }
  } /* of 'for(cell=...)' */
} /* of 'fprintcell' */
