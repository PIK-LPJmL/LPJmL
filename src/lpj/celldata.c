/**************************************************************************************/
/**                                                                                \n**/
/**                       c  e  l  l  d  a  t  a  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions opens and reads soil data, coordinates                           \n**/
/**     for each cell. NetCDF input is supported if compiled with                  \n**/
/**     -DUSE_NETCDF flag.                                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct celldata
{
  int soil_fmt;
  union
  {
    struct
    {
      Bool swap;
      size_t offset;
      FILE *file;
      Type type;
      Coordfile file_coord;
    } bin;
    Coord_netcdf cdf;
  } soil;
  Infile lakes;
  Infile soilph;
  Infile landfrac;
};

Celldata opencelldata(Config *config /**< LPJmL configuration */
                     )               /** \return pointer to cell data or NULL */
{
  Celldata celldata;
  List *map;
  int *soilmap;
  float lon,lat;
  celldata=new(struct celldata);
  if(celldata==NULL)
    return NULL;
  celldata->soil_fmt=config->soil_filename.fmt;
  if(config->soil_filename.fmt==CDF)
  {
    celldata->soil.cdf=opencoord_netcdf(config->soil_filename.name,
                                        config->soil_filename.var,
                                        isroot(*config));
    if(celldata->soil.cdf==NULL)
    {
      free(celldata);
      return NULL;
    }
    getresolution_netcdf(celldata->soil.cdf,&config->resolution);
  }
  else
  {
    celldata->soil.bin.file_coord=opencoord(&config->coord_filename,isroot(*config));
    if(celldata->soil.bin.file_coord==NULL)
    {
      free(celldata);
      return NULL;
    }
    getcellsizecoord(&lon,&lat,celldata->soil.bin.file_coord);
    config->resolution.lon=lon;
    config->resolution.lat=lat;
    if(config->nall>numcoord(celldata->soil.bin.file_coord))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR249: Number of cells in grid file '%s'=%d less than %d.\n",
                config->coord_filename.name,numcoord(celldata->soil.bin.file_coord),config->nall);
      free(celldata);
      return NULL;
    }

    /* Open soiltype file */
    celldata->soil.bin.file=fopensoilcode(&config->soil_filename,
                                          &map,
                                          &celldata->soil.bin.swap,
                                          &celldata->soil.bin.offset,
                                          &celldata->soil.bin.type,config->nsoil,
                                          isroot(*config));
    if(celldata->soil.bin.file==NULL)
    {
      closecoord(celldata->soil.bin.file_coord);
      free(celldata);
      return NULL;
    }
    if(map!=NULL)
    {
      soilmap=getsoilmap(map,config);
      if(soilmap==NULL)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR250: Invalid soilmap in '%s'.\n",config->soil_filename.name);
      }
      else
      {
        if(isroot(*config) && config->soilmap!=NULL)
           cmpsoilmap(soilmap,getlistlen(map),config);
        free(config->soilmap);
        config->soilmap=soilmap;
        config->soilmap_size=getlistlen(map);
      }
      freemap(map);
    }
    if(config->soilmap==NULL)
    {
      config->soilmap=defaultsoilmap(&config->soilmap_size,config);
      if(config->soilmap==NULL)
        return NULL;
    }
  }
  if(config->with_lakes)
  {
    /* Open file for lake fraction */
    if(openinputdata(&celldata->lakes,&config->lakes_filename,"lakes","1",LPJ_BYTE,0.01,config))
    {
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      free(celldata);
      return NULL;
    }
  }
  if(config->with_nitrogen)
  {
    if(openinputdata(&celldata->soilph,&config->soilph_filename,"soilph",NULL,LPJ_SHORT,0.01,config))
    {
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      if(config->with_lakes)
        closeinput(&celldata->lakes);
      free(celldata);
      return NULL;
    }
  }
  if(config->landfrac_from_file)
  {
    if(openinputdata(&celldata->landfrac,&config->landfrac_filename,"landfrac","1",LPJ_SHORT,0.01,config))
    {
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      if(config->with_lakes)
        closeinput(&celldata->lakes);
      if(config->with_nitrogen)
        closeinput(&celldata->soilph);
      free(celldata);
      return NULL;
    }
  }
  return celldata;
} /* of 'opencelldata' */

Bool seekcelldata(Celldata celldata, /**< pointer to celldata */
                  int startgrid      /**< index of first cell */
                 )                   /** \return TRUE on error */
{
  if(celldata->soil_fmt==CDF)
  {
    if(seekcoord_netcdf(celldata->soil.cdf,startgrid))
    {
      fprintf(stderr,
              "ERROR109: Cannot seek in coordinate file to position %d.\n",
              startgrid);
      return TRUE;
    }
  }
  else
  {
    if(seekcoord(celldata->soil.bin.file_coord,startgrid))
    {
      /* seeking to position of first grid cell failed */
      fprintf(stderr,
              "ERROR109: Cannot seek in coordinate file to position %d.\n",
              startgrid);
      return TRUE;
    }
    if(seeksoilcode(celldata->soil.bin.file,startgrid,celldata->soil.bin.offset,celldata->soil.bin.type))
    {
      /* seeking to position of first grid cell failed */
      fprintf(stderr,"ERROR107: Cannot seek in soilcode file to position %d.\n",
              startgrid);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'seekcelldata' */

Bool readcelldata(Celldata celldata,      /**< pointer to celldata */
                  Cell *grid,             /**< pointer to grid cell */
                  unsigned int *soilcode, /**< soil code */
                  int cell,               /**< cell index */
                  Config *config          /**< LPJmL configuration */
                 )                        /** \return TRUE on error */
{
  char *name;
  if(celldata->soil_fmt==CDF)
  {
    if(readcoord_netcdf(celldata->soil.cdf,&grid->coord,&config->resolution,soilcode))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->soil_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readcoord(celldata->soil.bin.file_coord,&grid->coord,&config->resolution))
    {
      name=getrealfilename(&config->coord_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              name,cell+config->startgrid);
      free(name);
      return TRUE;
    }
    /* read soilcode from file */

    if(freadsoilcode(celldata->soil.bin.file,soilcode,
                     celldata->soil.bin.swap,celldata->soil.bin.type))
    {
      name=getrealfilename(&config->soil_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              name,cell+config->startgrid);
      config->ngridcell=cell;
      free(name);
      return TRUE;
    }
  }
  if(*soilcode>=config->soilmap_size)
  {
    name=getrealfilename(&config->soil_filename);
    fprintf(stderr,"ERROR250: Invalid soilcode %u of cell %d in '%s', must be in [0,%d].\n",
            *soilcode,cell+config->startgrid,name,config->soilmap_size-1);
    free(name);
    return TRUE;
  }
  if(config->with_nitrogen)
  {
    if(readinputdata(&celldata->soilph,&grid->soilph,&grid->coord,cell+config->startgrid,&config->soilph_filename))
      return TRUE;
  }
  if(config->landfrac_from_file)
  {
    if(readinputdata(&celldata->landfrac,&grid->landfrac,&grid->coord,cell+config->startgrid,&config->landfrac_filename))
      return TRUE;
    if(grid->landfrac==0)
    {
      fprintf(stderr,"WARNING034: Land fraction of cell %d is zero, set to %g.\n",
              cell+config->startgrid,tinyfrac);
      grid->landfrac=tinyfrac;
    }
    else if(grid->landfrac>1 || grid->landfrac<0)
    {
      fprintf(stderr,"ERROR257: Land fraction of cell %d=%g not in (0,1].\n",
              cell+config->startgrid,grid->landfrac);
      return TRUE;
    }
    grid->coord.area*=grid->landfrac;
  }
  else
    grid->landfrac=1;
  if(config->with_lakes)
  {
    if(readinputdata(&celldata->lakes,&grid->lakefrac,&grid->coord,cell+config->startgrid,&config->lakes_filename))
      return TRUE;
    /* rescale to land fraction */
    grid->lakefrac/=grid->landfrac;
    if(grid->lakefrac>1)
      fprintf(stderr,"WARNING035: Lake fraction in cell %d=%g greater than one, set to one.\n",cell+config->startgrid,grid->lakefrac);
    if(grid->lakefrac>1-epsilon)
      grid->lakefrac=1;
  }
  else
    grid->lakefrac=0;
  return FALSE;
} /* of 'readcelldata' */

void closecelldata(Celldata celldata,   /**< pointer to celldata */
                   const Config *config /**< LPJmL configuration */
                  )
{
  if(celldata->soil_fmt==CDF)
    closecoord_netcdf(celldata->soil.cdf);
  else
  {
    closecoord(celldata->soil.bin.file_coord);
    fclose(celldata->soil.bin.file);
  }
  if(config->with_nitrogen)
    closeinput(&celldata->soilph);
  if(config->landfrac_from_file)
    closeinput(&celldata->landfrac);
  if(config->with_lakes)
    closeinput(&celldata->lakes);
  free(celldata);
} /* of 'closecelldata' */
