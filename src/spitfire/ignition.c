/**************************************************************************************/
/**                                                                                \n**/
/**                     i  g  n  i  t  i  o  n  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of datatype Human_ignition                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct human_ignition
{
  Climatefile file;
}; /* Definition of opaque datatype Human_ignition */

Human_ignition inithumanignition(const Config *config /**< LPJ configuration */
                                 )                     /** \return pointer to human ignition
                                                            struct or NULL */
{
  Human_ignition ignition;

  ignition=new(struct human_ignition);
  if(ignition==NULL)
  {
    printallocerr("ignition");
    return NULL;
  }
  if(opendata(&ignition->file,&config->human_ignition_filename,"human ignition","yr-1",LPJ_FLOAT,LPJ_SHORT,1.0,1,TRUE,config))
  {
    free(ignition);
    return NULL;
  }
  if(isroot(*config) && config->human_ignition_filename.fmt!=SOCK && config->lastyear>ignition->file.firstyear+ignition->file.nyear-1)
    fprintf(stderr,"WARNING024: Last year in ignition data file=%d is less than last simulation year %d, data from last year used.\n",
            ignition->file.firstyear+ignition->file.nyear-1,config->lastyear);

  return ignition;
} /* of 'inithumanignition' */

/*
- called in lpj()
- opens the ignition input file (see also building file for the ignition Input (like cfts26_lu2clm.c)
- sets the ignition variables (see also manage.h)
*/

Bool gethumanignition(Human_ignition ignition, /**< pointer to population data */
                      int year,                /**< year (AD) */
                      Cell grid[],             /**< LPJ grid */
                      const Config *config     /**< LPJ configuration */
                     )                         /** \return TRUE on error */
{
  Real *data;
  int cell;
  data=readdata(&ignition->file,NULL,grid,"human ignition",year,config);
  if(data==NULL)
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    grid[cell].ignition.human=data[cell];
    if(grid[cell].ignition.human<0)
      grid[cell].ignition.human=0;
    grid[cell].ignition.human/=365;
  }
  free(data);
  return FALSE;
} /* of 'gethumanignition' */

void freehumanignition(Human_ignition ignition,Bool isroot)
{
  if(ignition!=NULL)
  {
    closeclimatefile(&ignition->file,isroot);
    free(ignition);
  }
} /* of 'freehumanignition' */
