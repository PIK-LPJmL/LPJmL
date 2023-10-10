/**************************************************************************************/
/**                                                                                \n**/
/**                     r  e  a  l  d  a  t  a  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of datatype Realdata                                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct realdata
{
  Climatefile file;
  char *name;
  Real *data;
}; /* Definition of opaque datatype Realdata */

Realdata initrealdata(const Filename *filename, /**< filename */
                      const char *name,
                      const char *unit,         /**< unit */
                      const Config *config      /**< LPJ configuration */
                     )                          /** \return pointer to realdata
                                                     struct or NULL */
{
  Realdata realdata;
  int i;

  if(filename==NULL)
    return NULL;
  realdata=new(struct realdata);
  if(realdata==NULL)
  {
    printallocerr("realdata");
    return NULL;
  }
  if(opendata(&realdata->file,filename,name,unit,LPJ_FLOAT,LPJ_SHORT,1.0,1,TRUE,config))
  //if(opendata(&realdata->file,filename,name,unit,(filename->fmt==SOCK) ? LPJ_FLOAT : LPJ_SHORT,1.0,1,TRUE,config))
  {
    free(realdata);
    return NULL;
  }
  if(isroot(*config) && filename->fmt!=SOCK && config->lastyear>realdata->file.firstyear+realdata->file.nyear-1)
    fprintf(stderr,"WARNING024: Last year in real data file=%d for %s is less than last simulation year %d, data from last year used.\n",
            realdata->file.firstyear+realdata->file.nyear-1,name,config->lastyear);
  realdata->file.n=config->ngridcell;
  realdata->name=strdup(name);
  if(name==NULL)
  {
    printallocerr("name");
    closeclimatefile(&realdata->file,isroot(*config));
    free(realdata);
    return NULL;
  }
  if((realdata->data=newvec(Real,realdata->file.n))==NULL)
  {
    printallocerr("data");
    free(realdata->name);
    closeclimatefile(&realdata->file,isroot(*config));
    free(realdata);
    return NULL;
  }
  for(i=0;i<realdata->file.n;i++)
    realdata->data[i]=0;
  return realdata;
} /* of 'initrealdata' */

/*
- called in lpj()
- opens the realdata input file (see also building file for the realdata Input (like cfts26_lu2clm.c)
- sets the realdata variables (see also manage.h)
*/

Bool readrealdata(Realdata realdata,   /**< pointer to real data */
                  int year,            /**< year (AD) */
                  const Cell grid[],   /**< LPJ grid */
                  const Config *config /**< LPJ configuration */
                 )                     /** \return TRUE on error */
{
  return (readdata(&realdata->file,realdata->data,grid,realdata->name,year,config)==NULL);
} /* of 'readrealdata' */

void freerealdata(Realdata realdata,Bool isroot)
{
  if(realdata!=NULL)
  {
    closeclimatefile(&realdata->file,isroot);
    free(realdata->data);
    free(realdata->name);
    free(realdata);
  }
} /* of 'freerealdata' */


Real getrealdata(const Realdata realdata,int cell)
{
  return realdata->data[cell];
}  /* of 'getrealdata' */
