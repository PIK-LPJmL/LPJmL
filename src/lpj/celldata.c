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
  int kbf_fmt;
  int slope_fmt;
  int slope_min_fmt;
  int slope_max_fmt;
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
  int soilph_fmt;
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
  } soilph;
  struct
  {
    Bool swap;
    size_t offset;
    FILE *file;
  } runoff2ocean_map;
  union
  {
    struct
    {
      Bool swap;
      size_t offset;
      int version;
      Real scalar;
      Type type;
      FILE *file;
    } bin;
    Input_netcdf cdf;
  } kbf;
  union
  {
    struct
    {
      Bool swap;
      size_t offset;
      int version;
      Real scalar;
      Type type;
      FILE *file;
    } bin;
    Input_netcdf cdf;
  } slope;
  union
  {
    struct
    {
      Bool swap;
      size_t offset;
      int version;
      Real scalar;
      Type type;
      FILE *file;
    } bin;
    Input_netcdf cdf;
  } slope_min;
  union
  {
    struct
    {
      Bool swap;
      size_t offset;
      int version;
      Real scalar;
      Type type;
      FILE *file;
    } bin;
    Input_netcdf cdf;
  } slope_max;
};

Celldata opencelldata(Config *config /**< LPJmL configuration */
                     )               /** \return pointer to cell data or NULL */
{
  Celldata celldata;
  Header header;
  String headername;
  int version;
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
    if(isroot(*config) && config->nall>numcoord(celldata->soil.bin.file_coord))
      fprintf(stderr,
              "WARNING003: Number of gridcells in '%s' distinct from %d.\n",
              config->coord_filename.name,numcoord(celldata->soil.bin.file_coord));

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
  celldata->kbf_fmt = config->kbf_filename.fmt;
  if (config->kbf_filename.fmt == CDF)
  {
    celldata->kbf.cdf = openinput_netcdf(&config->kbf_filename,
                                         NULL, 0, config);
    if (celldata->kbf.cdf == NULL)
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

    celldata->kbf.bin.file = openinputfile(&header, &celldata->kbf.bin.swap,
                                           &config->kbf_filename,
                                           headername,
                                           &version,&celldata->kbf.bin.offset,FALSE,config);

    if (celldata->kbf.bin.file == NULL)
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
    celldata->kbf.bin.scalar = (version == 1) ? 0.001 : header.scalar;
    if(celldata->kbf.bin.version<3)
      celldata->kbf.bin.type=LPJ_FLOAT;
    else
      celldata->kbf.bin.type=header.datatype;
  }
  if(config->with_nitrogen)
  {
    celldata->soilph_fmt=config->soilph_filename.fmt;
    celldata->with_nitrogen=TRUE;
    if(config->soilph_filename.fmt==CDF)
    {
      celldata->soilph.cdf=openinput_netcdf(&config->soilph_filename,
                                            NULL,0,config);
      if(celldata->soilph.cdf==NULL)
      {
        if(isroot(*config))
          printfopenerr(config->soilph_filename.name);
        if(config->soil_filename.fmt==CDF)
          closecoord_netcdf(celldata->soil.cdf);
        else
        {
          closecoord(celldata->soil.bin.file_coord);
          fclose(celldata->soil.bin.file);
        }
        if (config->kbf_filename.fmt == CDF)
          closeinput_netcdf(celldata->kbf.cdf);
        else
          fclose(celldata->kbf.bin.file);
        free(celldata);
        return NULL;
      }
    }
    else
    {
      celldata->soilph.bin.file=openinputfile(&header,&celldata->soilph.bin.swap,
                                              &config->soilph_filename,
                                              headername,
                                              &version,&celldata->soilph.bin.offset,FALSE,config);
      if(celldata->soilph.bin.file==NULL)
      {
        if(isroot(*config))
          printfopenerr(config->soilph_filename.name);
        if(config->soil_filename.fmt==CDF)
          closecoord_netcdf(celldata->soil.cdf);
        else
        {
          closecoord(celldata->soil.bin.file_coord);
          fclose(celldata->soil.bin.file);
        }
        if(config->kbf_filename.fmt==CDF)
      	  closeinput_netcdf(celldata->kbf.cdf);
        else
           fclose(celldata->kbf.bin.file);
        free(celldata);
        return NULL;
      }
      if(version==1)
        celldata->soilph.bin.scalar=0.01;
      else
        celldata->soilph.bin.scalar=header.scalar;
      celldata->soilph.bin.type=header.datatype;

    }

  }
  else
    celldata->with_nitrogen=FALSE;
  if(config->sim_id==LPJML_FMS)
  {
    celldata->runoff2ocean_map.file=openinputfile(&header,&celldata->runoff2ocean_map.swap,
                                                  &config->runoff2ocean_filename,
                                                  headername,
                                                  &version,&celldata->runoff2ocean_map.offset,FALSE,config);
    if(celldata->runoff2ocean_map.file==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->runoff2ocean_filename.name);
      if(config->soil_filename.fmt==CDF)
        closecoord_netcdf(celldata->soil.cdf);
      else
      {
        closecoord(celldata->soil.bin.file_coord);
        fclose(celldata->soil.bin.file);
      }
      if (config->kbf_filename.fmt == CDF)
        closeinput_netcdf(celldata->kbf.cdf);
      else
        fclose(celldata->kbf.bin.file);
      if(config->soilph_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->soilph.cdf);
      else
          fclose(celldata->soilph.bin.file);
      free(celldata);
      return NULL;
    }
  }
  else
    celldata->runoff2ocean_map.file=NULL;
  celldata->slope_fmt=config->slope_filename.fmt;
  if(config->slope_filename.fmt==CDF)
  {
    celldata->slope.cdf=openinput_netcdf(config->slope_filename.name,
                                         NULL,0,config);
    if(celldata->slope.cdf==NULL)
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
      if(config->kbf_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->kbf.cdf);
      else
         fclose(celldata->kbf.bin.file);
      if(config->soilph_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->soilph.cdf);
      else
          fclose(celldata->soilph.bin.file);
      fclose(celldata->runoff2ocean_map.file);
      free(celldata);
      return NULL;
    }
  }
  else
  {
    celldata->slope.bin.file=openinputfile(&header,&celldata->slope.bin.swap,
                                           &config->slope_filename,
                                           headername,
                                           &version,&celldata->slope.bin.offset,FALSE,config);

    if(celldata->slope.bin.file==NULL)
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
      if(config->kbf_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->kbf.cdf);
      else
         fclose(celldata->kbf.bin.file);
      if(config->soilph_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->soilph.cdf);
      else
          fclose(celldata->soilph.bin.file);
      fclose(celldata->runoff2ocean_map.file);
      free(celldata);
      return NULL;
    }
    celldata->slope.bin.scalar=(version==1) ? 1 : header.scalar;
    if(version<3)
      celldata->slope.bin.type=LPJ_FLOAT;
    else
      celldata->slope.bin.type=header.datatype;
  }
  celldata->slope_min_fmt=config->slope_min_filename.fmt;
  if(config->slope_min_filename.fmt==CDF)
  {
    celldata->slope_min.cdf=openinput_netcdf(config->slope_min_filename.name,
                                             NULL,0,config);
    if(celldata->slope_min.cdf==NULL)
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
      if(config->kbf_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->kbf.cdf);
      else
         fclose(celldata->kbf.bin.file);
      fclose(celldata->runoff2ocean_map.file);
      if(config->slope_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->slope.cdf);
      else
         fclose(celldata->slope.bin.file);
      if(config->soilph_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->soilph.cdf);
      else
          fclose(celldata->soilph.bin.file);
     free(celldata);
      return NULL;
    }
  }
  else
  {
    celldata->slope_min.bin.file=openinputfile(&header,&celldata->slope_min.bin.swap,
                                               &config->slope_min_filename,
                                               headername,
                                               &version,&celldata->slope_min.bin.offset,
                                               FALSE,config);
    if(celldata->slope_min.bin.file==NULL)
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
      if(config->kbf_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->kbf.cdf);
      else
         fclose(celldata->kbf.bin.file);
      fclose(celldata->runoff2ocean_map.file);
      if(config->slope_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->slope.cdf);
      else
         fclose(celldata->slope.bin.file);
      if(config->soilph_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->soilph.cdf);
      else
          fclose(celldata->soilph.bin.file);
      free(celldata);
      return NULL;
    }
    celldata->slope_min.bin.scalar=(version==1) ? 1 : header.scalar;
    if(version<3)
      celldata->slope_min.bin.type=LPJ_FLOAT;
    else
      celldata->slope_min.bin.type=header.datatype;
  }
  celldata->slope_max_fmt=config->slope_max_filename.fmt;
  if(config->slope_max_filename.fmt==CDF)
  {
    celldata->slope_max.cdf=openinput_netcdf(config->slope_max_filename.name,
                                             NULL,0,config);
    if(celldata->slope_max.cdf==NULL)
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
      if(config->kbf_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->kbf.cdf);
      else
         fclose(celldata->kbf.bin.file);
      fclose(celldata->runoff2ocean_map.file);
      if(config->slope_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->slope.cdf);
      else
         fclose(celldata->slope.bin.file);
      if(config->slope_min_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->slope_min.cdf);
      else
         fclose(celldata->slope_min.bin.file);
      if(config->soilph_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->soilph.cdf);
      else
          fclose(celldata->soilph.bin.file);
      free(celldata);
      return NULL;
    }
  }
  else
  {
    celldata->slope_max.bin.file=openinputfile(&header,&celldata->slope_max.bin.swap,
                                               &config->slope_max_filename,
                                               headername,&version,&celldata->slope_max.bin.offset,FALSE,config);


    if(celldata->slope_max.bin.file==NULL)
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
      if(config->kbf_filename.fmt==CDF)
        closeinput_netcdf(celldata->kbf.cdf);
      else
        fclose(celldata->kbf.bin.file);
      fclose(celldata->runoff2ocean_map.file);
      if(config->slope_filename.fmt==CDF)
        closeinput_netcdf(celldata->slope.cdf);
      else
        fclose(celldata->slope.bin.file);
      if(config->slope_min_filename.fmt==CDF)
        closeinput_netcdf(celldata->slope_min.cdf);
      else
        fclose(celldata->slope_min.bin.file);
      if(config->soilph_filename.fmt==CDF)
    	  closeinput_netcdf(celldata->soilph.cdf);
      else
          fclose(celldata->soilph.bin.file);
      free(celldata);
      return NULL;
    }
    celldata->slope_max.bin.scalar=(version==1) ? 1 : header.scalar;
    if(version<3)
      celldata->slope_max.bin.type=LPJ_FLOAT;
    else
      celldata->slope_max.bin.type=header.datatype;
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
  if(celldata->kbf_fmt!=CDF &&
     fseek(celldata->kbf.bin.file,startgrid*typesizes[celldata->kbf.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in kbf file to position %d.\n",
            startgrid);
    return TRUE;
  }
  if(celldata->slope_fmt!=CDF &&
     fseek(celldata->slope.bin.file,startgrid*typesizes[celldata->slope.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in slope file to position %d.\n",
            startgrid);
    return TRUE;
  }
  if(celldata->slope_min_fmt!=CDF &&
     fseek(celldata->slope_min.bin.file,startgrid*typesizes[celldata->slope_min.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in slope_min file to position %d.\n",
            startgrid);
    return TRUE;
  }
  if(celldata->slope_max_fmt!=CDF &&
     fseek(celldata->slope_max.bin.file,startgrid*typesizes[celldata->slope_max.bin.type],SEEK_CUR))
  {
    /* seeking to position of first grid cell failed */
    fprintf(stderr,
            "ERROR107: Cannot seek in slope_max file to position %d.\n",
            startgrid);
    return TRUE;
  }

  if(celldata->with_nitrogen)
  {
    if(celldata->soilph_fmt!=CDF &&
       fseek(celldata->soilph.bin.file,startgrid*typesizes[celldata->soilph.bin.type],SEEK_CUR))
    {
      /* seeking to position of first grid cell failed */
      fprintf(stderr,
              "ERROR107: Cannot seek in soilpH file to position %d.\n",
              startgrid);
      return TRUE;
    }

  }
  if(celldata->runoff2ocean_map.file!=NULL)
    fseek(celldata->runoff2ocean_map.file,startgrid*sizeof(Intcoord)+celldata->runoff2ocean_map.offset,SEEK_CUR);
  return FALSE;
} /* of 'seekcelldata' */

Bool readcelldata(Celldata celldata, /**< pointer to celldata */
                  Coord *coord,      /**< lon,lat coordinate */
                  unsigned int *soilcode,     /**< soil code */
                  Real *soil_ph,                /**< soil pH */
                  Real *kbf,
                  Real *Hag_beta, 
                  Real *slope,
                  Real *slope_min, 
                  Real *slope_max,
                  Intcoord *runoff2ocean_coord, /**< coordinate for runoff */
                  int cell,          /**< cell index */
                  Config *config     /**< LPJmL configuration */
                 )                   /** \return TRUE on error */
{
  if(celldata->soil_fmt==CDF)
  {
    if(readcoord_netcdf(celldata->soil.cdf,coord,&config->resolution,soilcode))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->soil_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readcoord(celldata->soil.bin.file_coord,coord,&config->resolution))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->coord_filename.name,cell+config->startgrid);
      return TRUE;
    }
    /* read soilcode from file */

    if(freadsoilcode(celldata->soil.bin.file,soilcode,
                     celldata->soil.bin.swap,celldata->soil.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->soil_filename.name,cell+config->startgrid);
      config->ngridcell=cell;
      return TRUE;
    }
  }
  /* read kbf value*/
  if(celldata->kbf_fmt==CDF)
  {
    if(readinput_netcdf(celldata->kbf.cdf,kbf,coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->kbf_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->kbf.bin.file,kbf,0,celldata->kbf.bin.scalar,1,celldata->kbf.bin.swap,celldata->kbf.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->kbf_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  /* read slope value and calculate Haggard Beta value*/
  if(celldata->slope_fmt==CDF)
  {
    if(readinput_netcdf(celldata->slope.cdf,slope,coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->slope.bin.file,slope,0,celldata->slope.bin.scalar,1,celldata->slope.bin.swap,celldata->slope.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }

  *Hag_beta=min(1,(0.06*log(*slope+0.1)+0.22)/0.43);
  if(*Hag_beta>1) fail(HAG_BETA_ERR,FALSE,"HAG_BETA greater than 1 HAG_BETA= %.2f  slope= %.2f lat=e= %.2f lon=e= %.2f\n", *Hag_beta,*slope,coord->lat,coord->lon);

  if(celldata->slope_min_fmt==CDF)
  {
    if(readinput_netcdf(celldata->slope_min.cdf,slope_min,coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_min_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->slope_min.bin.file,slope_min,0,celldata->slope_min.bin.scalar,1,celldata->slope_min.bin.swap,celldata->slope_min.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_min_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  if(celldata->slope_max_fmt==CDF)
  {
    if(readinput_netcdf(celldata->slope_max.cdf,slope_max,coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_max_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(celldata->slope_max.bin.file,slope_max,0,celldata->slope_max.bin.scalar,1,celldata->slope_max.bin.swap,celldata->slope_max.bin.type))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              config->slope_max_filename.name,cell+config->startgrid);
      return TRUE;
    }
  }
  if(config->with_nitrogen)
  {
    if(celldata->soilph_fmt==CDF)
    {
      if(readinput_netcdf(celldata->soilph.cdf,soil_ph,coord))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->soilph_filename.name,cell+config->startgrid);
        return TRUE;
      }
    }
    else
    {
      if(readrealvec(celldata->soilph.bin.file,soil_ph,0,celldata->soilph.bin.scalar,1,celldata->soilph.bin.swap,celldata->soilph.bin.type))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                config->soilph_filename.name,cell+config->startgrid);
        return TRUE;
      }
    }
  }
  if(celldata->runoff2ocean_map.file!=NULL)
  {
    fread(runoff2ocean_coord,sizeof(Intcoord),1,celldata->runoff2ocean_map.file);
    if(celldata->runoff2ocean_map.swap)
    {
      runoff2ocean_coord->lon=swapshort(runoff2ocean_coord->lon);
      runoff2ocean_coord->lat=swapshort(runoff2ocean_coord->lat);
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
  if(celldata->kbf_fmt==CDF)
    closeinput_netcdf(celldata->kbf.cdf);
  else
    fclose(celldata->kbf.bin.file);
  if(celldata->slope_fmt==CDF)
    closeinput_netcdf(celldata->slope.cdf);
  else
    fclose(celldata->slope.bin.file);
  if(celldata->slope_min_fmt==CDF)
    closeinput_netcdf(celldata->slope_min.cdf);
  else
    fclose(celldata->slope_min.bin.file);
  if(celldata->slope_max_fmt==CDF)
    closeinput_netcdf(celldata->slope_max.cdf);
  else
    fclose(celldata->slope_max.bin.file);
  if(celldata->with_nitrogen)
  {
    if(celldata->soilph_fmt==CDF)
      closeinput_netcdf(celldata->soilph.cdf);
    else
      fclose(celldata->soilph.bin.file);
  }
  if(celldata->runoff2ocean_map.file!=NULL)
    fclose(celldata->runoff2ocean_map.file);
  free(celldata);
} /* of 'closecelldata' */
