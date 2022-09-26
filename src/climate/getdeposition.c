/**************************************************************************************/
/**                                                                                \n**/
/**                 g  e  t  d  e  p  o  s  i  t  i  o  n  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads deposition data                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool getdeposition(Climate *climate,    /**< pointer to climate data */
                   const Cell grid[],   /**< LPJ grid */
                   int year,            /**< year of climate data to be read */
                   const Config *config /**< LPJ configuration */
                  )                     /** \return TRUE on error */
{
  int year_depos;
  char *name;
  if(config->const_deposition)
    year=config->depos_year_const+(year-config->depos_year_const) % 30;
  if(climate->data.no3deposition!=NULL)
  {
    if(year<climate->file_no3deposition.firstyear)
      year_depos=climate->file_no3deposition.firstyear;
    else if(year>climate->file_no3deposition.firstyear+climate->file_no3deposition.nyear-1)
      year_depos=climate->file_no3deposition.firstyear+climate->file_no3deposition.nyear-1;
    else
      year_depos=year;
    if(readclimate(&climate->file_no3deposition,climate->data.no3deposition,0,climate->file_no3deposition.scalar,grid,year_depos,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->no3deposition_filename);
        fprintf(stderr,"ERROR131: Cannot read no3deposition of year %d from '%s'.\n",
                year,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.nh4deposition!=NULL)
  {
    if(year<climate->file_nh4deposition.firstyear)
      year_depos=climate->file_nh4deposition.firstyear;
    else if(year>climate->file_nh4deposition.firstyear+climate->file_nh4deposition.nyear-1)
      year_depos=climate->file_nh4deposition.firstyear+climate->file_nh4deposition.nyear-1;
    else
      year_depos=year;
    if(readclimate(&climate->file_nh4deposition,climate->data.nh4deposition,0,climate->file_nh4deposition.scalar,grid,year_depos,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->nh4deposition_filename);
        fprintf(stderr,"ERROR131: Cannot read nh4deposition of year %d from '%s'.\n",
                year,name);
        free(name);
      }
      return TRUE;
    }
  }
  return FALSE;
} /* of 'getdeposition' */
