/**************************************************************************************/
/**                                                                                \n**/
/**   f  w  r  i  t  e  o  u  t  p  u  t  _  d  a  i  l  y  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static void writeday(Outputfile *output,int index,float *data,int year,int day,
                     const Config *config)
{
#ifdef USE_MPI
  MPI_Status status;
#endif
  int i;
  for(i=0;i<config->count;i++)
    data[i]=config->outnames[index].scale*data[i]+config->outnames[index].offset;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
                        ((long)(year-config->outputyear)*NDAYYEAR+day)*config->total+config->offset,
                        data,config->count,MPI_FLOAT,&status);
      break;
    case LPJ_GATHER:
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          mpi_write(output->files[index].fp.file,data,MPI_FLOAT,config->total,
                    output->counts,output->offsets,config->rank,config->comm);
          break;
        case TXT:
          mpi_write_txt(output->files[index].fp.file,data,MPI_FLOAT,config->total,
                        output->counts,output->offsets,config->rank,config->comm);
          break;
        case CDF:
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,config->total,
                           output->files[index].oneyear ? day : (year-config->outputyear)*NDAYYEAR+day,
                           output->counts,output->offsets,config->rank,config->comm);
          break;
      }
      break;
    case LPJ_SOCKET:
      if(isroot(*config))
        writeint_socket(output->socket,&index,1);
      mpi_write_socket(output->socket,data,MPI_FLOAT,config->total,
                       output->counts,output->offsets,config->rank,config->comm);
      break;
  } /* of switch */
#else
  if(output->method==LPJ_SOCKET)
  {
    writeint_socket(output->socket,&index,1);
    writefloat_socket(output->socket,data,config->count);
  }
  else
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(data,sizeof(float),config->count,output->files[index].fp.file)!=config->count)
          fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[index].fp.file,"%g ",data[i]);
        fprintf(output->files[index].fp.file,"%g\n",data[config->count-1]);
        break;
      case CDF:
        write_float_netcdf(&output->files[index].fp.cdf,data,
                           output->files[index].oneyear ? day : (year-config->outputyear)*NDAYYEAR+day,
                           config->count);
        break;
    }
#endif
} /* of 'writeday' */

void fwriteoutput_daily(Outputfile *output, /**< Output data */
                        const Cell grid[],  /**< LPJ cell array */
                        int day,            /**< day of year (0..364) */
                        int year,           /**< year (AD) */
                        const Config *config /**< LPJmL configuration */
                       )
{
  
  int count,cell,i,index;
  float *fvec;
  fvec=newvec(float,config->count);
  check(fvec);
  index=0;
  for(i=D_LAI;i<=D_PET;i++)
  {
    if(isopen(output,i))
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
       if(!grid[cell].skip) 
         fvec[count++]=(float)((Real *)(&grid[cell].output.daily))[index];
       writeday(output,i,fvec,year,day,config);
    }
    index++;
  }
  free(fvec);
} /* of 'fwriteoutput_daily' */
