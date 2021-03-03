/***************************************************************************/
/**                                                                       **/
/**                 i  n  i  t  h  y  d  r  o . c                         **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**                                                                       **/
/**     Hydrotopes code                                                   **/
/**     Thomas Kleinen, 27/06/2008                                        **/
/**                                                                       **/
/**     initialisation of cti fields                                      **/
/***************************************************************************/

#include "lpj.h"

#define CTI_DATA_LENGTH 3

Bool inithydro(Cell *grid,
  Config *config)
{
  FILE *hydrofile;
  Header header;
  String headername;
  int version;
  int cell;
  int swap;
  short ctidata_in[CTI_DATA_LENGTH];
  size_t offset;

  if ((hydrofile = openinputfile(&header, &swap, &config->hydrotopes_filename,
    headername,
    &version, &offset, FALSE,config)) == NULL)
    return TRUE;
  if (fseek(hydrofile, config->startgrid * sizeof(short)*CTI_DATA_LENGTH, SEEK_CUR))
  {
    fclose(hydrofile);
    return TRUE;
  }
  for (cell = 0; cell<config->ngridcell; cell++)
  {
    if (freadshort(ctidata_in, CTI_DATA_LENGTH, swap, hydrofile) != CTI_DATA_LENGTH)
    {
      fclose(hydrofile);
      return TRUE;
    }
    grid[cell].hydrotopes.skip_cell = TRUE;
    if (grid[cell].coord.lat > hydropar.lat_min)
    {
      grid[cell].hydrotopes.cti_mean = (float)ctidata_in[0] / header.scalar;
      if (grid[cell].hydrotopes.cti_mean > hydropar.cti_thres)
      {
        grid[cell].hydrotopes.cti_chi = ((float)ctidata_in[1] / header.scalar*(float)ctidata_in[2] / header.scalar) / 2;
        if (grid[cell].hydrotopes.cti_chi > 0.)
        {
          grid[cell].hydrotopes.cti_phi = ((float)ctidata_in[1] / header.scalar*(float)ctidata_in[1] / header.scalar) / (grid[cell].hydrotopes.cti_chi*grid[cell].hydrotopes.cti_chi);
          if (grid[cell].hydrotopes.cti_phi > 100.)
            grid[cell].hydrotopes.cti_phi = 100.;
          grid[cell].hydrotopes.cti_mu = (float)ctidata_in[0] / header.scalar - (grid[cell].hydrotopes.cti_phi*grid[cell].hydrotopes.cti_chi);
          grid[cell].hydrotopes.skip_cell = FALSE;
        }
      }
    }
  }
  fclose(hydrofile);
  return FALSE;
} /* inithydro' */
