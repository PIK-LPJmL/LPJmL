/***************************************************************************/
/**                                                                       **/
/**                     i  c  e  f  r  a  c  .  c                         **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Definition of datatype Icefrac                                    **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Landuse Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: 21.02.2007 M. Gumpenberger                           **/
/**     Last change: $Date:: 2014-11-28 14:33:41 +0100 (Fri, 28 Nov 201#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

struct icefrac
{
  Climatefile file;
  Real *data[3];
}; /* Definition of opaque datatype Icefrac */

Icefrac initicefrac(const Config *config /* LPJ configuration */
)                     /* returns pointer to icefrac
                      struct or NULL */
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
    return NULL;
  icefrac->file.fmt = config->icefrac_filename.fmt;
  if (config->icefrac_filename.fmt == CDF)
  {
    if (opendata_netcdf(&icefrac->file,&config->icefrac_filename,
      "fraction",config))
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
    icefrac->file.size = header.ncell * sizeof(short);
    icefrac->file.scalar = header.scalar;
    if (config->icefrac_filename.fmt == RAW)
      icefrac->file.offset = config->startgrid * sizeof(short);
    else
      icefrac->file.offset = config->startgrid * sizeof(short) + headersize(headername, version);
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

Bool readicefrac(Icefrac icefrac,     /* pointer to population data */
  const Cell grid[],   /* LPJ grid */
  int index,
  int year,            /* year (AD) */
  const Config *config /* LPJ configuration */
)                     /* returns TRUE on error */
{
  short *vec;
  int i, index_year;
  index_year = (year - icefrac->file.firstyear) / config->delta_year;
  if (icefrac->file.fmt == CDF)
    return readdata_netcdf(&icefrac->file, icefrac->data[index], grid, index_year, config);
  if (fseek(icefrac->file.file, index_year*icefrac->file.size + icefrac->file.offset, SEEK_SET))
  {
    fprintf(stderr, "ERROR184: Cannot seek to ice fraction of year %d in geticefrac().\n", year);
    return TRUE;
  }
  vec = newvec(short, icefrac->file.n);
  if (vec == NULL)
  {
    printallocerr("vec");
    return TRUE;
  }
  if (fread(vec, sizeof(short), icefrac->file.n, icefrac->file.file) != icefrac->file.n)
  {
    fprintf(stderr, "ERROR184: Cannot read ice fraction of year %d in geticefrac().\n", year);
    free(vec);
    return TRUE;
  }

  if (icefrac->file.swap)
    for (i = 0; i<icefrac->file.n; i++)
      icefrac->data[index][i] = swapshort(vec[i])*icefrac->file.scalar;
  else
    for (i = 0; i<icefrac->file.n; i++)
      icefrac->data[index][i] = vec[i] * icefrac->file.scalar;
  free(vec);

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
