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
  Bool with_nitrogen;
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
  Infile kbf;
  Infile slope;
  Infile slope_min;
  Infile slope_max;
  Infile soilph;
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
          fprintf(stderr,"ERROR249: Invalid soilmap in '%s'.\n",config->soil_filename.name);
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
  if(openinputdata(&celldata->kbf,&config->kbf_filename,"Kbf",NULL,LPJ_FLOAT,0.001,config))
  {
    if (config->soil_filename.fmt == CDF)
      closecoord_netcdf(celldata->soil.cdf);
    else
    {
      closecoord(celldata->soil.bin.file_coord);
      fclose(celldata->soil.bin.file);
    }
    free(celldata);
    return NULL;
  }
  if(config->with_nitrogen)
  {
    celldata->with_nitrogen=TRUE;
    if(openinputdata(&celldata->soilph,&config->soilph_filename,"soilph",NULL,LPJ_SHORT,0.01,config))
    {
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      closeinput(&celldata->kbf);
      free(celldata);
      return NULL;
    }
  }
  else
    celldata->with_nitrogen=FALSE;
  if(openinputdata(&celldata->slope,&config->slope_filename,"slope",NULL,LPJ_FLOAT,1,config))
  {
    if(config->soil_filename.fmt==CDF)
      closecoord_netcdf(celldata->soil.cdf);
    else
    {
      closecoord(celldata->soil.bin.file_coord);
      fclose(celldata->soil.bin.file);
    }
    closeinput(&celldata->kbf);
    if(config->with_nitrogen)
      closeinput(&celldata->soilph);
    free(celldata);
    return NULL;
  }
  if(openinputdata(&celldata->slope_min,&config->slope_min_filename,"slope_min",NULL,LPJ_FLOAT,1,config))
  {
    if(config->soil_filename.fmt==CDF)
      closecoord_netcdf(celldata->soil.cdf);
    else
    {
      closecoord(celldata->soil.bin.file_coord);
      fclose(celldata->soil.bin.file);
    }
    closeinput(&celldata->kbf);
    if(config->with_nitrogen)
      closeinput(&celldata->soilph);
    closeinput(&celldata->slope);
    free(celldata);
    return NULL;
  }
  if(openinputdata(&celldata->slope_max,&config->slope_max_filename,"slope_max",NULL,LPJ_FLOAT,1,config))
  {
    if(config->soil_filename.fmt==CDF)
      closecoord_netcdf(celldata->soil.cdf);
    else
    {
      closecoord(celldata->soil.bin.file_coord);
      fclose(celldata->soil.bin.file);
    }
    closeinput(&celldata->kbf);
    if(config->with_nitrogen)
      closeinput(&celldata->soilph);
    closeinput(&celldata->slope);
    closeinput(&celldata->slope_min);
    free(celldata);
    return NULL;
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

Bool readcelldata(Celldata celldata, /**< pointer to celldata */
                  unsigned int *soilcode,     /**< soil code */
                  Cell *cell,
                  int cell_id,       /**< cell index */
                  Config *config     /**< LPJmL configuration */
                 )                   /** \return TRUE on error */
{
  char *name;
  if(celldata->soil_fmt==CDF)
  {
    if(readcoord_netcdf(celldata->soil.cdf,&cell->coord,&config->resolution,soilcode))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->soil_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readcoord(celldata->soil.bin.file_coord,&cell->coord,&config->resolution))
    {
      name=getrealfilename(&config->coord_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              name,cell_id+config->startgrid);
      free(name);
      return TRUE;
    }
    /* read soilcode from file */

    if(freadsoilcode(celldata->soil.bin.file,soilcode,
                     celldata->soil.bin.swap,celldata->soil.bin.type))
    {
      name=getrealfilename(&config->soil_filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              name,cell_id+config->startgrid);
      free(name);
      config->ngridcell=cell_id;
      return TRUE;
    }
  }
  /* read kbf value*/
  if(readinputdata(&celldata->kbf,&cell->kbf,&cell->coord,cell_id+config->startgrid,&config->kbf_filename))
    return TRUE;
  /* read slope value and calculate Haggard Beta value*/
  if(readinputdata(&celldata->slope,&cell->slope,&cell->coord,cell_id+config->startgrid,&config->slope_filename))
    return TRUE;

  cell->Hag_beta=min(1,(0.06*log(cell->slope+0.1)+0.22)/0.43);
  if(cell->Hag_beta>1)
    fail(HAG_BETA_ERR,FALSE,"HAG_BETA greater than 1 HAG_BETA= %.2f  slope= %.2f lat=e= %.2f lon=e= %.2f\n",
         cell->Hag_beta,cell->slope,cell->coord.lat,cell->coord.lon);
  if(readinputdata(&celldata->slope_min,&cell->slope_min,&cell->coord,cell_id+config->startgrid,&config->slope_min_filename))
    return TRUE;
  if(readinputdata(&celldata->slope_max,&cell->slope_max,&cell->coord,cell_id+config->startgrid,&config->slope_max_filename))
    return TRUE;
  if(*soilcode>=config->soilmap_size)
  {
    name=getrealfilename(&config->soil_filename);
    fprintf(stderr,"ERROR250: Invalid soilcode %u of cell %d in '%s', must be in [0,%d].\n",
            *soilcode,cell_id+config->startgrid,name,config->soilmap_size-1);
    free(name);
    return TRUE;
  }
  if(config->with_nitrogen)
  {
    if(readinputdata(&celldata->soilph,&cell->soilph,&cell->coord,cell_id+config->startgrid,&config->soilph_filename))
      return TRUE;
  }
  return FALSE;
} /* of 'readcelldata' */

void closecelldata(Celldata celldata /**< pointer to celldata */
                  )
{
  if(celldata->soil_fmt==CDF)
    closecoord_netcdf(celldata->soil.cdf);
  else
  {
    closecoord(celldata->soil.bin.file_coord);
    fclose(celldata->soil.bin.file);
  }
  closeinput(&celldata->kbf);
  if(celldata->with_nitrogen)
    closeinput(&celldata->soilph);
  closeinput(&celldata->slope);
  closeinput(&celldata->slope_min);
  closeinput(&celldata->slope_max);
  free(celldata);
} /* of 'closecelldata' */
