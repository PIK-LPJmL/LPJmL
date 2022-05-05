/**************************************************************************************/
/**                                                                                \n**/
/**                 w  a  t  e  r  u  s  e  f  c  n  s  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function implements wateruse functions                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct wateruse
{
  Climatefile file;
};               /* definition of opaque datatype Wateruse */

Wateruse initwateruse(const Filename *filename, /**< filename of wateruse file */
                      const Config *config      /**< LPJmL configuration */
                     )
{
  Wateruse wateruse;
  wateruse=new(struct wateruse);
  if(wateruse==NULL)
  {
    printallocerr("wateruse");
    return NULL;
  }
  if(opendata(&wateruse->file,filename,"wateruse",NULL,LPJ_INT,1000.0,1,TRUE,config))
  {
    free(wateruse);
    return NULL;
  }
  return wateruse;
} /* of 'initwateruse' */

static Real *readwateruse(Wateruse wateruse,   /**< Pointer to wateruse data */
                          Cell grid[],         /**< cell grid */
                          int year,            /**< year of wateruse data (AD) */
                          const Config *config /**< LPJ configuration */
                         )                     /** \return data array or NULL on error */
{
  int cell;
  Real *data;
  if(config->wateruse==ALL_WATERUSE)
  {
    /* first and last wateruse data is used outside available wateruse data */
    if(year<=wateruse->file.firstyear)
      year=wateruse->file.firstyear;
    else if(year>=wateruse->file.firstyear+wateruse->file.nyear)
      year=wateruse->file.firstyear+wateruse->file.nyear-1;
  }
  if(year>=wateruse->file.firstyear && year<wateruse->file.firstyear+wateruse->file.nyear)
    data=readdata(&wateruse->file,NULL,grid,"wateruse",year,config);
  else
  {
    data=newvec(Real,config->ngridcell);
    if(data==NULL)
    {
      printallocerr("data");
      return NULL;
    }
    /* no wateruse data available for year, set all to zero */
    for(cell=0;cell<config->ngridcell;cell++)
      data[cell]=0;
  }
  return data;
} /* of 'readwateruse' */

Bool getwateruse(Wateruse wateruse,   /**< Pointer to wateruse data */
                 Cell grid[],         /**< cell grid */
                 int year,            /**< year of wateruse data (AD) */
                 const Config *config /**< LPJ configuration */
                )                     /** \return TRUE on error */
{
  int cell;
  Real *data;
  data=readwateruse(wateruse,grid,year,config);
  if(data==NULL)
    return TRUE;
  for (cell=0;cell<config->ngridcell;cell++)
    grid[cell].discharge.wateruse=data[cell];
  free(data);
  return FALSE;
} /* of 'getwateruse' */

void freewateruse(Wateruse wateruse, /**< pointer to wateruse data */
                  Bool isroot        /**< task is root task (TRUE/FALSE) */
                 )
{
  if(wateruse!=NULL)
  {
    closeclimatefile(&wateruse->file,isroot);
    free(wateruse);
  }
} /* of 'freewateruse' */

#ifdef IMAGE

Bool getwateruse_wd(Wateruse wateruse,   /**< Pointer to wateruse data */
                    Cell grid[],         /**< cell grid */
                    int year,            /**< year of wateruse data (AD) */
                    const Config *config /**< LPJ configuration */
                   )                     /** \return TRUE on error */
{
  int cell;
  Real *data;
  data=readwateruse(wateruse,grid,year,config);
  if(data==NULL)
    return TRUE;
  for (cell=0;cell<config->ngridcell;cell++)
    grid[cell].discharge.wateruse_wd=data[cell];
  free(data);
  return FALSE;
} /* of 'getwateruse_wd' */

#endif
