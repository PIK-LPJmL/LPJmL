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

Bool inithydro(Cell *grid,    /**< LPJ grid */
               Config *config /**< LPJmL configuation */
              )               /** \return TRUE on error */
{
  Infile hydrofile;
  int cell;
  Real ctidata_in[CTI_DATA_LENGTH];
  if(openinputdata(&hydrofile,&config->hydrotopes_filename,"hydrotopes",NULL,LPJ_SHORT,0.001,CTI_DATA_LENGTH,config))
    return TRUE;
  for (cell = 0; cell<config->ngridcell; cell++)
  {
    if(readinputdata(&hydrofile,ctidata_in,&grid[cell].coord,cell,&config->hydrotopes_filename))
    {
      closeinput(&hydrofile);
      return TRUE;
    }
    grid[cell].hydrotopes.skip_cell = TRUE;
    grid[cell].hydrotopes.cti_mean = ctidata_in[0];
    if (grid[cell].hydrotopes.cti_mean > config->hydropar.cti_thres)
    {
      grid[cell].hydrotopes.cti_chi = (ctidata_in[1] *ctidata_in[2]) / 2;
      if (grid[cell].hydrotopes.cti_chi > 0.)
      {
        grid[cell].hydrotopes.cti_phi = (ctidata_in[1] *ctidata_in[1] ) / (grid[cell].hydrotopes.cti_chi*grid[cell].hydrotopes.cti_chi);
        if (grid[cell].hydrotopes.cti_phi > 100.)
          grid[cell].hydrotopes.cti_phi = 100.;
        grid[cell].hydrotopes.cti_mu = ctidata_in[0] - (grid[cell].hydrotopes.cti_phi*grid[cell].hydrotopes.cti_chi);
        grid[cell].hydrotopes.skip_cell = FALSE;
      }
    }
  }
  closeinput(&hydrofile);
  return FALSE;
} /* inithydro' */
