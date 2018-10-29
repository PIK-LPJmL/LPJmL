/**************************************************************************************/
/**                                                                                \n**/
/**                  f  o  p  e  n  o  u  t  p  u  t  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function creates files for output                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <signal.h>
#include "lpj.h"

static void handler(int UNUSED(num))
{
  fail(SOCKET_ERR,FALSE,"Output channel is broken"); 
} /* of 'handler' */

static Bool create(Netcdf *cdf,const char *filename,int index,
                   Coord_array *array,const Config *config)
{
  int size;
  size=outputsize(config->outputvars[index].id,
                  config->npft[GRASS]+config->npft[TREE],
                  config->nbiomass,
                  config->npft[CROP]);
  if(size==1)
    return create_netcdf(cdf,filename,
                         (config->outputvars[index].id==GRID) ? "soilcode" :
                         config->outnames[config->outputvars[index].id].var,
                         (config->outputvars[index].id==GRID) ? "soil code" :
                         config->outnames[config->outputvars[index].id].descr,
                         (config->outputvars[index].id==GRID) ? "" :
                         config->outnames[config->outputvars[index].id].unit,
                         getoutputtype(config->outputvars[index].id),
                         getnyear(config->outputvars[index].id),array,config);
  else
    return create_pft_netcdf(cdf,filename,
                             config->outputvars[index].id,
                             config->npft[GRASS]+config->npft[TREE],
                             config->npft[CROP],
                             config->outnames[config->outputvars[index].id].var,
                             config->outnames[config->outputvars[index].id].descr,
                             config->outnames[config->outputvars[index].id].unit,
                             getoutputtype(config->outputvars[index].id),
                             getnyear(config->outputvars[index].id),
                             array,config);
} /* of 'create' */

static void openfile(Outputfile *output,const Cell grid[],
                     const char *filename,int i,
                     const Config *config)
{
  Header header;
  output->files[config->outputvars[i].id].fmt=config->outputvars[i].filename.fmt;
  if(config->outputvars[i].filename.fmt==CDF)
  {
    switch(config->outputvars[i].id)
    {
      case ADISCHARGE: case GRID:
        if(output->index_all==NULL)
        {
          output->index_all=createcoord_all(grid,config);
          if(output->index_all==NULL)
          {
            output->files[config->outputvars[i].id].isopen=FALSE;
            return;
          }
        }
        break;
      default:
        if(output->index==NULL)
        {
          output->index=createcoord(output,grid,config);
          if(output->index==NULL)
          {
            output->files[config->outputvars[i].id].isopen=FALSE;
            return;
          }
        }
        break;
    }
    if(isroot(*config))
    {
      if(!config->outputvars[i].oneyear && 
       !create(&output->files[config->outputvars[i].id].fp.cdf,filename,i,
                (config->outputvars[i].id==ADISCHARGE || config->outputvars[i].id==GRID) ? output->index_all : output->index,config))
      output->files[config->outputvars[i].id].isopen=TRUE;
    }
  }
  else if(isroot(*config) && !config->outputvars[i].oneyear)
    switch(config->outputvars[i].filename.fmt)
    {
       case CLM:
        if(config->ischeckpoint && config->outputvars[i].id!=GRID  && config->outputvars[i].id!=COUNTRY && config->outputvars[i].id!=REGION)
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"r+b"))==NULL)
            printfopenerr(config->outputvars[i].filename.name);
          else
          {
            output->files[config->outputvars[i].id].isopen=TRUE;
            if(config->checkpointyear>=config->outputyear)
            {
              fseek(output->files[config->outputvars[i].id].fp.file,
                    headersize(LPJOUTPUT_HEADER,LPJOUTPUT_VERSION)+
                    getsize(i,config)*(config->checkpointyear-config->outputyear+1),SEEK_SET);
            }
            else
              fseek(output->files[config->outputvars[i].id].fp.file,
                    headersize(LPJOUTPUT_HEADER,LPJOUTPUT_VERSION),SEEK_SET);
          }
        }
        else
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
            printfcreateerr(config->outputvars[i].filename.name);
          else
          {
            output->files[config->outputvars[i].id].isopen=TRUE;
            header.firstyear=config->outputyear;
            if(config->outputvars[i].id==ADISCHARGE)
              header.ncell=config->nall;
            else
              header.ncell=config->total;
            header.firstcell=config->firstgrid;
            header.cellsize_lon=(float)config->resolution.lon;
            header.cellsize_lat=(float)config->resolution.lat;
            header.scalar=1;
            if(config->outputvars[i].id==GRID)
            {
              header.datatype=LPJ_SHORT;
              header.nbands=2;
              header.nyear=1;
              header.order=CELLYEAR;
              fwriteheader(output->files[config->outputvars[i].id].fp.file,
                           &header,LPJGRID_HEADER,LPJGRID_VERSION);
            }
            else
            {
              header.order=CELLSEQ;
              header.nbands=getnyear(config->outputvars[i].id);
              header.nbands*=outputsize(config->outputvars[i].id,
                                        config->npft[GRASS]+config->npft[TREE],
                                        config->nbiomass,
                                        config->npft[CROP]);
              header.nyear=config->lastyear-config->outputyear+1;
              header.datatype=getoutputtype(config->outputvars[i].id);
              fwriteheader(output->files[config->outputvars[i].id].fp.file,
                           &header,LPJOUTPUT_HEADER,LPJOUTPUT_VERSION);
            }
          }
        }
        break;
      case RAW:
        if(config->ischeckpoint && config->outputvars[i].id!=GRID  && config->outputvars[i].id!=COUNTRY && config->outputvars[i].id!=REGION)
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"r+b"))==NULL)
            printfopenerr(config->outputvars[i].filename.name);
          else
          {
            output->files[config->outputvars[i].id].isopen=TRUE;
            if(config->checkpointyear>=config->outputyear)
            {
              fseek(output->files[config->outputvars[i].id].fp.file,
                    getsize(i,config)*(config->checkpointyear-config->outputyear+1),SEEK_SET);
            }
          }
        }
        else
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
            printfcreateerr(config->outputvars[i].filename.name);
          else
            output->files[config->outputvars[i].id].isopen=TRUE;
        }
        break;
      case TXT:
        if((output->files[config->outputvars[i].id].fp.file=fopen(filename,(config->ischeckpoint && config->outputvars[i].id!=GRID  && config->outputvars[i].id!=COUNTRY && config->outputvars[i].id!=REGION) ? "a" : "w"))==NULL)
          printfcreateerr(config->outputvars[i].filename.name);
        else
          output->files[config->outputvars[i].id].isopen=TRUE;
        break;
    }
  output->files[config->outputvars[i].id].oneyear=config->outputvars[i].oneyear;
} /* of 'openfile' */

Outputfile *fopenoutput(const Cell grid[], /**< LPJ grid */
                        int n,         /**< size of output file array */
                        const Config *config /**< LPJmL configuration */
                       )               /** \return output file data */
{
  int i;
#ifdef USE_MPI
  int count;
#endif
  Bool isopen;
  char *filename;
  Outputfile *output;
  output=new(Outputfile);
  check(output);
  output->method=config->outputmethod;
  output->files=newvec(File,n);
  check(output->files);
  output->n=n;
  output->index=output->index_all=NULL; 
  for(i=0;i<n;i++)
    output->files[i].isopen=FALSE;
  output->withdaily=FALSE;
#ifdef USE_MPI
  if(output->method!=LPJ_MPI2)
  {
    output->counts=newvec(int,config->ntask);
    check(output->counts);
    output->offsets=newvec(int,config->ntask);
    check(output->offsets);
    count=config->count;
    MPI_Allgather(&count,1,MPI_INT,output->counts,1,MPI_INT,
                  config->comm);
    /* calculate array offsets */
    output->offsets[0]=0;
    for(i=1;i<config->ntask;i++)
      output->offsets[i]=output->offsets[i-1]+output->counts[i-1];
  }
#endif
  if(output->method==LPJ_SOCKET)
  {
    if(isroot(*config))
    {
      output->socket=connect_socket(config->hostname,config->port,0);
      if(output->socket==NULL)
      {
        fputs("ERROR167: Cannot establish connection.\n",stderr);
        isopen=FALSE;
      }
      else
      {
#ifndef _WIN32
        signal(SIGPIPE,handler);
#endif
        isopen=TRUE;
        write_socket(output->socket,"LPJ",3);
        writeint_socket(output->socket,&config->total,1);
      }
    }
#ifdef USE_MPI
    MPI_Bcast(&isopen,1,MPI_INT,0,config->comm);
#endif
    if(!isopen)
    {
#ifdef USE_MPI
      output->counts=output->offsets=NULL;
#endif
      return output; 
    }
  }
  outputnames(output,config);
  for(i=0;i<config->n_out;i++)
  {
    if(hassuffix(config->outputvars[i].filename.name,".gz"))
    { 
      output->files[config->outputvars[i].id].compress=TRUE;
      filename=stripsuffix(config->outputvars[i].filename.name);
      check(filename);
    }
    else
    {
      output->files[config->outputvars[i].id].compress=FALSE;
      filename=config->outputvars[i].filename.name;
    }
    output->files[config->outputvars[i].id].filename=config->outputvars[i].filename.name;
#ifdef USE_MPI
    switch(output->method)
    {
      case LPJ_MPI2:
        if(MPI_File_open(config->comm,filename,
                         MPI_MODE_CREATE|MPI_MODE_WRONLY,MPI_INFO_NULL,
                         &output->files[config->outputvars[i].id].fp.mpi_file))
        {
          if(isroot(*config))
            fprintf(stderr,"ERROR100: Cannot open output file '%s'.\n",
                    filename);
        }
        else
        {
          switch(config->outputvars[i].id)
          {
            case SDATE: case HDATE: case GRID: case COUNTRY: case REGION: case SEASONALITY:
              MPI_File_set_view(output->files[config->outputvars[i].id].fp.mpi_file,
                                0, MPI_SHORT, 
                                MPI_SHORT,"native", MPI_INFO_NULL);
              break;
            default:
              MPI_File_set_view(output->files[config->outputvars[i].id].fp.mpi_file,
                                0, MPI_FLOAT,
                                MPI_FLOAT,"native", MPI_INFO_NULL);
          }
          output->files[config->outputvars[i].id].isopen=TRUE;
        }
        break;
      case LPJ_SOCKET:
        output->files[config->outputvars[i].id].isopen=TRUE;
        break;
      case LPJ_GATHER:
        openfile(output,grid,filename,i,config);
        MPI_Bcast(&output->files[config->outputvars[i].id].isopen,1,MPI_INT,
                  0,config->comm);
        break;
    } /* of 'switch' */
#else
    switch(output->method)
    {
      case LPJ_FILES:
        openfile(output,grid,filename,i,config);
        break;
      case LPJ_SOCKET:
        output->files[config->outputvars[i].id].isopen=TRUE;
        break;
    } /* of 'switch' */
#endif
    if(output->files[config->outputvars[i].id].compress)
      free(filename);
    if(output->files[config->outputvars[i].id].isopen && isdailyoutput(config->outputvars[i].id))
      output->withdaily=TRUE;
  }
  return output;
} /* of 'fopenoutput' */

void openoutput_yearly(Outputfile *output,int year,const Config *config)
{
  String filename;
  Header header;
  int i,size;
  for(i=0;i<config->n_out;i++)
    if(config->outputvars[i].oneyear)
    {
      if(isroot(*config))
      {
        snprintf(filename,STRING_LEN,config->outputvars[i].filename.name,year);
        switch(config->outputvars[i].filename.fmt)
        {
          case CLM:
            if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
            {
              printfcreateerr(filename);
              output->files[config->outputvars[i].id].isopen=FALSE;
            }
            else
            {
              output->files[config->outputvars[i].id].isopen=TRUE;
              header.firstyear=year;
              header.nyear=1;
              header.ncell=config->total;
              header.firstcell=config->firstgrid;
              header.cellsize_lon=(float)config->resolution.lon;
              header.cellsize_lat=(float)config->resolution.lat;
              header.scalar=1;
              header.order=CELLSEQ;
              header.nbands=getnyear(config->outputvars[i].id);
              if(header.nbands==1)
                header.nbands=outputsize(config->outputvars[i].id,
                                         config->npft[GRASS]+config->npft[TREE],
                                         config->nbiomass,
                                         config->npft[CROP]);
              if(config->outputvars[i].id==SDATE || config->outputvars[i].id==HDATE || config->outputvars[i].id==SEASONALITY)
                header.datatype=LPJ_SHORT;
              else
                header.datatype=LPJ_FLOAT;
              fwriteheader(output->files[config->outputvars[i].id].fp.file,
                           &header,LPJOUTPUT_HEADER,LPJOUTPUT_VERSION);

            }
            break;

          case RAW:
            if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
            {
              printfcreateerr(filename);
              output->files[config->outputvars[i].id].isopen=FALSE;
            }
            else
              output->files[config->outputvars[i].id].isopen=TRUE;
            break;
          case TXT:
            if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"w"))==NULL)
            {
              output->files[config->outputvars[i].id].isopen=FALSE;
              printfcreateerr(filename);
            }
            else
              output->files[config->outputvars[i].id].isopen=TRUE;
            break;
          case CDF:
            size=outputsize(config->outputvars[i].id,
                            config->npft[GRASS]+config->npft[TREE],
                            config->nbiomass,
                            config->npft[CROP]);
           if(size==1)
             output->files[config->outputvars[i].id].isopen=!create1_netcdf(&output->files[config->outputvars[i].id].fp.cdf,filename,
                           config->outnames[config->outputvars[i].id].var,
                           config->outnames[config->outputvars[i].id].descr,
                           config->outnames[config->outputvars[i].id].unit,
                           getoutputtype(config->outputvars[i].id),
                           getnyear(config->outputvars[i].id),
                           (config->outputvars[i].id==ADISCHARGE) ? output->index_all : output->index,year,config);
           else
             output->files[config->outputvars[i].id].isopen=!create1_pft_netcdf(&output->files[config->outputvars[i].id].fp.cdf,filename,
                           config->outputvars[i].id,
                           config->npft[GRASS]+config->npft[TREE],
                           config->npft[CROP],
                           config->outnames[config->outputvars[i].id].var,
                           config->outnames[config->outputvars[i].id].descr,
                           config->outnames[config->outputvars[i].id].unit,
                           getoutputtype(config->outputvars[i].id),
                           getnyear(config->outputvars[i].id),year,
                               output->index,config);

        }
     }
#ifdef USE_MPI
     MPI_Bcast(&output->files[config->outputvars[i].id].isopen,1,MPI_INT,0,config->comm);
#endif
     if(output->files[config->outputvars[i].id].isopen && isdailyoutput(config->outputvars[i].id))
      output->withdaily=TRUE;
   }
} /* of 'openoutput_yearly */
