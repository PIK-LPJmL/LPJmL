/**************************************************************************************/
/**                                                                                \n**/
/**                 i  n  i  t  h  y  d  r  o . c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Initialisation of cti fields                                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define CTI_DATA_LENGTH 3

Bool inithydro(Cell *grid,    /**< LPJ grid */
               Config *config /**< LPJmL configuation */
              )               /** \return TRUE on error */
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
                                 headername,NULL,LPJ_SHORT,
                                 &version, &offset, FALSE,config)) == NULL)
    return TRUE;
  if(header.nbands!=CTI_DATA_LENGTH)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Number of bands=%d in CTI file '%s' is not %d.\n",
              header.nbands,config->hydrotopes_filename.name,CTI_DATA_LENGTH);
    fclose(hydrofile);
    return TRUE;
  }
  if(header.datatype!=LPJ_SHORT)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Datatype %s in CTI file '%s' is not short.\n",
              typenames[header.datatype],config->hydrotopes_filename.name);
    fclose(hydrofile);
    return TRUE;
  }
  if (fseek(hydrofile, config->startgrid * sizeof(short)*CTI_DATA_LENGTH+offset, SEEK_CUR))
  {
    fprintf(stderr,"ERROR150: Cannot seek to cell %d in CTI file '%s'.\n",
            config->startgrid,config->hydrotopes_filename.name);
    fclose(hydrofile);
    return TRUE;
  }
  for (cell = 0; cell<config->ngridcell; cell++)
  {
    if (freadshort(ctidata_in, CTI_DATA_LENGTH, swap, hydrofile) != CTI_DATA_LENGTH)
    {
      fprintf(stderr,"ERROR151: Cannot read cell %d in CTI file '%s'.\n",
              config->startgrid+cell,config->hydrotopes_filename.name);
      fclose(hydrofile);
      return TRUE;
    }
    if (grid[cell].coord.lat > hydropar.lat_min && grid[cell].hydrotopes.skip_cell)
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
