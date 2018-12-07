/**************************************************************************************/
/**                                                                                \n**/
/**               o  p  e  n  i  n  p  u  t  f  i  l  e  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions opens input file and reads file header                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

FILE *openinputfile(Header *header, /**< pointer to file header */
                    Bool *swap, /**< byte order has to be changed (TRUE/FALSE) */
                    const Filename *filename, /**< file name */
                    String headername, /**< clm file header string */
                    int *version, /**< clm file version */
                    size_t *offset, /**< offset in binary file */
                    const Config *config /**< grid configuration */
                   )           /** \return file pointer to open file or NULL */
{
  FILE *file;
  if(filename->fmt==META)
  {
    *version=4;
    /* set default values for header */
    header->order=CELLYEAR;
    header->firstyear=config->firstyear;
    header->nyear=config->lastyear-config->firstyear+1;
    header->firstcell=0;
    header->ncell=config->nall;
    header->nbands=1;
    header->scalar=1;
    header->datatype=LPJ_SHORT;
    header->cellsize_lon=config->resolution.lon;
    header->cellsize_lat=config->resolution.lat;
    /* open description file */
    file=openmetafile(header,swap,offset,filename->name,isroot(*config));
    if(file==NULL)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR224: Cannot read description file '%s'.\n",filename->name);
      return NULL;
    }
  /*  if(fabs(header->cellsize_lon-config->resolution.lon)>epsilon)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR154: Longitudinal cell size %g different from %g in '%s'.\n",
                header->cellsize_lon,config->resolution.lon,filename->name);
      fclose(file);  
      return NULL;
    }
    if(fabs(header->cellsize_lat-config->resolution.lat)>epsilon)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR154: Latitudinal cell size %g different from %g in '%s'.\n",
                header->cellsize_lat,config->resolution.lat,filename->name);
      fclose(file);  
      return NULL;
    }*/
    if(header->firstyear>config->firstyear)
      if(isroot(*config))
        fprintf(stderr,"WARNING004: First year in '%s'=%d greater than %d.\n",
                 filename->name,header->firstyear,config->firstyear);
    if(config->firstgrid<header->firstcell ||
       config->nall+config->firstgrid>header->ncell+header->firstcell)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR155: gridcells [%d,%d] in '%s' not in [%d,%d].\n",
                header->firstcell,header->ncell+header->firstcell-1,filename->name,
                config->firstgrid,config->nall+config->firstgrid-1);
      fclose(file);  
      return NULL;
    }
    return file;
  }
  *offset=0; /* no additional offset in CLM file */
  if((file=fopen(filename->name,"rb"))==NULL)
  {
    if(isroot(*config))
      printfopenerr(filename->name);
    return NULL;
  }
  if(filename->fmt==RAW)
  {
    header->order=CELLYEAR;
    header->firstyear=config->firstyear;
    header->nyear=config->lastyear-config->firstyear+1;
    *swap=FALSE;
    header->firstcell=0;
    header->ncell=config->nall;
    header->nbands=1;
    header->scalar=1;
    header->datatype=LPJ_SHORT;
    *version=0;
  }
  else
  {
    *version=(filename->fmt==CLM) ? READ_VERSION : 2;
    if(freadanyheader(file,header,swap,headername,version))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR154: Invalid header in '%s'.\n",filename->name);
      fclose(file);
      return NULL;
    }
    if(*version>1)
    {
      if(fabs(header->cellsize_lon-config->resolution.lon)>epsilon)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR154: Longitudinal cell size %g different from %g in '%s'.\n",
                  header->cellsize_lon,config->resolution.lon,filename->name);
        fclose(file);
        return NULL;
      }
      if(fabs(header->cellsize_lat-config->resolution.lat)>epsilon)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR154: Latitudinal cell size %g different from %g in '%s'.\n",
                  header->cellsize_lat,config->resolution.lat,filename->name);
        fclose(file);
        return NULL;
      }
    }
    if(header->firstyear>config->firstyear)
      if(isroot(*config))
        fprintf(stderr,"WARNING004: First year in '%s'=%d greater than %d.\n",
                filename->name,header->firstyear,config->firstyear);
    if(config->firstgrid<header->firstcell ||
       config->nall+config->firstgrid>header->ncell+header->firstcell)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR155: gridcells [%d,%d] in '%s' not in [%d,%d].\n",
                header->firstcell,header->ncell+header->firstcell-1,filename->name,
                config->firstgrid,config->nall+config->firstgrid-1);
      fclose(file);
      return NULL;
    }
  }
  return file;
} /* of 'openinputfile' */
