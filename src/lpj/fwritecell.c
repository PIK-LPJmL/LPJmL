/**************************************************************************************/
/**                                                                                \n**/
/**             f  w  r  i  t  e  c  e  l  l  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes cell array and calculates index vector                     \n**/
/**     Index vector is used at reading to seek to specific LPJ cells              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int fwritecell(Bstruct file,        /**< File pointer of binary file */
               long long index[], /**< index vector to be calculated */
               const Cell grid[], /**< cell data array */
               int ncell,         /**< number of cells */
               int ncft,          /**< number of crop PFTs */
               int npft,          /**< number of PFTs */
               Bool ischeckpoint,
               const Config *config /**< LPJml configuration  */
              )                   /** \return number of cells written */
{
  int cell;
  for(cell=0;cell<ncell;cell++)
  {
    if(index!=NULL)
      index[cell]=bstruct_getarrayindex(file); /* store actual position in index vector */
    bstruct_writestruct(file,NULL);
    bstruct_writebool(file,"skip",grid[cell].skip);
    fwriteseed(file,"seed",grid[cell].seed);
    if(bstruct_writereal(file,"dmass_lake",grid[cell].discharge.dmass_lake))
      break;
    if(config->river_routing)
    {
#ifdef IMAGE
    if(bstruct_writereal(file,"dmass_gw",grid[cell].discharge.dmass_gw))
      break;
#endif
      if(bstruct_writereal(file,"dfout",grid[cell].discharge.dfout))
        break;
      if(bstruct_writereal(file,"dmass_river",grid[cell].discharge.dmass_river))
        break;
      if(bstruct_writereal(file,"dmass_sum",grid[cell].discharge.dmass_sum))
        break;
#ifdef COUPLING_WITH_FMS
      if(bstruct_writereal(file,"laketemp",&grid[cell].laketemp))
        break;
#endif
      if(fwritequeue(file,"queue",grid[cell].discharge.queue))
        break;
      bstruct_writebool(file,"dam",grid[cell].ml.dam);
      if(grid[cell].ml.dam)
      {
        if(fwriteresdata(file,"resdata",grid+cell))
          break;
      }
    }
    if(!grid[cell].skip)
    {
      if(fwritestocksarray(file,"estab_storage_tree",grid[cell].balance.estab_storage_tree,2))
        break;
      if(fwritestocksarray(file,"estab_storage_grass",grid[cell].balance.estab_storage_grass,2))
        break;
      if(fwriteignition(file,"ignition",&grid[cell].ignition))
        break;
      if(bstruct_writereal(file,"excess_water",grid[cell].balance.excess_water))
        break;
      if(bstruct_writereal(file,"waterdeficit",grid[cell].discharge.waterdeficit))
        break;
      if(bstruct_writerealarray(file,"gdd",grid[cell].gdd,npft))
        break;
      if(fwritestandlist(file,"standlist",grid[cell].standlist,npft+ncft))
        break;
      if(bstruct_writereal(file,"cropfrac_rf",grid[cell].ml.cropfrac_rf))
        break;
      if(bstruct_writereal(file,"cropfrac_ir",grid[cell].ml.cropfrac_ir))
        break;
      if(fwriteclimbuf(file,"climbuf",&grid[cell].climbuf,ncft))
        break;
      if(fwritecropdates(file,"cropdates",grid[cell].ml.cropdates,ncft))
        break;
      if(config->sdate_option>NO_FIXED_SDATE)
      {
        if(bstruct_writeintarray(file,"sdate_fixed",grid[cell].ml.sdate_fixed,2*ncft))
          break;
      }
      if(config->crop_phu_option>=PRESCRIBED_CROP_PHU)
      {
        if(bstruct_writerealarray(file,"crop_phu_fixed",grid[cell].ml.crop_phu_fixed,2*ncft))
          break;
      }
      if(bstruct_writeintarray(file,"sowing_month",grid[cell].ml.sowing_month,2*ncft))
        break;
      if(bstruct_writeintarray(file,"gs",grid[cell].ml.gs,2*ncft))
        break;
      if(grid[cell].ml.landfrac!=NULL)
      {
        fwritelandfrac(file,"landfrac",grid[cell].ml.landfrac,ncft,config->nagtree);
#ifndef IMAGE
        if(fwritepool(file,"product",&grid[cell].ml.product))
          break;
#endif
      }
      if(grid[cell].ml.fertilizer_nr!=NULL)
        fwritelandfrac(file,"fertilizer_nr",grid[cell].ml.fertilizer_nr,ncft,config->nagtree);
      if(ischeckpoint && config->n_out)
       fwriteoutputdata(file,"outputdata",&grid[cell].output,config);
    }
    bstruct_writeendstruct(file);
  } /* of 'for(cell=...)' */
  return cell;
} /* of 'fwritecell' */
