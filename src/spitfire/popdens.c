/**************************************************************************************/
/**                                                                                \n**/
/**                     p  o  p  d  e  n  s  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of datatype Popdens                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct popdens
{
  Climatefile file;
  Real *npopdens;
}; /* Definition of opaque datatype Popdens */

Popdens initpopdens(const Config *config /**< LPJ configuration */
                   )                     /** \return pointer to population
                                            struct or NULL */
{
  Popdens popdens;
  int i;

  if(config->popdens_filename.name==NULL)
    return NULL;
  popdens=new(struct popdens);
  if(popdens==NULL)
  {
    printallocerr("popdens");
    return NULL;
  }
  if(opendata(&popdens->file,&config->popdens_filename,"population density","km-2",LPJ_FLOAT,LPJ_SHORT,1.0,1,TRUE,config))
  {
    free(popdens);
    return NULL;
  }
  if(isroot(*config) && config->popdens_filename.fmt!=SOCK && config->lastyear>popdens->file.firstyear+popdens->file.nyear-1)
    fprintf(stderr,"WARNING024: Last year in popdens data file=%d is less than last simulation year %d, data from last year used.\n",
            popdens->file.firstyear+popdens->file.nyear-1,config->lastyear);

  popdens->file.n=config->ngridcell;
  if((popdens->npopdens=newvec(Real,popdens->file.n))==NULL)
  {
    printallocerr("npopdens");
    closeclimatefile(&popdens->file,isroot(*config));
    free(popdens);
    return NULL;
  }
  for(i=0;i<popdens->file.n;i++)
    popdens->npopdens[i]=0;
  return popdens;
} /* of 'initpopdens' */

/*
- called in lpj()
- opens the popdens input file (see also building file for the popdens Input (like cfts26_lu2clm.c)
- sets the popdens variables (see also manage.h)
*/

Bool readpopdens(Popdens popdens,     /**< pointer to population data */
                 int year,            /**< year (AD) */
                 const Cell grid[],   /**< LPJ grid */
                 const Config *config /**< LPJ configuration */
                )                     /** \return TRUE on error */
{
  return (readdata(&popdens->file,popdens->npopdens,grid,"population density",year,config)==NULL);
} /* of 'readpopdens' */

Real getpopdens(const Popdens popdens,int cell)
{
  return popdens->npopdens[cell];
}  /* of 'getpopdens' */

void freepopdens(Popdens popdens,Bool isroot)
{
  if(popdens!=NULL)
  {
    closeclimatefile(&popdens->file,isroot);
    free(popdens->npopdens);
    free(popdens);
  }
} /* of 'freepopdens' */
