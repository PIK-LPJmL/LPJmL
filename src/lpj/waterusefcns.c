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
  Header header;
  String headername;
  int version;
  size_t offset,filesize;
  wateruse=new(struct wateruse);
  if(wateruse==NULL)
  {
    printallocerr("wateruse");
    return NULL;
  }
  wateruse->file.fmt=filename->fmt;
  if(filename->fmt==CDF)
  { 
    if(opendata_netcdf(&wateruse->file,filename,NULL,config))
    {
      free(wateruse);
      return NULL;
    }
  }
  else if(filename->fmt==SOCK)
  {
    if(isroot(*config))
    {
      send_token_copan(GET_DATA_SIZE,filename->id,config);
      wateruse->file.id=filename->id;
      header.datatype=LPJ_FLOAT;
      writeint_socket(config->socket,&header.datatype,1);
      readint_socket(config->socket,&header.nbands,1);
    }
#ifdef USE_MPI
    MPI_Bcast(&header.nbands,1,MPI_INT,0,config->comm);
#endif
    if(header.nbands==COPAN_ERR)
    {
      if(isroot(*config))
        fputs("ERROR218: Socket stream for wateruse cannot be initialized.\n",stderr);
      free(wateruse);
      return NULL;
    }
    if(header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in wateruse socket stream is not 1.\n",
                header.nbands);
      free(wateruse);
      return NULL;
    }
    wateruse->file.var_len=header.nbands;
  }
  else
  {
    if((wateruse->file.file=openinputfile(&header,&wateruse->file.swap,
                                          filename,
                                          headername,
                                          &version,&offset,TRUE,config))==NULL)
    {
      free(wateruse);
      return NULL;
    }
    wateruse->file.firstyear=header.firstyear;
    wateruse->file.nyear=header.nyear;
    if(config->wateruse_filename.fmt==RAW)
    {
      wateruse->file.datatype=LPJ_INT;
      wateruse->file.offset=sizeof(int)*config->startgrid;
    }
    else
    {
      if(version>2)
        wateruse->file.datatype=header.datatype;
      else
        wateruse->file.datatype=LPJ_INT;
      wateruse->file.offset=headersize(headername,version)+typesizes[wateruse->file.datatype]*(config->startgrid-header.firstcell)+offset;
      if(isroot(*config))
      {
         filesize=getfilesizep(wateruse->file.file)-headersize(headername,version)-offset;
         if(filesize!=typesizes[wateruse->file.datatype]*header.nyear*header.nbands*header.ncell)
           fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",filename->name);
      }
    }
    wateruse->file.var_len=header.nbands;
    wateruse->file.size=header.ncell*typesizes[wateruse->file.datatype];
    wateruse->file.scalar=(version<=1) ? 1000 : header.scalar;
  }
  if(wateruse->file.var_len!=1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Number of bands=%zu in wateruse file '%s' is not 1.\n",
              wateruse->file.var_len,config->wateruse_filename.name);

    closeclimatefile(&wateruse->file,isroot(*config));
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
  if(wateruse->file.fmt==SOCK)
  {
    data=newvec(Real,config->ngridcell);
    if(data==NULL)
    {
      printallocerr("data");
      return NULL;
    }
    if(receive_real_copan(wateruse->file.id,data,1,year,config))
    {
      fprintf(stderr,"ERROR149: Cannot receive wateruse of year %d in readwateruse().\n",year);
      fflush(stderr);
      free(data);
      return NULL;
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
    if(wateruse->file.fmt==CDF)
    {
      data=newvec(Real,config->ngridcell);
      if(data==NULL)
      {
        printallocerr("data");
        return NULL;
      }
      if(readdata_netcdf(&wateruse->file,data,grid,year,config))
      {
        free(data);
        return NULL;
      }
    }
    else
    {
      if(fseek(wateruse->file.file,
               wateruse->file.offset+wateruse->file.size*(year-wateruse->file.firstyear),
               SEEK_SET))
      {
        fprintf(stderr,"ERROR150: Cannot seek file to year %d in wateruse().\n",year);
        return NULL;
      } 
      data=newvec(Real,config->ngridcell);
      if(data==NULL)
      {
        printallocerr("data");
        return NULL;
      }
      if(readrealvec(wateruse->file.file,data,0,wateruse->file.scalar,config->ngridcell,wateruse->file.swap,wateruse->file.datatype))
      {
        fprintf(stderr,"ERROR151: Cannot read wateruse for year %d.\n",year);
        free(data);
        return NULL;
      } 
    }
  }
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
