/**************************************************************************************/
/**                                                                                \n**/
/**               o  p  e  n  c  l  i  m  a  t  e  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens climate data file                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool openclimate(Climatefile *file,        /**< pointer to climate file */
                 const Filename *filename, /**< file name and format */
                 const char *units,        /**< units in NetCDF file or NULL */
                 Type datatype,            /**< data type in binary file */
                 const Config *config      /**< LPJ configuration */
                )                          /** \return TRUE on error */
{
  Header header;
  String headername;
  int last,version;
  char *s;
  size_t offset;
  file->fmt=filename->fmt;
  if(filename->fmt==FMS)
  {
    file->time_step=DAY;
    file->firstyear=config->firstyear;
    return FALSE;
  }
  file->oneyear=FALSE;
  if(filename->fmt==CDF) /** file is in NetCDF format? */
  {
    s=strchr(filename->name,'[');
    if(s!=NULL && sscanf(s,"[%d-%d]",&file->firstyear,&last)==2)
    {
      /**
      * one file for each simulation year
      **/
      file->filename=mkfilename(filename->name);
      if(file->filename==NULL)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR225: Cannot parse filename '%s'.\n",filename->name);
        return TRUE;
      } 
      if(isroot(*config))
      {
        s=malloc(strlen(file->filename)+12);
        check(s);
        sprintf(s,file->filename,file->firstyear);
        openclimate_netcdf(file,s,filename->var,units,config);
        free(s);
      }
#ifdef USE_MPI
      MPI_Bcast(&file->time_step,1,MPI_INT,0,config->comm);
#endif
      closeclimate_netcdf(file,isroot(*config));
      if(file->time_step==MISSING_TIME)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR436: Time axis missing in '%s'.\n",file->filename);
        return TRUE;
      }
      file->oneyear=TRUE;
      file->units=units;
      file->nyear=last-file->firstyear+1;
      if(file->filename==NULL)
        return TRUE;
      file->n=isdaily(*file) ? NDAYYEAR*config->ngridcell : NMONTH*config->ngridcell;
      file->var=filename->var;
      return FALSE;
    }
    else
    {
      if(mpi_openclimate_netcdf(file,filename->name,filename->var,units,config))
        return TRUE;
      if(file->time_step==MISSING_TIME)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR436: Time axis missing in '%s'.\n",filename->name);
        return TRUE;
      }
      if(file->var_len>1)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s'.\n",
                  (int)file->var_len,filename->name);
        return TRUE;
      }
      return FALSE;
    }
  }
  if((file->file=openinputfile(&header,&file->swap,
                               filename,
                               headername,
                               &version,&offset,config))==NULL)
    return TRUE;
  if (header.order==YEARCELL)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR127: Order YEARCELL in '%s' is not supported in this LPJ-Version.\n"
              "Please reorganize your input data!\n",filename->name);
    fclose(file->file);
    return TRUE;
  }
  if(filename->fmt!=RAW && header.nbands!=NMONTH && header.nbands!=NDAYYEAR)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR127: Invalid number of bands %d in '%s'.\n",
              header.nbands,filename->name);
    fclose(file->file);
    return TRUE;
  }
  file->version=version;
  file->firstyear=header.firstyear;
  file->scalar=header.scalar;
  file->nyear=header.nyear;
  if(file->version<=2)
    file->datatype=datatype;
  else 
    file->datatype=header.datatype;
  if(filename->fmt==RAW)
  {
    header.nbands=12;
    file->offset=config->startgrid*header.nbands*typesizes[datatype];
  }
  else
    file->offset=(config->startgrid-header.firstcell)*header.nbands*
                 typesizes[file->datatype]+headersize(headername,version)+offset;
  file->time_step=(header.nbands==NDAYYEAR) ? DAY : MONTH;
  file->size=header.ncell*header.nbands*typesizes[file->datatype];
  file->n=header.nbands*config->ngridcell;
  return FALSE;
} /* of 'openclimate' */
