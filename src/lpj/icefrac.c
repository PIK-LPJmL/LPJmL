/**************************************************************************************/
/**                                                                                \n**/
/**                     i  c  e  f  r  a  c  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of datatype Icefrac                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct icefrac
{
  Climatefile file;
  Real *data[3];
}; /* Definition of opaque datatype Icefrac */

Icefrac initicefrac(const Config *config /**< LPJ configuration */
                   )                     /** \return pointer to icefrac struct or NULL */
{
  Header header;
  Icefrac icefrac;
  size_t offset;
  String headername;
  int i, version, ndata;

  if (config->icefrac_filename.name == NULL)
    return NULL;
  icefrac = new(struct icefrac);
  if (icefrac == NULL)
  {
    printallocerr("icefrac");
    return NULL;
  }
  icefrac->file.fmt = config->icefrac_filename.fmt;
  if (config->icefrac_filename.fmt == CDF)
  {
    if (opendata_netcdf(&icefrac->file,&config->icefrac_filename,"fraction",config))
    {
      free(icefrac);
      return NULL;
    }
  }
  else
  {
    if ((icefrac->file.file = openinputfile(&header, &icefrac->file.swap,
                                            &config->icefrac_filename,
                                            headername,
                                            &version, &offset,TRUE, config)) == NULL)
    {
      free(icefrac);
      return NULL;
    }

    icefrac->file.firstyear = header.firstyear;
    icefrac->file.size = header.ncell * typesizes[header.datatype];
    icefrac->file.scalar = header.scalar;
    icefrac->file.datatype = header.datatype;
    if (config->icefrac_filename.fmt == RAW)
      icefrac->file.offset = config->startgrid * sizeof(short);
    else
    {
      if(header.nbands!=1)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR218: Number of bands=%d in ice fraction file '%s' is not 1.\n",
                  header.nbands,config->icefrac_filename.name);
        fclose(icefrac->file.file);
        free(icefrac);
        return NULL;
      }
      icefrac->file.offset = config->startgrid * typesizes[header.datatype] + headersize(headername, version)+offset;
    }
  }
  icefrac->file.n = config->ngridcell;
  ndata = (config->delta_year>1) ? 3 : 1;
  icefrac->data[1] = icefrac->data[2] = NULL;
  for (i = 0; i<ndata; i++)
    if ((icefrac->data[i] = newvec(Real, icefrac->file.n)) == NULL)
    {
      printallocerr("data");
      if (icefrac->file.fmt == CDF)
        closeclimate_netcdf(&icefrac->file, isroot(*config));
      else
        fclose(icefrac->file.file);
      free(icefrac);
      return NULL;
    }
  return icefrac;
} /* of 'initicefrac' */

  /*
  - called in lpj()
  - opens the icefrac input file (see also building file for the icefrac Input (like cfts26_lu2clm.c)
  - sets the icefrac variables (see also manage.h)
  */

Bool readicefrac(Icefrac icefrac,     /**< pointer to population data */
                 const Cell grid[],   /**< LPJ grid */
                 int index,
                 int year,            /**< year (AD) */
                 const Config *config /**< LPJ configuration */
                )                     /** \return TRUE on error */
{
  int index_year;
  index_year = (year - icefrac->file.firstyear) / config->delta_year;
  if (icefrac->file.fmt == CDF)
    return readdata_netcdf(&icefrac->file, icefrac->data[index], grid, index_year, config);
  if (fseek(icefrac->file.file, index_year*icefrac->file.size + icefrac->file.offset, SEEK_SET))
  {
    fprintf(stderr, "ERROR184: Cannot seek to ice fraction of year %d in geticefrac().\n", year);
    return TRUE;
  }
  if(readrealvec(icefrac->file.file,icefrac->data[index],0,icefrac->file.scalar,icefrac->file.n,icefrac->file.swap,icefrac->file.datatype))
  {
    fprintf(stderr, "ERROR184: Cannot read ice fraction of year %d in geticefrac().\n", year);
    return TRUE;
  }
  return FALSE;
} /* of 'readicefrac' */

void interpolate_icefrac(Icefrac icefrac, int index, Real x)
{
  interpolate_data(icefrac->data[0], icefrac->data[index + 1], icefrac->data[(index + 1) % 2 + 1], icefrac->file.n, x);
} /* of 'interpolate_icefrac' */

Real geticefrac(const Icefrac icefrac, int cell)
{
  return  icefrac->data[0][cell];
}  /* of 'geticefrac' */

void freeicefrac(Icefrac icefrac, Bool isroot)
{
  if (icefrac != NULL)
  {
    if (icefrac->file.fmt == CDF)
      closeclimate_netcdf(&icefrac->file, isroot);
    else
      fclose(icefrac->file.file);
    free(icefrac->data[0]);
    free(icefrac->data[1]);
    free(icefrac->data[2]);
    free(icefrac);
  }
} /* of 'freeicefrac' */
