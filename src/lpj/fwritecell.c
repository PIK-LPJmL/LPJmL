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

int fwritecell(FILE *file,        /**< File pointer of binary file */
               long long index[], /**< index vector to be calculated */
               const Cell grid[], /**< cell data array */
               int ncell,         /**< number of cells */
               int ncft,          /**< number of crop PFTs */
               int npft,          /**< number of PFTs */
               int sdate_option,  /**< sowing date option (0-2) */
               int crop_phu_option,    /**< phu computation option (0-1) */
               Bool river_routing /**< river routing (TRUE/FALSE) */
              )                   /** \return number of cells written */
{
  int cell;
  Byte b;
  for(cell=0;cell<ncell;cell++)
  {
    if(index!=NULL)
      index[cell]=ftell(file); /* store actual position in index vector */
    b=(Byte)grid[cell].skip;
    fwrite(&b,sizeof(b),1,file);
    if(river_routing)
    {
      if(fwrite(&grid[cell].discharge.dmass_lake,sizeof(Real),1,file)!=1)
        break;
      if(fwrite(&grid[cell].discharge.dfout,sizeof(Real),1,file)!=1)
        break;
      if(fwrite(&grid[cell].discharge.dmass_river,sizeof(Real),1,file)!=1)
        break;
      if(fwrite(&grid[cell].discharge.dmass_sum,sizeof(Real),1,file)!=1)
        break;
#ifdef COUPLING_WITH_FMS
      if(fwrite(&grid[cell].laketemp,sizeof(Real),1,file)!=1)
        break;
#endif
      if(fwritequeue(file,grid[cell].discharge.queue))
        break;
      b=(Byte)grid[cell].ml.dam;
      fwrite(&b,sizeof(b),1,file);
      if(grid[cell].ml.dam)
      {
        if(fwriteresdata(file,grid+cell))
          break;
      }
    }
    if(!grid[cell].skip)
    {
      if(fwrite(grid[cell].balance.estab_storage_tree,sizeof(Stocks),2,file)!=2)
        break;
      if(fwrite(grid[cell].balance.estab_storage_grass,sizeof(Stocks),2,file)!=2)
        break;
      if(fwrite(&grid[cell].balance.excess_water,sizeof(Real),1,file)!=1)
        break;
      if(fwrite(&grid[cell].discharge.waterdeficit,sizeof(Real),1,file)!=1)
        break;
      if(fwrite(grid[cell].gdd,sizeof(Real),npft, file)!=npft)
        break;
      if(fwritestandlist(file,grid[cell].standlist,npft+ncft)!=
         grid[cell].standlist->n)
        break;
      if(fwrite(&grid[cell].ml.cropfrac_rf,sizeof(Real),1,file)!=1)
        break;
      if(fwrite(&grid[cell].ml.cropfrac_ir,sizeof(Real),1,file)!=1)
        break;
      if(fwriteclimbuf(file,&grid[cell].climbuf))
        break;
      if(fwritecropdates(file,grid[cell].ml.cropdates,ncft))
        break;
      if(sdate_option>NO_FIXED_SDATE)
      {
        if(fwrite(grid[cell].ml.sdate_fixed,sizeof(int),2*ncft,file)!=2*ncft)
          break;
      }
      if(crop_phu_option>SEMISTATIC_CROP_PHU)
      {
        if(fwrite(grid[cell].ml.crop_phu_fixed,sizeof(Real),2*ncft,file)!=2*ncft)
          break;
      }
      if(fwrite(grid[cell].ml.sowing_month,sizeof(int),2*ncft,file)!=2*ncft)
        break;
      if(fwrite(grid[cell].ml.gs,sizeof(int),2*ncft,file)!=2*ncft)
        break;
      if(grid[cell].ml.landfrac!=NULL)
        fwritelandfrac(file,grid[cell].ml.landfrac,ncft);
      if(grid[cell].ml.fertilizer_nr!=NULL)
        fwritelandfrac(file,grid[cell].ml.fertilizer_nr,ncft);
    }
  } /* of 'for(cell=...)' */
  return cell;
} /* of 'fwritecell' */
