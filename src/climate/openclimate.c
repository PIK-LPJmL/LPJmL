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
                 Real scalar,              /**< scaling factor */
                 const Config *config      /**< LPJ configuration */
                )                          /** \return TRUE on error */
{
  Header header;
  String headername;
  int last,version,count,nbands;
  char *s;
  size_t offset,filesize;
  file->fmt=filename->fmt;
  if(filename->fmt==FMS)
  {
    file->time_step=DAY;
    file->firstyear=config->firstyear;
    return FALSE;
  }
  file->issocket=filename->issocket;
  if(iscoupled(*config) && filename->issocket)
  {
    if(openinput_coupler(filename->id,LPJ_FLOAT,config->nall,&nbands,config))
      return TRUE;
    if(nbands==NDAYYEAR)
    {
      file->time_step=DAY;
      file->n=NDAYYEAR*config->ngridcell;
    }
    else if(nbands==NMONTH)
    {
      file->time_step=MONTH;
      file->n=NMONTH*config->ngridcell;
    }
    else
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR127: Invalid number of bands %d received from socket, must be 12 or 365.\n",
                nbands);
      return TRUE;
    }
    file->id=filename->id;
    if(filename->fmt==SOCK)
    {
      file->firstyear=config->firstyear;
      return FALSE;
    }
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
        count=snprintf(NULL,0,file->filename,file->firstyear);
        if(count==-1)
          return TRUE;
        s=malloc(count+1);
        check(s);
        snprintf(s,count+1,file->filename,file->firstyear);
        openclimate_netcdf(file,s,filename->time,filename->var,filename->unit,units,config);
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
      if(file->time_step==YEAR)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR438: Yearly time step not allowed in '%s'.\n",file->filename);
        return TRUE;
      }
      file->oneyear=TRUE;
      file->units=units;
      file->nyear=last-file->firstyear+1;
      if(file->filename==NULL)
        return TRUE;
      file->n=isdaily(*file) ? NDAYYEAR*config->ngridcell : NMONTH*config->ngridcell;
      file->var=filename->var;
      file->var_units=filename->unit;
      return FALSE;
    }
    else
    {
      if(mpi_openclimate_netcdf(file,filename,units,config))
        return TRUE;
      if(file->time_step==MISSING_TIME)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR436: Time axis missing in '%s'.\n",filename->name);
        return TRUE;
      }
      if(file->time_step==YEAR)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR438: Yearly time step not allowed in '%s'.\n",filename->name);
        return TRUE;
      }
      if(file->var_len>1)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s', must be 1.\n",
                  (int)file->var_len,filename->name);
        return TRUE;
      }
      return FALSE;
    }
  }
  if((file->file=openinputfile(&header,&file->swap,
                               filename,
                               headername,units,datatype,
                               &version,&offset,TRUE,config))==NULL)
    return TRUE;
  if (header.order!=CELLYEAR)
  {
    if(isroot(*config))
    {
      fprintf(stderr,"ERROR127: Order in '%s' must be cellyear, order ",filename->name);
      if(header.order>0 || header.order<=CELLSEQ)
        fprintf(stderr,"%s",ordernames[header.order-1]);
      else
        fprintf(stderr,"%d",header.order);
      fprintf(stderr," is not supported in this LPJ-Version.\n"
              "Please reorganize your input data!\n");
    }
    fclose(file->file);
    return TRUE;
  }
  if(header.timestep!=1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR127: Invalid time step %d in '%s', must be 1.\n",
              header.timestep,filename->name);
    fclose(file->file);
    return TRUE;
  }
  if(filename->fmt==META || (filename->fmt==CLM && version==4))
  {
    if(header.nbands>1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR127: Invalid number of bands %d for step=%d in '%s', must be 1.\n",
                header.nbands,header.nstep,filename->name);
      fclose(file->file);
      return TRUE;
    }
    if(header.nstep!=NMONTH && header.nstep!=NDAYYEAR)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR127: Invalid number of steps %d in '%s', must be 12 or 365.\n",
                header.nstep,filename->name);
      fclose(file->file);
      return TRUE;
    }
    header.nbands=header.nstep;
  }
  else if(filename->fmt!=RAW && header.nbands!=NMONTH && header.nbands!=NDAYYEAR)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR127: Invalid number of bands %d in '%s', must be 12 or 365.\n",
              header.nbands,filename->name);
    fclose(file->file);
    return TRUE;
  }
  file->version=version;
  file->firstyear=header.firstyear;
  file->scalar=(version<=1) ? scalar : header.scalar;
  file->nyear=header.nyear;
  file->datatype=header.datatype;
  if(filename->fmt==RAW)
  {
    header.nbands=12;
    file->offset=config->startgrid*header.nbands*typesizes[datatype];
  }
  else
  {
    file->offset=(config->startgrid-header.firstcell)*header.nbands*
                 typesizes[file->datatype]+headersize(headername,version)+offset;
    if(isroot(*config) && filename->fmt!=META)
    {
      filesize=getfilesizep(file->file)-headersize(headername,version)-offset;
      if(filesize!=typesizes[file->datatype]*header.nyear*header.nbands*header.ncell)
        fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",filename->name);
    }
  }
  file->time_step=(header.nbands==NDAYYEAR) ? DAY : MONTH;
  file->size=header.ncell*header.nbands*typesizes[file->datatype];
  file->n=header.nbands*config->ngridcell;
  file->isopen=TRUE;
  return FALSE;
} /* of 'openclimate' */
