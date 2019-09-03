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
  int cell,cft;
  for(cell=0;cell<ncell;cell++)
  {
    fputs("Coord:\t\t",file);
    fprintcoord(file,&grid[cell].coord);
    fputc('\n',file);
    if(config->river_routing)
    {
       fprintf(file,"dmass_lake:\t%g (dm3)\n"
                    "dfout:\t\t%g (dm3/d)\n"
                    "dmass_river:\t%g (dm3)\n"
                    "dmass_sum:\t%g (dm3)\n",
               grid[cell].discharge.dmass_lake,grid[cell].discharge.dfout,
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
      if(config->withlanduse!=NO_LANDUSE)
      {
        if(grid[cell].ml.sdate_fixed!=NULL)
        {
          fputs("CFT     ",file);
          for(cft=0;cft<ncft;cft++)
            fprintf(file,"%4d",cft);
          fputs("\n--------",file);
          for(cft=0;cft<ncft;cft++)
            fputs(" ---",file);
          fputs("\nfsdate  ",file);
          for(cft=0;cft<ncft;cft++)
            fprintf(file,"%4d",grid[cell].ml.sdate_fixed[cft]);
          fputs("\nirrsdate",file);
          for(cft=ncft;cft<2*ncft;cft++)
            fprintf(file,"%4d",grid[cell].ml.sdate_fixed[cft]);
          fputc('\n',file);
        }
        fprintf(file,"Cropfrac (rf/ir):\t%g\t%g\n",grid[cell].ml.cropfrac_rf,grid[cell].ml.cropfrac_ir);
        fprintcropdates(file,grid[cell].ml.cropdates,config->pftpar+npft,ncft);
      }
      fputs("Establ. stock for biomass:\n"
            "Type  rf     ir\n",file);
      fprintf(file,"Tree  %6.2f %6.2f (gC/m2)\n"
              "Grass %6.2f %6.2f (gC/m2)\n",
              grid[cell].balance.estab_storage_tree[0],
              grid[cell].balance.estab_storage_tree[1],
              grid[cell].balance.estab_storage_grass[0],
              grid[cell].balance.estab_storage_grass[1]);
      if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
        fprintignition(file,&grid[cell].ignition);
      fprintstandlist(file,grid[cell].standlist);
    }
  } /* of 'for(cell=...)' */
} /* of 'fprintcell' */
