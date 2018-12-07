/**************************************************************************************/
/**                                                                                \n**/
/**                 w  a  t  e  r  u  s  e  f  c  n  s  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function implements wateruse functions                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct wateruse
{
  Climatefile file;
};               /* definition of opaque datatype Wateruse */

Wateruse initwateruse(const Config *config /**< LPJmL configuration */
                     )
{
  Wateruse wateruse;
  Header header;
  String headername;
  int version;
  size_t offset;
  wateruse=new(struct wateruse);
  if(wateruse==NULL)
  {
    printallocerr("wateruse");
    return NULL;
  }
  wateruse->file.fmt=config->wateruse_filename.fmt;
  if(config->wateruse_filename.fmt==CDF)
  { 
    if(opendata_netcdf(&wateruse->file,config->wateruse_filename.name,config->wateruse_filename.var,NULL,config))
    {
      free(wateruse);
      return NULL;
    }
    if(wateruse->file.var_len!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in wateruse file '%s' is not 1.\n",
                (int)wateruse->file.var_len,config->wateruse_filename.name);

      closeclimate_netcdf(&wateruse->file,isroot(*config));
      free(wateruse);
      return NULL;
    }
  }
  else
  {
    if((wateruse->file.file=openinputfile(&header,&wateruse->file.swap,
                                          &config->wateruse_filename,
                                          headername,
                                          &version,&offset,config))==NULL)
    {
      free(wateruse);
      return NULL;
    }
    wateruse->file.firstyear=header.firstyear;
    wateruse->file.nyear=header.nyear;
    if(config->wateruse_filename.fmt==RAW)
    {
      wateruse->file.datatype=LPJ_INT;
      wateruse->file.offset=sizeof(int)*config->startgrid;
    }
    else
    {
      if(version>2)
        wateruse->file.datatype=header.datatype;
      else
        wateruse->file.datatype=LPJ_INT;
      wateruse->file.offset=headersize(headername,version)+typesizes[wateruse->file.datatype]*(config->startgrid-header.firstcell)+offset;
    }
    if(header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in wateruse file '%s' is not 1.\n",
                header.nbands,config->wateruse_filename.name);
      fclose(wateruse->file.file);
      free(wateruse);
      return NULL;
    }
    wateruse->file.size=header.ncell*typesizes[wateruse->file.datatype];
    wateruse->file.scalar=(version<=1) ? 1000 : header.scalar;
  }
  return wateruse;
} /* of 'initwateruse' */

Bool getwateruse(Wateruse wateruse,   /**< Pointer to wateruse data */
                 Cell grid[],         /**< cell grid */
                 int year,            /**< year of wateruse data (AD) */
                 const Config *config /**< LPJ configuration */
                )                     /** \return TRUE on error */
{
  int cell;
  Real *data;
  if(config->wateruse==ALL_WATERUSE)
  {
    /* first and last wateruse data is used outside available wateruse data */
    if(year<=wateruse->file.firstyear)
      year=wateruse->file.firstyear;
    else if(year>=wateruse->file.firstyear+wateruse->file.nyear)
      year=wateruse->file.firstyear+wateruse->file.nyear-1;
  }
  if(year>=wateruse->file.firstyear && year<wateruse->file.firstyear+wateruse->file.nyear)
  {
    if(wateruse->file.fmt==CDF)
    {
      data=newvec(Real,config->ngridcell);
      if(data==NULL)
      {
        printallocerr("data");
        return TRUE;
      }
      if(readdata_netcdf(&wateruse->file,data,grid,year,config))
      {
        free(data);
        return TRUE;
      }
      for(cell=0;cell<config->ngridcell;cell++)
        grid[cell].discharge.wateruse=data[cell];
      free(data);
    }
    else
    {
      if(fseek(wateruse->file.file,
               wateruse->file.offset+wateruse->file.size*(year-wateruse->file.firstyear),
               SEEK_SET))
      {
        fprintf(stderr,"ERROR150: Cannot seek file to year %d in wateruse().\n",year);
        return TRUE;
      } 
      data=newvec(Real,config->ngridcell);
      if(data==NULL)
      {
        printallocerr("data");
        return TRUE;
      }
      if(readrealvec(wateruse->file.file,data,0,wateruse->file.scalar,config->ngridcell,wateruse->file.swap,wateruse->file.datatype))
      {
        fprintf(stderr,"ERROR151: Cannot read wateruse for year %d.\n",year);
        free(data);
        return TRUE;
      } 
      for(cell=0;cell<config->ngridcell;cell++)
        grid[cell].discharge.wateruse=data[cell];
      free(data);
    }
  }
  else 
    /* no wateruse data available for year, set all to zero */
    for(cell=0;cell<config->ngridcell;cell++)
      grid[cell].discharge.wateruse=0;
  return FALSE;
} /* of 'getwateruse' */

void freewateruse(Wateruse wateruse, /**< pointer to wateruse data */
                  Bool isroot        /**< task is root task (TRUE/FALSE) */
                 )
{
  if(wateruse!=NULL)
  {
    if(wateruse->file.fmt==CDF)
      closeclimate_netcdf(&wateruse->file,isroot);
    else
      fclose(wateruse->file.file);
    free(wateruse);
  }
} /* of 'freewateruse' */
