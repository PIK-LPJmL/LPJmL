/**************************************************************************************/
/**                                                                                \n**/
/**             r  e  a  d  c  o  t  t  o  n  d  a  y  s  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads sowing and harvest dates for cotton                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readcottondays(Cell grid[],         /**< LPJ grid */
                    const Config *config /**< LPJ configuration */
                   )                     /** \return TRUE on error */
{
  int *vec;
  char *name;
  size_t filesize;
  int cell,version;
  size_t offset;
  String headername;
  Bool missing;
  Header header;
  int data;
  union
  {
    struct
    {
      Bool swap;
      FILE *file;
    } bin;
    Input_netcdf cdf;
  } file;
  if(config->sowing_cotton_rf_filename.fmt==CDF)
  {
    file.cdf=openinput_netcdf(&config->sowing_cotton_rf_filename,
                              NULL,0,config);
    if(file.cdf==NULL)
      return TRUE;
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(readintinput_netcdf(file.cdf,grid[cell].ml.sowing_day_cotton,&grid[cell].coord,&missing))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->sowing_cotton_rf_filename.name,cell+config->startgrid);
        closeinput_netcdf(file.cdf);
        return TRUE;
      }
      if(missing)
        grid[cell].ml.sowing_day_cotton[0]=-1;
    }
    closeinput_netcdf(file.cdf);
  }
  else
  {
    file.bin.file=openinputfile(&header,&file.bin.swap,&config->sowing_cotton_rf_filename,headername,&version,&offset,FALSE,config);
    if(file.bin.file==NULL)
      return TRUE;
    if(header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in rf sowing cotton data file '%s' is not 1.\n",
                header.nbands,config->sowing_cotton_rf_filename.name);
      fclose(file.bin.file);
      return TRUE;
    }
    if(isroot(*config) && config->sowing_cotton_rf_filename.fmt!=META)
    {
      filesize=getfilesizep(file.bin.file)-headersize(headername,version)-offset;
      if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
        fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",
                config->sowing_cotton_rf_filename.name);
    }
    if(fseek(file.bin.file,typesizes[header.datatype]*(config->startgrid-header.firstcell)+offset,SEEK_CUR))
    {
      name=getrealfilename(&config->sowing_cotton_rf_filename);
      fprintf(stderr,"ERROR150: Cannot seek rf sowing cotton data file '%s' to position %d.\n",
              name,config->startgrid);
      free(name);
      fclose(file.bin.file);
      return TRUE;
    }
    vec=newvec(int,config->ngridcell);
    if(vec==NULL)
    {
      printallocerr("vec");
      return TRUE;
    }
    if(readintvec(file.bin.file,vec,config->ngridcell,file.bin.swap,header.datatype))
    {
      name=getrealfilename(&config->sowing_cotton_rf_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s'.\n",name);
      free(name);
      free(vec);
      fclose(file.bin.file);
      return TRUE;
    }
    for(cell=0;cell<config->ngridcell;cell++)
      grid[cell].ml.sowing_day_cotton[0]=vec[cell];
    free(vec);
    fclose(file.bin.file);
  }
  if(config->harvest_cotton_rf_filename.fmt==CDF)
  {
    file.cdf=openinput_netcdf(&config->harvest_cotton_rf_filename,
                              NULL,0,config);
    if(file.cdf==NULL)
      return TRUE;
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(readintinput_netcdf(file.cdf,grid[cell].ml.growing_season_cotton,&grid[cell].coord,&missing))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->harvest_cotton_rf_filename.name,cell+config->startgrid);
        closeinput_netcdf(file.cdf);
        return TRUE;
      }
      if(missing)
        grid[cell].ml.growing_season_cotton[0]=-1;
    }
    closeinput_netcdf(file.cdf);
  }
  else
  {
    file.bin.file=openinputfile(&header,&file.bin.swap,&config->harvest_cotton_rf_filename,headername,&version,&offset,FALSE,config);
    if(file.bin.file==NULL)
      return TRUE;
    if(header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in rf harvest cotton data file '%s' is not 1.\n",
                header.nbands,config->harvest_cotton_rf_filename.name);
      fclose(file.bin.file);
      return TRUE;
    }
    if(isroot(*config) && config->harvest_cotton_rf_filename.fmt!=META)
    {
      filesize=getfilesizep(file.bin.file)-headersize(headername,version)-offset;
      if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
        fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",
                config->harvest_cotton_rf_filename.name);
    }
    if(fseek(file.bin.file,typesizes[header.datatype]*(config->startgrid-header.firstcell)+offset,SEEK_CUR))
    {
      name=getrealfilename(&config->harvest_cotton_rf_filename);
      fprintf(stderr,"ERROR150: Cannot seek rf harvest cotton data file '%s' to position %d.\n",
              name,config->startgrid);
      free(name);
      fclose(file.bin.file);
      return TRUE;
    }
    vec=newvec(int,config->ngridcell);
    if(vec==NULL)
    {
      printallocerr("vec");
      return TRUE;
    }
    if(readintvec(file.bin.file,vec,config->ngridcell,file.bin.swap,header.datatype))
    {
      name=getrealfilename(&config->harvest_cotton_rf_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s'.\n",name);
      free(name);
      free(vec);
      fclose(file.bin.file);
      return TRUE;
    }
    for(cell=0;cell<config->ngridcell;cell++)
      grid[cell].ml.growing_season_cotton[0]=vec[cell];
    free(vec);
    fclose(file.bin.file);
  }
  if(config->sowing_cotton_ir_filename.fmt==CDF)
  {
    file.cdf=openinput_netcdf(&config->sowing_cotton_ir_filename,
                              NULL,0,config);
    if(file.cdf==NULL)
      return TRUE;
    for(cell=0;cell<config->ngridcell;cell++)
    {
      //if(readintinput_netcdf(file.cdf,grid[cell].ml.sowing_day_cotton+1,&grid[cell].coord,&missing))
      if(readintinput_netcdf(file.cdf,&data,&grid[cell].coord,&missing))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->sowing_cotton_ir_filename.name,cell+config->startgrid);
        closeinput_netcdf(file.cdf);
        return TRUE;
      }
      if(missing)
        grid[cell].ml.sowing_day_cotton[1]=-1;
      else
        grid[cell].ml.sowing_day_cotton[1]=data;
    }
    closeinput_netcdf(file.cdf);
  }
  else
  {
    file.bin.file=openinputfile(&header,&file.bin.swap,&config->sowing_cotton_ir_filename,headername,&version,&offset,FALSE,config);
    if(file.bin.file==NULL)
      return TRUE;
    if(header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in ir sowing cotton data file '%s' is not 1.\n",
                header.nbands,config->sowing_cotton_ir_filename.name);
      fclose(file.bin.file);
      return TRUE;
    }
    if(isroot(*config) && config->sowing_cotton_ir_filename.fmt!=META)
    {
      filesize=getfilesizep(file.bin.file)-headersize(headername,version)-offset;
      if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
        fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",
                config->sowing_cotton_ir_filename.name);
    }
    if(fseek(file.bin.file,typesizes[header.datatype]*(config->startgrid-header.firstcell)+offset,SEEK_CUR))
    {
      name=getrealfilename(&config->sowing_cotton_ir_filename);
      fprintf(stderr,"ERROR150: Cannot seek ir sowing cotton data file '%s' to position %d.\n",
              name,config->startgrid);
      free(name);
      fclose(file.bin.file);
      return TRUE;
    }
    vec=newvec(int,config->ngridcell);
    if(vec==NULL)
    {
      printallocerr("vec");
      return TRUE;
    }
    if(readintvec(file.bin.file,vec,config->ngridcell,file.bin.swap,header.datatype))
    {
      name=getrealfilename(&config->sowing_cotton_ir_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s'.\n",name);
      free(name);
      free(vec);
      fclose(file.bin.file);
      return TRUE;
    }
    for(cell=0;cell<config->ngridcell;cell++)
      grid[cell].ml.sowing_day_cotton[1]=vec[cell];
    free(vec);
    fclose(file.bin.file);
  }
  if(config->harvest_cotton_ir_filename.fmt==CDF)
  {
    file.cdf=openinput_netcdf(&config->harvest_cotton_ir_filename,
                              NULL,0,config);
    if(file.cdf==NULL)
      return TRUE;
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(readintinput_netcdf(file.cdf,&data,&grid[cell].coord,&missing))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->harvest_cotton_ir_filename.name,cell+config->startgrid);
        closeinput_netcdf(file.cdf);
        return TRUE;
      }
      if(missing)
        grid[cell].ml.growing_season_cotton[1]=-1;
      else
        grid[cell].ml.growing_season_cotton[1]=data;
    }
    closeinput_netcdf(file.cdf);
  }
  else
  {
    file.bin.file=openinputfile(&header,&file.bin.swap,&config->harvest_cotton_ir_filename,headername,&version,&offset,FALSE,config);
    if(file.bin.file==NULL)
      return TRUE;
    if(header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in ir harvest cotton data file '%s' is not 1.\n",
                header.nbands,config->harvest_cotton_ir_filename.name);
      fclose(file.bin.file);
      return TRUE;
    }
    if(isroot(*config) && config->harvest_cotton_ir_filename.fmt!=META)
    {
      filesize=getfilesizep(file.bin.file)-headersize(headername,version)-offset;
      if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
        fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",
                config->harvest_cotton_ir_filename.name);
    }
    if(fseek(file.bin.file,typesizes[header.datatype]*(config->startgrid-header.firstcell)+offset,SEEK_CUR))
    {
      name=getrealfilename(&config->harvest_cotton_ir_filename);
      fprintf(stderr,"ERROR150: Cannot seek ir harvest cotton data file '%s' to position %d.\n",
              name,config->startgrid);
      free(name);
      fclose(file.bin.file);
      return TRUE;
    }
    vec=newvec(int,config->ngridcell);
    if(vec==NULL)
    {
      printallocerr("vec");
      return TRUE;
    }
    if(readintvec(file.bin.file,vec,config->ngridcell,file.bin.swap,header.datatype))
    {
      name=getrealfilename(&config->harvest_cotton_ir_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s'.\n",name);
      free(name);
      free(vec);
      fclose(file.bin.file);
      return TRUE;
    }
    for(cell=0;cell<config->ngridcell;cell++)
      grid[cell].ml.growing_season_cotton[1]=vec[cell];
    free(vec);
    fclose(file.bin.file);
  }

    //if(config->withlanduse==ALL_CROPS)
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(grid[cell].ml.sowing_day_cotton[0]==-1)
      grid[cell].ml.sowing_day_cotton[0]=(grid[cell].ml.sowing_day_cotton[1]==-1) ? 1 : grid[cell].ml.sowing_day_cotton[1];
    if(grid[cell].ml.sowing_day_cotton[1]==-1)
      grid[cell].ml.sowing_day_cotton[1]=(grid[cell].ml.sowing_day_cotton[0]==-1) ? 1 : grid[cell].ml.sowing_day_cotton[0];
    if(grid[cell].ml.growing_season_cotton[0]==-1)
      grid[cell].ml.growing_season_cotton[0]=(grid[cell].ml.growing_season_cotton[1]==-1) ? 364 : grid[cell].ml.growing_season_cotton[1];
    if(grid[cell].ml.growing_season_cotton[1]==-1)
      grid[cell].ml.growing_season_cotton[1]=(grid[cell].ml.growing_season_cotton[0]==-1) ? 364 : grid[cell].ml.growing_season_cotton[0];
  }
  return FALSE;
} /* of 'readcottondays' */
