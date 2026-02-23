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
                      Config *config            /**< LPJmL configuration */
                     )
{
  Wateruse wateruse;
  wateruse=new(struct wateruse);
  if(wateruse==NULL)
  {
    printallocerr("wateruse");
    return NULL;
  }
  if(openclimate(&wateruse->file,filename,NULL,LPJ_INT,1,1000.0,config))
  {
    free(wateruse);
    return NULL;
  }
  if(wateruse->file.time_step==DAY)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR147: Invalid daily time step in '%s', must be 1 or 12.\n",filename->name);
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
  data=newvec(Real,wateruse->file.n);
  if(data==NULL)
  {
    printallocerr("data");
    return NULL;
  }
  if(iscoupled(*config) && wateruse->file.issocket && year>=config->start_coupling)
  {
    if(readclimate(&wateruse->file,data,0,wateruse->file.scalar,grid,year,1,config))
    {
      free(data);
      data=NULL;
    }
    return data;
  }
  if(config->wateruse==ALL_WATERUSE)
  {
    /* first and last wateruse data is used outside available wateruse data */
    if(year<=wateruse->file.firstyear)
      year=wateruse->file.firstyear;
    else if(year>=wateruse->file.firstyear+wateruse->file.nyear)
      year=wateruse->file.firstyear+wateruse->file.nyear-1;
  }
  if(year>=wateruse->file.firstyear && year<wateruse->file.firstyear+wateruse->file.nyear)
  {
    if(readclimate(&wateruse->file,data,0,wateruse->file.scalar,grid,year,1,config))
    {
      free(data);
      data=NULL;
    }
  }
  else
  {
    /* no wateruse data available for year, set all to zero */
    for(cell=0;cell<wateruse->file.n;cell++)
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
  int cell,m;
  Real *data;
  data=readwateruse(wateruse,grid,year,config);
  if(data==NULL)
    return TRUE;
  if(wateruse->file.time_step==YEAR)
    for (cell=0;cell<config->ngridcell;cell++)
      for (m=0;m<NMONTH;m++)
        grid[cell].discharge.wateruse[m]=data[cell];
  else
    for (cell=0;cell<config->ngridcell;cell++)
      for (m=0;m<NMONTH;m++)
        grid[cell].discharge.wateruse[m]=data[cell*NMONTH+m];
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
  int cell,m;
  Real *data;
  data=readwateruse(wateruse,grid,year,config);
  if(data==NULL)
    return TRUE;
  if(wateruse->file.time_step==YEAR)
    for (cell=0;cell<config->ngridcell;cell++)
      for (m=0;m<NMONTH;m++)
        grid[cell].discharge.wateruse_wd[m]=data[cell];
  else
    for (cell=0;cell<config->ngridcell;cell++)
      for (m=0;m<NMONTH;m++)
        grid[cell].discharge.wateruse_wd[m]=data[cell*NMONTH+m];
  free(data);
  return FALSE;
} /* of 'getwateruse_wd' */
#endif
