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
  Header header;
  Popdens popdens;
  String headername;
  int i,version;
  size_t offset,filesize;

  if(config->popdens_filename.name==NULL)
    return NULL;
  popdens=new(struct popdens);
  if(popdens==NULL)
  {
    printallocerr("popdens");
    return NULL;
  }
  popdens->file.fmt=config->popdens_filename.fmt;
  if(config->popdens_filename.fmt==CDF)
  {
    if(opendata_netcdf(&popdens->file,&config->popdens_filename,"km-2",config))
    {
      free(popdens);
      return NULL;
    }
  }
  else if(config->popdens_filename.fmt==SOCK)
  {
    if(openinput_copan(config->popdens_filename.id,LPJ_FLOAT,config->nall,&header.nbands,config))
    {
      free(popdens);
      return NULL;
    }
    if(header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d  in population density stream is not 1.\n",
                header.nbands);
      free(popdens);
      return NULL;
    }
    popdens->file.id=config->popdens_filename.id;
    popdens->file.var_len=header.nbands;
  }
  else
  {
    if((popdens->file.file=openinputfile(&header,&popdens->file.swap,
                                         &config->popdens_filename,
                                         headername,
                                         &version,&offset,TRUE,config))==NULL)
    {
      free(popdens);
      return NULL;
    }

    popdens->file.firstyear=header.firstyear;
    popdens->file.size=header.ncell*typesizes[header.datatype];
    popdens->file.scalar=header.scalar;
    popdens->file.datatype=header.datatype;
    popdens->file.nyear=header.nyear;
    popdens->file.var_len=header.nbands;
    if(config->popdens_filename.fmt==RAW)
      popdens->file.offset=config->startgrid*sizeof(short);
    else
    {
      popdens->file.offset=(config->startgrid-header.firstcell)*typesizes[header.datatype]+headersize(headername,version)+offset;
      if(isroot(*config) && config->popdens_filename.fmt!=META)
      {
         filesize=getfilesizep(popdens->file.file)-headersize(headername,version)-offset;
         if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
           fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->popdens_filename.name);
      }
    }
  }
  if(popdens->file.var_len>1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Number of bands=%zu in population density file '%s' is not 1.\n",
              popdens->file.var_len,config->popdens_filename.name);
    closeclimatefile(&popdens->file,isroot(*config));
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
  char *name;
  year-=popdens->file.firstyear;
  if(year<0)
    year=0;
  if(year>=popdens->file.nyear)
    year=popdens->file.nyear-1;
  if(popdens->file.fmt==CDF)
  {
    if(readdata_netcdf(&popdens->file,popdens->npopdens,grid,year,config))
    {
      fprintf(stderr,"ERROR185: Cannot read population density of year %d from '%s'.\n",
              year+popdens->file.firstyear,config->popdens_filename.name);
      return TRUE;
    }
    return FALSE;
  }
  if(popdens->file.fmt==SOCK)
  {
    if(receive_real_copan(popdens->file.id,popdens->npopdens,1,year,config))
    {
      fprintf(stderr,"ERROR149: Cannot receive population density of year %d.\n",year);
      return TRUE;
    }
    return FALSE;
  }
  if(fseek(popdens->file.file,year*popdens->file.size+popdens->file.offset,SEEK_SET))
  {
    name=getrealfilename(&config->popdens_filename);
    fprintf(stderr,"ERROR184: Cannot seek to population density of year %d in '%s'.\n",
            year+popdens->file.firstyear,name);
    free(name);
    return TRUE;
  }
  if(readrealvec(popdens->file.file,popdens->npopdens,0,popdens->file.scalar,popdens->file.n,popdens->file.swap,popdens->file.datatype))
  {
    name=getrealfilename(&config->popdens_filename);
    fprintf(stderr,"ERROR185: Cannot read population density of year %d from '%s'.\n",
            year+popdens->file.firstyear,name);
    free(name);
    return TRUE;
  }
   return FALSE;
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
