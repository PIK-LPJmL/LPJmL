/**************************************************************************************/
/**                                                                                \n**/
/**  f  w  r  i  t  e  o  u  t  p  u  t  _  d  a  i  l  y  2  .  c                 \n**/
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

static void writeday(Outputfile *output,int index,float *data,int year,
                     const Config *config)
{
  int i,day;
#ifdef USE_MPI
  MPI_Status status;
#endif
  if(output->files[index].isopen)
  {
    for(i=0;i<config->count*NDAYYEAR;i++)
      data[i]*=config->outnames[index].scale;
#ifdef USE_MPI
    switch(output->method)
    {
      case LPJ_MPI2:
        for(day=0;day<NDAYYEAR;day++)
          MPI_File_write_at(output->files[index].fp.mpi_file,
                            ((long)(year-config->firstyear)*NDAYYEAR+day)*config->total+config->offset,
                            data+config->count*day,config->count,MPI_FLOAT,
                            &status);
        break;
      case LPJ_GATHER:
        switch(output->files[index].fmt)
        {
          case RAW: case CLM:
            for(day=0;day<NDAYYEAR;day++)
              mpi_write(output->files[index].fp.file,data+config->count*day,MPI_FLOAT,
                        config->total,output->counts,output->offsets,config->rank,
                        config->comm);
            break;
          case TXT:
            for(day=0;day<NDAYYEAR;day++)
              mpi_write_txt(output->files[index].fp.file,data+config->count*day,MPI_FLOAT,
                            config->total,output->counts,output->offsets,config->rank,
                            config->comm);
            break;
          case CDF:
             for(day=0;day<NDAYYEAR;day++)
                mpi_write_netcdf(&output->files[index].fp.cdf,
                                 data+config->count*day,
                                 MPI_FLOAT,
                                 config->total,
                                 output->files[index].oneyear ? day : (year-config->firstyear)*NDAYYEAR+day,
                                 output->counts,output->offsets,config->rank,
                                 config->comm);

            break;
        }
        break;
      case LPJ_SOCKET:
        for(day=0;day<NDAYYEAR;day++)
        {
          if(isroot(*config))
            writeint_socket(output->socket,&index,1);
          mpi_write_socket(output->socket,data+config->count*day,MPI_FLOAT,
                           config->total,output->counts,output->offsets,
                           config->rank,config->comm);
        }
        break;
    } /* of switch */
#else
    if(output->method==LPJ_SOCKET)
      for(day=0;day<NDAYYEAR;day++)
      {
        writeint_socket(output->socket,&index,1);
        writefloat_socket(output->socket,data+config->count*day,config->count);
      }
    else
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          if(fwrite(data,sizeof(float),config->count*NDAYYEAR,
                 output->files[index].fp.file)!=config->count*NDAYYEAR)
            fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
          break;
        case TXT:
          for(i=0;i<config->count*NDAYYEAR-1;i++)
            fprintf(output->files[index].fp.file,"%g ",data[i]);
          fprintf(output->files[index].fp.file,"%g\n",data[config->count*NDAYYEAR-1]);
          break;
        case CDF:
          for(day=0;day<NDAYYEAR;day++)
            write_float_netcdf(&output->files[index].fp.cdf,data+config->count*day,
                               output->files[index].oneyear ? day : (year-config->firstyear)*NDAYYEAR+day,
                                config->count);

          break;
      }
#endif
  }
} /* of 'writeday' */

void fwriteoutput_daily2(Outputfile *output,
                         const Outputday *data,
                         int year,
                         const Config *config)
{
  int i,index;
  index=0;
  for(i=D_LAI;i<=D_PET;i++)
    writeday(output,i,data->data[index++],year,config);
} /* of 'fwriteoutput_daily2' */
