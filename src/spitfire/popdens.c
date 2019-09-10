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
  size_t offset;
 
  if(config->popdens_filename.name==NULL)
    return NULL;
  popdens=new(struct popdens);
  if(popdens==NULL)  
    return NULL;
  popdens->file.fmt=config->popdens_filename.fmt;
  if(config->popdens_filename.fmt==CDF)
  {
    if(opendata_netcdf(&popdens->file,config->popdens_filename.name,
                       config->popdens_filename.var,"km-2",config))
    {
      free(popdens);
      return NULL;
    }
    if(popdens->file.var_len>1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR408: Invalid number of dimensions %d in population density file '%s'.\n",
                (int)popdens->file.var_len,config->popdens_filename.name);
      free(popdens);
      return NULL;
    }
  }
  else
  { 
    if((popdens->file.file=openinputfile(&header,&popdens->file.swap,
                                         &config->popdens_filename,
                                         headername,
                                         &version,&offset,config))==NULL)
    {
      free(popdens);
      return NULL;
    }

    popdens->file.firstyear=header.firstyear;
    popdens->file.size=header.ncell*typesizes[header.datatype];
    popdens->file.scalar=header.scalar;
    popdens->file.datatype=header.datatype;
    popdens->file.nyear=header.nyear;
    if(config->popdens_filename.fmt==RAW)
      popdens->file.offset=config->startgrid*sizeof(short);
    else
    {
      if(header.nbands!=1)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR218: Number of bands=%d in population density file '%s' is not 1.\n",
                  header.nbands,config->popdens_filename.name);
        fclose(popdens->file.file);
        free(popdens);
        return NULL;
      }
      popdens->file.offset=(config->startgrid-header.firstcell)*typesizes[header.datatype]+headersize(headername,version)+offset;
    }

  
  }
  popdens->file.n=config->ngridcell;
  if((popdens->npopdens=newvec(Real,popdens->file.n))==NULL)
  {
    printallocerr("npopdens");
    if(popdens->file.fmt==CDF)
      closeclimate_netcdf(&popdens->file,isroot(*config));
    else
      fclose(popdens->file.file);
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
  year-=popdens->file.firstyear;
  if(year<0)
    year=0;
  if(year>=popdens->file.nyear)
    year=popdens->file.nyear-1;
  if(popdens->file.fmt==CDF)
    return readdata_netcdf(&popdens->file,popdens->npopdens,grid,year,config);
  if(fseek(popdens->file.file,year*popdens->file.size+popdens->file.offset,SEEK_SET))
  {
    fprintf(stderr,"ERROR184: Cannot seek to population density of year %d in getpopdens().\n",year+popdens->file.firstyear);
    return TRUE;
  }
  return readrealvec(popdens->file.file,popdens->npopdens,0,popdens->file.scalar,popdens->file.n,popdens->file.swap,popdens->file.datatype);
} /* of 'readpopdens' */

Real getpopdens(const Popdens popdens,int cell)
{
  return popdens->npopdens[cell];
}  /* of 'getpopdens' */

void freepopdens(Popdens popdens,Bool isroot)
{
  if(popdens!=NULL)
  {
    if(popdens->file.fmt==CDF)
      closeclimate_netcdf(&popdens->file,isroot);
    else
      fclose(popdens->file.file);
    free(popdens->npopdens);
    free(popdens);
  }
} /* of 'freepopdens' */
