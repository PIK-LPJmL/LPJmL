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

typedef struct
{
  int fmt;
  union
  {
    struct
    {
      Bool swap;
      size_t offset;
      float scalar;
      Type type;
      FILE *file;
    } bin;
    Input_netcdf cdf;
  } file;
} Inputfile;

struct celldata
{
  Bool with_nitrogen;
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
  int soil_fmt;
  Inputfile kbf;
  Inputfile slope;
  Inputfile slope_min;
  Inputfile slope_max;
  Inputfile soilph;
};

static void closefile(Inputfile *input)
{
   if(input->fmt==CDF)
     closeinput_netcdf(input->file.cdf);
   else
     fclose(input->file.bin.file);
}

Celldata opencelldata(Config *config /**< LPJmL configuration */
                     )               /** \return pointer to cell data or NULL */
{
  Celldata celldata;
  Header header;
  String headername;
  int version;
  float lon,lat;
  size_t filesize;
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
  }
  celldata->kbf.fmt = config->kbf_filename.fmt;
  if (config->kbf_filename.fmt == CDF)
  {
    celldata->kbf.file.cdf = openinput_netcdf(&config->kbf_filename,
                                              NULL, 0, config);
    if (celldata->kbf.file.cdf == NULL)
    {
      if (isroot(*config))
        printfopenerr(config->kbf_filename.name);
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
  }
  else
  {

    celldata->kbf.file.bin.file = openinputfile(&header, &celldata->kbf.file.bin.swap,
                                           &config->kbf_filename,
                                           headername,
                                           &version,&celldata->kbf.file.bin.offset,FALSE,config);

    if (celldata->kbf.file.bin.file == NULL)
    {
      if (isroot(*config))
        printfopenerr(config->kbf_filename.name);
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
    celldata->kbf.file.bin.scalar = (version == 1) ? 0.001 : header.scalar;
    if(version<3)
      celldata->kbf.file.bin.type=LPJ_FLOAT;
    else
      celldata->kbf.file.bin.type=header.datatype;
  }
  if(config->with_nitrogen)
  {
    celldata->soilph.fmt=config->soilph_filename.fmt;
    celldata->with_nitrogen=TRUE;
    if(config->soilph_filename.fmt==CDF)
    {
      celldata->soilph.file.cdf=openinput_netcdf(&config->soilph_filename,
                                            NULL,0,config);
      if(celldata->soilph.file.cdf==NULL)
      {
        if(config->soil_filename.fmt==CDF)
          closecoord_netcdf(celldata->soil.cdf);
        else
        {
          closecoord(celldata->soil.bin.file_coord);
          fclose(celldata->soil.bin.file);
        }
        closefile(&celldata->kbf);
        free(celldata);
        return NULL;
      }
    }
    else
    {
      celldata->soilph.file.bin.file=openinputfile(&header,&celldata->soilph.file.bin.swap,
                                              &config->soilph_filename,
                                              headername,
                                              &version,&celldata->soilph.file.bin.offset,FALSE,config);
      if(celldata->soilph.file.bin.file==NULL)
      {
        if(config->soil_filename.fmt==CDF)
          closecoord_netcdf(celldata->soil.cdf);
        else
        {
          closecoord(celldata->soil.bin.file_coord);
          fclose(celldata->soil.bin.file);
        }
        closefile(&celldata->kbf);
        free(celldata);
        return NULL;
      }
      if(config->soilph_filename.fmt==RAW)
        header.nyear=1;
      if(version<2)
        celldata->soilph.file.bin.scalar=0.01;
      else
        celldata->soilph.file.bin.scalar=header.scalar;
      if(header.nbands!=1)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR218: Invalid number of bands %d in '%s', must be 1.\n",
                  header.nbands,config->soilph_filename.name);
        free(celldata);
        return NULL;
      }
      if(isroot(*config) && config->soilph_filename.fmt!=META)
      {
         filesize=getfilesizep(celldata->soilph.file.bin.file)-headersize(headername,version)-celldata->soilph.file.bin.offset;
         if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
           fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->soilph_filename.name);
      }
      celldata->soilph.file.bin.type=header.datatype;
    }

  }
  else
    celldata->with_nitrogen=FALSE;
  celldata->slope.fmt=config->slope_filename.fmt;
  if(config->slope_filename.fmt==CDF)
  {
    celldata->slope.file.cdf=openinput_netcdf(&config->slope_filename,
                                              NULL,0,config);
    if(celldata->slope.file.cdf==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->slope_filename.name);
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      closefile(&celldata->kbf);
      if(config->with_nitrogen)
        closefile(&celldata->soilph);
      free(celldata);
      return NULL;
    }
  }
  else
  {
    celldata->slope.file.bin.file=openinputfile(&header,&celldata->slope.file.bin.swap,
                                           &config->slope_filename,
                                           headername,
                                           &version,&celldata->slope.file.bin.offset,FALSE,config);

    if(celldata->slope.file.bin.file==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->slope_filename.name);
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      closefile(&celldata->kbf);
      if(config->with_nitrogen)
        closefile(&celldata->soilph);
      free(celldata);
      return NULL;
    }
    celldata->slope.file.bin.scalar=(version==1) ? 1 : header.scalar;
    if(version<3)
      celldata->slope.file.bin.type=LPJ_FLOAT;
    else
      celldata->slope.file.bin.type=header.datatype;
  }
  celldata->slope_min.fmt=config->slope_min_filename.fmt;
  if(config->slope_min_filename.fmt==CDF)
  {
    celldata->slope_min.file.cdf=openinput_netcdf(&config->slope_min_filename,
                                             NULL,0,config);
    if(celldata->slope_min.file.cdf==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->slope_min_filename.name);
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      closefile(&celldata->kbf);
      if(config->with_nitrogen)
        closefile(&celldata->soilph);
      closefile(&celldata->slope);
      free(celldata);
      return NULL;
    }
  }
  else
  {
    celldata->slope_min.file.bin.file=openinputfile(&header,&celldata->slope_min.file.bin.swap,
                                               &config->slope_min_filename,
                                               headername,
                                               &version,&celldata->slope_min.file.bin.offset,
                                               FALSE,config);
    if(celldata->slope_min.file.bin.file==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->slope_min_filename.name);
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      closefile(&celldata->kbf);
      if(config->with_nitrogen)
        closefile(&celldata->soilph);
      closefile(&celldata->slope);
      free(celldata);
      return NULL;
    }
    celldata->slope_min.file.bin.scalar=(version==1) ? 1 : header.scalar;
    if(version<3)
      celldata->slope_min.file.bin.type=LPJ_FLOAT;
    else
      celldata->slope_min.file.bin.type=header.datatype;
  }
  celldata->slope_max.fmt=config->slope_max_filename.fmt;
  if(config->slope_max_filename.fmt==CDF)
  {
    celldata->slope_max.file.cdf=openinput_netcdf(&config->slope_max_filename,
                                                  NULL,0,config);
    if(celldata->slope_max.file.cdf==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->slope_max_filename.name);
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      closefile(&celldata->kbf);
      if(config->with_nitrogen)
        closefile(&celldata->soilph);
      closefile(&celldata->slope);
      closefile(&celldata->slope_min);
      free(celldata);
      return NULL;
    }
  }
  else
  {
    celldata->slope_max.file.bin.file=openinputfile(&header,&celldata->slope_max.file.bin.swap,
                                               &config->slope_max_filename,
                                               headername,&version,&celldata->slope_max.file.bin.offset,FALSE,config);


    if(celldata->slope_max.file.bin.file==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->slope_max_filename.name);
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      closefile(&celldata->kbf);
      if(config->with_nitrogen)
        closefile(&celldata->soilph);
      closefile(&celldata->slope);
      closefile(&celldata->slope_min);
      free(celldata);
      return NULL;
    }
    celldata->slope_max.file.bin.scalar=(version==1) ? 1 : header.scalar;
    if(version<3)
      celldata->slope_max.file.bin.type=LPJ_FLOAT;
    else
      celldata->slope_max.file.bin.type=header.datatype;
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
  if(celldata->kbf.fmt!=CDF &&
     fseek(celldata->kbf.file.bin.file,startgrid*typesizes[celldata->kbf.file.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in kbf file to position %d.\n",
            startgrid);
    return TRUE;
  }
  if(celldata->slope.fmt!=CDF &&
     fseek(celldata->slope.file.bin.file,startgrid*typesizes[celldata->slope.file.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in slope file to position %d.\n",
            startgrid);
    return TRUE;
  }
  if(celldata->slope_min.fmt!=CDF &&
     fseek(celldata->slope_min.file.bin.file,startgrid*typesizes[celldata->slope_min.file.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in slope_min file to position %d.\n",
            startgrid);
    return TRUE;
  }
  if(celldata->slope_max.fmt!=CDF &&
     fseek(celldata->slope_max.file.bin.file,startgrid*typesizes[celldata->slope_max.file.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in slope_max file to position %d.\n",
            startgrid);
    return TRUE;
  }

  if(celldata->with_nitrogen)
  {
    if(celldata->soilph.fmt!=CDF &&
       fseek(celldata->soilph.file.bin.file,startgrid*typesizes[celldata->soilph.file.bin.type],SEEK_CUR))
    {
      /* seeking to position of first grid cell failed */
      fprintf(stderr,
              "ERROR107: Cannot seek in soilpH file to position %d.\n",
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
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->coord_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
    /* read soilcode from file */

    if(freadsoilcode(celldata->soil.bin.file,soilcode,
                     celldata->soil.bin.swap,celldata->soil.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->soil_filename.name,cell_id+config->startgrid);
      config->ngridcell=cell_id;
      return TRUE;
    }
  }
  /* read kbf value*/
  if(celldata->kbf.fmt==CDF)
  {
    if(readinput_netcdf(celldata->kbf.file.cdf,&cell->kbf,&cell->coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->kbf_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->kbf.file.bin.file,&cell->kbf,0,celldata->kbf.file.bin.scalar,1,celldata->kbf.file.bin.swap,celldata->kbf.file.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->kbf_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  /* read slope value and calculate Haggard Beta value*/
  if(celldata->slope.fmt==CDF)
  {
    if(readinput_netcdf(celldata->slope.file.cdf,&cell->slope,&cell->coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->slope.file.bin.file,&cell->slope,0,celldata->slope.file.bin.scalar,1,celldata->slope.file.bin.swap,celldata->slope.file.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }

  cell->Hag_beta=min(1,(0.06*log(cell->slope+0.1)+0.22)/0.43);
  if(cell->Hag_beta>1)
    fail(HAG_BETA_ERR,FALSE,"HAG_BETA greater than 1 HAG_BETA= %.2f  slope= %.2f lat=e= %.2f lon=e= %.2f\n",
         cell->Hag_beta,cell->slope,cell->coord.lat,cell->coord.lon);

  if(celldata->slope_min.fmt==CDF)
  {
    if(readinput_netcdf(celldata->slope_min.file.cdf,&cell->slope_min,&cell->coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_min_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->slope_min.file.bin.file,&cell->slope_min,0,celldata->slope_min.file.bin.scalar,1,celldata->slope_min.file.bin.swap,celldata->slope_min.file.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_min_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  if(celldata->slope_max.fmt==CDF)
  {
    if(readinput_netcdf(celldata->slope_max.file.cdf,&cell->slope_max,&cell->coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_max_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->slope_max.file.bin.file,&cell->slope_max,0,celldata->slope_max.file.bin.scalar,1,celldata->slope_max.file.bin.swap,celldata->slope_max.file.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_max_filename.name,cell_id+config->startgrid);
      return TRUE;
    }
  }
  if(*soilcode>=config->soilmap_size)
  {
    fprintf(stderr,"ERROR250: Invalid soilcode %u of cell %d in '%s', must be in [0,%d].\n",
            *soilcode,cell_id,config->soil_filename.name,config->soilmap_size-1);
    return TRUE;
  }
  if(config->with_nitrogen)
  {
    if(celldata->soilph.fmt==CDF)
    {
      if(readinput_netcdf(celldata->soilph.file.cdf,&cell->soilph,&cell->coord))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->soilph_filename.name,cell_id+config->startgrid);
        return TRUE;
      }
    }
    else
    {
      if(readrealvec(celldata->soilph.file.bin.file,&cell->soilph,0,celldata->soilph.file.bin.scalar,1,celldata->soilph.file.bin.swap,celldata->soilph.file.bin.type))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->soilph_filename.name,cell_id+config->startgrid);
        return TRUE;
      }
    }
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
  closefile(&celldata->kbf);
  if(celldata->with_nitrogen)
    closefile(&celldata->soilph);
  closefile(&celldata->slope);
  closefile(&celldata->slope_min);
  closefile(&celldata->slope_max);
  free(celldata);
} /* of 'closecelldata' */
