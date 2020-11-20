/**************************************************************************************/
/**                                                                                \n**/
/**                f  w  r  i  t  e  o  u  t  p  u  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes daily, monthly or annual output to file                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "agriculture.h"

#define iswrite(output,index) (isopen(output,index) && config->outnames[index].timestep==timestep)

#define writeoutputvar(index,name) if(iswrite(output,index))\
  {\
    count=0;\
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)\
        vec[count++]=(float)(grid[cell].output.name);\
    writedata(output,index,vec,year,date,ndata,config);\
  }

#define writeoutputvar2(index,name,n) if(iswrite(output,index))\
  {\
    outindex(output,index,config->rank);\
    for(i=0;i<n;i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          vec[count++]=(float)grid[cell].output.name[i];\
      writepft(output,index,vec,year,date,ndata,i,n,config);\
    }\
  }

#define writeoutputvaritem(index,name,item,n) if(iswrite(output,index))\
  {\
    outindex(output,index,config->rank);\
    for(i=0;i<n;i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          vec[count++]=(float)grid[cell].output.name[i].item;\
      writepft(output,index,vec,year,date,ndata,i,n,config);\
    }\
  }

#define writeoutputshortvar(index,name,n) if(iswrite(output,index))\
  {\
    outindex(output,index,config->rank);\
    svec=newvec(short,config->count);\
    check(svec);\
    for(i=0;i<n;i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          svec[count++]=(short)grid[cell].output.name[i];\
      writeshortpft(output,index,svec,year,date,ndata,i,n,config);\
    }\
    free(svec);\
  }

static void outindex(Outputfile *output,int index,int rank)
{
  if(output->method==LPJ_SOCKET && rank==0)
    writeint_socket(output->socket,&index,1);
} /* of 'outindex' */

static void writedata(Outputfile *output,int index,float data[],int year,int date,int ndata,
                      const Config *config)
{
  int i,offset;
#ifdef USE_MPI
  MPI_Status status;
#endif
  for(i=0;i<config->count;i++)
    data[i]=(float)(config->outnames[index].scale*data[i]+config->outnames[index].offset);
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
         ((year-config->outputyear)*ndata+date)*config->total+config->offset,data,config->count,
                        MPI_FLOAT,&status);
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
          if(output->files[index].oneyear)
          {
            if(config->outnames[index].timestep==ANNUAL)
              offset=NO_TIME;
            else
              offset=date;
          }
          else
            offset=(year-config->outputyear)*ndata+date;
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,config->total,
                           offset,
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
  if(output->method==LPJ_FILES)
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
        if(output->files[index].oneyear)
        {
          if(config->outnames[index].timestep==ANNUAL)
            offset=NO_TIME;
          else
            offset=date;
        }
        else
          offset=(year-config->outputyear)*ndata+date;
        write_float_netcdf(&output->files[index].fp.cdf,data,
                           offset,
                           config->count);
        break;
    }
  else
  {
    writeint_socket(output->socket,&index,1);
    writefloat_socket(output->socket,data,config->count);
  }
#endif
} /* of 'writedata' */

static void writeshortdata(Outputfile *output,int index,short data[],int year,int date,int ndata,
                             const Config *config)
{
  int offset;
#ifdef USE_MPI
  MPI_Status status;
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
         ((year-config->outputyear)*ndata+date)*config->total+config->offset,data,config->count,
                        MPI_SHORT,&status);
      break;
    case LPJ_GATHER:
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          mpi_write(output->files[index].fp.file,data,MPI_SHORT,config->total,
                    output->counts,output->offsets,config->rank,config->comm);
          break;
        case TXT:
          mpi_write_txt(output->files[index].fp.file,data,MPI_SHORT,config->total,
                        output->counts,output->offsets,config->rank,config->comm);
          break;
        case CDF:
          if(output->files[index].oneyear)
          {
            if(config->outnames[index].timestep==ANNUAL)
              offset=NO_TIME;
            else
              offset=date;
          }
          else
            offset=(year-config->outputyear)*ndata+date;
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_SHORT,config->total,
                           offset,
                           output->counts,output->offsets,config->rank,config->comm);
          break;
      }
      break;
    case LPJ_SOCKET:
      if(isroot(*config))
        writeint_socket(output->socket,&index,1);
      mpi_write_socket(output->socket,data,MPI_SHORT,config->total,
                       output->counts,output->offsets,config->rank,config->comm);
      break;
  } /* of switch */
#else
  int i;
  if(output->method==LPJ_FILES)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        fwrite(data,sizeof(short),config->count,output->files[index].fp.file);
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[index].fp.file,"%d ",data[i]);
        fprintf(output->files[index].fp.file,"%d\n",data[config->count-1]);
        break;
      case CDF:
        if(output->files[index].oneyear)
        {
          if(config->outnames[index].timestep==ANNUAL)
            offset=NO_TIME;
          else
            offset=date;
        }
        else
          offset=(year-config->outputyear)*ndata+date;
        write_short_netcdf(&output->files[index].fp.cdf,data,
                           offset,
                           config->count);
        break;
    }
  else
  {
    writeint_socket(output->socket,&index,1);
    writeshort_socket(output->socket,data,config->count);
  }
#endif
} /* of 'writeshortdata' */

static void writealldata(Outputfile *output,int index,float data[],int year,int date,int ndata,
                         const Config *config)
{
  int i,offset;
#ifdef USE_MPI
  int *counts,*offsets;
  MPI_Status status;
#endif
  for(i=0;i<config->ngridcell;i++)
    data[i]=(float)(config->outnames[index].scale*data[i]+config->outnames[index].offset);
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
        ((year-config->outputyear)*ndata+date)*config->nall+config->offset,data,config->ngridcell,
                        MPI_FLOAT,&status);
      break;
    case LPJ_GATHER:
      counts=newvec(int,config->ntask);
      check(counts);
      offsets=newvec(int,config->ntask);
      check(offsets);
      getcounts(counts,offsets,config->nall,1,config->ntask);
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          mpi_write(output->files[index].fp.file,data,MPI_FLOAT,config->nall,counts,
                    offsets,config->rank,config->comm);
          break;
        case TXT:
          mpi_write_txt(output->files[index].fp.file,data,MPI_FLOAT,config->nall,counts,
                        offsets,config->rank,config->comm);
          break;
        case CDF:
          if(output->files[index].oneyear)
          {
            if(config->outnames[index].timestep==ANNUAL)
              offset=NO_TIME;
            else
              offset=date;
          }
          else
            offset=(year-config->outputyear)*ndata+date;
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,config->nall,
                           offset,
                           counts,offsets,config->rank,config->comm);
          break;
      }
      free(counts);
      free(offsets);
      break;
    case LPJ_SOCKET:
      counts=newvec(int,config->ntask);
      check(counts);
      offsets=newvec(int,config->ntask);
      check(offsets);
      getcounts(counts,offsets,config->nall,1,config->ntask);
      if(isroot(*config))
        writeint_socket(output->socket,&index,1);
      mpi_write_socket(output->socket,data,MPI_FLOAT,config->nall,counts,
                       offsets,config->rank,config->comm);
      free(counts);
      free(offsets);
      break;
  } /* of switch */
#else
  if(output->method==LPJ_FILES)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        fwrite(data,sizeof(float),config->ngridcell,output->files[index].fp.file);
        break;
      case TXT:
        for(i=0;i<config->ngridcell-1;i++)
          fprintf(output->files[index].fp.file,"%g ",data[i]);
        fprintf(output->files[index].fp.file,"%g\n",data[config->ngridcell-1]);
        break;
      case CDF:
        if(output->files[index].oneyear)
        {
          if(config->outnames[index].timestep==ANNUAL)
            offset=NO_TIME;
          else
            offset=date;
        }
        else
          offset=(year-config->outputyear)*ndata+date;
        write_float_netcdf(&output->files[index].fp.cdf,data,
                           offset,
                           config->ngridcell);
        break;
    }
  else
  {
    writeint_socket(output->socket,&index,1);
    writefloat_socket(output->socket,data,config->ngridcell);
  }
#endif
} /* of 'writealldata' */

static void writepft(Outputfile *output,int index,float *data,int year,
                     int date,int ndata,int layer,int nlayer,const Config *config)
{
  int i,offset;
#ifdef USE_MPI
  MPI_Status status;
#endif
  for(i=0;i<config->count;i++)
    data[i]=config->outnames[index].scale*data[i]+config->outnames[index].offset;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
                        ((year-config->outputyear)*ndata*nlayer+date*nlayer+layer)*config->total+config->offset,
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
          if(output->files[index].oneyear)
          {
            if(config->outnames[index].timestep==ANNUAL)
              offset=NO_TIME;
            else
              offset=date;
          }
          else
            offset=(year-config->outputyear)*ndata+date;
          mpi_write_pft_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,
                               config->total,offset,layer,
                               output->counts,output->offsets,config->rank,
                               config->comm);
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
        if(output->files[index].oneyear)
        {
          if(config->outnames[index].timestep==ANNUAL)
            offset=NO_TIME;
          else
            offset=date;
        }
        else
          offset=(year-config->outputyear)*ndata+date;
        write_pft_float_netcdf(&output->files[index].fp.cdf,data,
                               offset,layer,config->count);
        break;
    }
#endif
} /* of 'writepft' */

static void writeshortpft(Outputfile *output,int index,short *data,int year,
                          int date,int ndata,int layer,int nlayer,const Config *config)
{
  int i,offset;
#ifdef USE_MPI
  MPI_Status status;
#endif
  for(i=0;i<config->count;i++)
    data[i]=(short)(config->outnames[index].scale*data[i]+config->outnames[index].offset);
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
                        ((year-config->outputyear)*ndata*nlayer+date*nlayer+layer)*config->total+config->offset,
                        data,config->count,MPI_SHORT,&status);
      break;
    case LPJ_GATHER:
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          mpi_write(output->files[index].fp.file,data,MPI_SHORT,config->total,
                    output->counts,output->offsets,config->rank,config->comm);
          break;
        case TXT:
          mpi_write_txt(output->files[index].fp.file,data,MPI_SHORT,config->total,
                        output->counts,output->offsets,config->rank,config->comm);
          break;
        case CDF:
          if(output->files[index].oneyear)
          {
            if(config->outnames[index].timestep==ANNUAL)
              offset=NO_TIME;
            else
              offset=date;
          }
          else
            offset=(year-config->outputyear)*ndata+date;
          mpi_write_pft_netcdf(&output->files[index].fp.cdf,data,MPI_SHORT,
                               config->total,offset,layer,
                               output->counts,output->offsets,config->rank,
                               config->comm);
          break;
      }
      break;
    case LPJ_SOCKET:
      if(isroot(*config))
        writeint_socket(output->socket,&index,1);
      mpi_write_socket(output->socket,data,MPI_SHORT,config->total,
                       output->counts,output->offsets,config->rank,config->comm);
      break;
  } /* of switch */
#else
  if(output->method==LPJ_SOCKET)
  {
    writeint_socket(output->socket,&index,1);
    writeshort_socket(output->socket,data,config->count);
  }
  else
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(data,sizeof(short),config->count,output->files[index].fp.file)!=config->count)
          fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[index].fp.file,"%d ",data[i]);
        fprintf(output->files[index].fp.file,"%d\n",data[config->count-1]);
        break;
      case CDF:
        if(output->files[index].oneyear)
        {
          if(config->outnames[index].timestep==ANNUAL)
            offset=NO_TIME;
          else
            offset=date;
        }
        else
          offset=(year-config->outputyear)*ndata+date;
        write_pft_short_netcdf(&output->files[index].fp.cdf,data,
                               offset,layer,config->count);
        break;
    }
#endif
} /* of 'writeshortpft' */

void fwriteoutput(Outputfile *output,  /**< output file array */
                  const Cell grid[],   /**< grid cell array */
                  int year,            /**< simulation year (AD) */
                  int date,            /**< day or month */
                  int timestep,        /**< time step (ANNUAL,MONTHLY,DAILY) */
                  int npft,            /**< number of natural PFTs */
                  int ncft,            /**< number of crop PFTs */
                  const Config *config /**< LPJ configuration */
                 )
{
  int i,count,s,p,cell,l,index,ndata;
  Real ndate1;
  const Stand *stand;
  const Pft *pft;
  const Pfttree *tree;
  const Pftgrass *grass;
  const Irrigation *data;
  float *vec;
  short *svec;
  switch(timestep)
  {
    case ANNUAL:
      ndate1=1./NDAYYEAR;
      ndata=1;
      break;
    case MONTHLY:
      ndate1=ndaymonth1[date];
      ndata=NMONTH;
      break;
    case DAILY:
      ndate1=1.;
      ndata=NDAYYEAR;
      break;
  }
  if(iswrite(output,SEASONALITY))
  {
    count=0;
    svec=newvec(short,config->ngridcell);
    check(svec);
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        svec[count++]=(short)(grid[cell].ml.seasonality_type);
    writeshortdata(output,SEASONALITY,svec,year,date,ndata,config);
    free(svec);
  }
  vec=newvec(float,config->ngridcell);
  check(vec);
  writeoutputvar(NPP,npp);
  writeoutputvar(GPP,gpp);
  writeoutputvar(RH,rh);
  writeoutputvar(EVAP,evap);
  writeoutputvar(INTERC,interc);
  writeoutputvar(TRANSP,transp);
  writeoutputvar(PET,pet);
  writeoutputvar(RUNOFF,runoff);
  writeoutputvar(LITFALLC,alittfall.carbon);
  writeoutputvar(LITFALLN,alittfall.nitrogen);
  writeoutputvar(FIREC,fire.carbon);
  writeoutputvar(FIREN,fire.nitrogen);
  writeoutputvar(FLUX_FIREWOOD,flux_firewood.carbon);
  writeoutputvar(FIREF,firef);
  if(iswrite(output,VEGC))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          /*if(stand->type->landusetype==NATURAL) */
            foreachpft(pft,p,&stand->pftlist)
               vec[count]+=(float)(vegc_sum(pft)*stand->frac);
        count++;
      }
    writedata(output,VEGC,vec,year,date,ndata,config);
  }
  if(iswrite(output,SOILC))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          for(p=0;p<stand->soil.litter.n;p++)
            vec[count]+=(float)(stand->soil.litter.bg[p].carbon*stand->frac);
          forrootsoillayer(l)
            vec[count]+=(float)((stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac);
        }
        count++;
      }
    writedata(output,SOILC,vec,year,date,ndata,config);
  }
  if(iswrite(output,SOILC_SLOW))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          forrootsoillayer(l)
            vec[count]+=(float)((stand->soil.pool[l].slow.carbon)*stand->frac);
          vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);
        }
        count++;
      }
    writedata(output,SOILC_SLOW,vec,year,date,ndata,config);
  }
  if(iswrite(output,LITC))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          /* if(stand->type->landusetype==NATURAL) */
            vec[count]+=(float)(litter_ag_sum(&stand->soil.litter)*stand->frac);
        count++;
      }
      writedata(output,LITC,vec,year,date,ndata,config);
  }
  if(iswrite(output,VEGN))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          /*if(stand->type->landusetype==NATURAL) */
            foreachpft(pft,p,&stand->pftlist)
               vec[count]+=(float)(vegn_sum(pft)*stand->frac);
        count++;
      }
    writedata(output,VEGN,vec,year,date,ndata,config);
  }
  if(iswrite(output,SOILN))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          for(p=0;p<stand->soil.litter.n;p++)
            vec[count]+=(float)(stand->soil.litter.bg[p].nitrogen*stand->frac);
          forrootsoillayer(l)
            vec[count]+=(float)((stand->soil.pool[l].slow.nitrogen+stand->soil.pool[l].fast.nitrogen)*stand->frac);
        }
        count++;
      }
    writedata(output,SOILN,vec,year,date,ndata,config);
  }
  if(iswrite(output,SOILN_SLOW))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          forrootsoillayer(l)
            vec[count]+=(float)((stand->soil.pool[l].slow.nitrogen)*stand->frac);
          /*vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);*/
        }
        count++;
      }
    writedata(output,SOILN_SLOW,vec,year,date,ndata,config);
  }
  if(iswrite(output,LITN))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          /* if(stand->type->landusetype==NATURAL) */
            vec[count]+=(float)(litter_ag_sum_n(&stand->soil.litter)*stand->frac);
        count++;
      }
      writedata(output,LITN,vec,year,date,ndata,config);
  }
  if(iswrite(output,SOILNO3))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          forrootsoillayer(l)
          {
            if(stand->soil.mean_maxthaw>=layerbound[l])
               vec[count]+=(float)((stand->soil.NO3[l])*stand->frac);
          /*vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);*/
          }
        }
        count++;
      }
    writedata(output,SOILNO3,vec,year,date,ndata,config);
  }
  if(iswrite(output,SOILNH4))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          forrootsoillayer(l)
          {
            if(stand->soil.mean_maxthaw>=layerbound[l])
              vec[count]+=(float)((stand->soil.NH4[l])*stand->frac);
            /*vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);*/
          }
        }
        count++;
      }
      writedata(output,SOILNH4,vec,year,date,ndata,config);
  }
  if(iswrite(output,MAXTHAW_DEPTH))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          vec[count]+=(float)(stand->soil.maxthaw_depth*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));
        count++;
      }
      writedata(output,MAXTHAW_DEPTH,vec,year,date,ndata,config);
  }
  writeoutputvar(FLUX_ESTABC,flux_estab.carbon);
  writeoutputvar(FLUX_ESTABN,flux_estab.nitrogen);
  writeoutputvar(HARVESTC,flux_harvest.carbon);
  writeoutputvar(HARVESTN,flux_harvest.nitrogen);
  writeoutputvar(RHARVEST_BURNTC,flux_rharvest_burnt.carbon);
  writeoutputvar(RHARVEST_BURNTN,flux_rharvest_burnt.nitrogen);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDC,flux_rharvest_burnt_in_field.carbon);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDN,flux_rharvest_burnt_in_field.nitrogen);
  if(iswrite(output,MG_VEGC))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          if(stand->type->landusetype!=NATURAL)
            foreachpft(pft,p,&stand->pftlist)
              vec[count]+=(float)(vegc_sum(pft)*stand->frac);
        count++;
      }
      writedata(output,MG_VEGC,vec,year,date,ndata,config);
  }
  if(iswrite(output,MG_SOILC))
  {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          vec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype!=NATURAL)
            {
              for(p=0;p<stand->soil.litter.n;p++)
                vec[count]+=(float)(stand->soil.litter.bg[p].carbon*stand->frac);
              forrootsoillayer(l)
                vec[count]+=(float)((stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac);
            }
          }
          count++;
        }
      writedata(output,MG_SOILC,vec,year,date,ndata,config);
  }
  if(iswrite(output,MG_LITC))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          if(stand->type->landusetype!=NATURAL)
            vec[count]+=(float)(litter_ag_sum(&stand->soil.litter)*stand->frac);
        count++;
      }
    writedata(output,MG_LITC,vec,year,date,ndata,config);
  }
  writeoutputvar(INPUT_LAKE,input_lake*1e-9);
  if(iswrite(output,ADISCHARGE))
  {
    for(cell=0;cell<config->ngridcell;cell++)
      vec[cell]=(float)(grid[cell].output.adischarge*1e-9);
    writealldata(output,ADISCHARGE,vec,year,date,ndata,config);
  }
  writeoutputvar(DEFOREST_EMIS,deforest_emissions.carbon);
  writeoutputvar(TRAD_BIOFUEL,trad_biofuel);
  writeoutputvar(FBURN,fburn);
  writeoutputvar(FTIMBER,ftimber);
  writeoutputvar(TIMBER_HARVESTC,timber_harvest.carbon);
#ifdef IMAGE
  writeoutputvar(PRODUCT_POOL_FAST,product_pool_fast);
  writeoutputvar(PRODUCT_POOL_SLOW,product_pool_slow);
  writeoutputvar(PROD_TURNOVER,prod_turnover);
#else
  writeoutputvar(PRODUCT_POOL_FAST,product_pool_fast.carbon);
  writeoutputvar(PRODUCT_POOL_SLOW,product_pool_slow.carbon);
  writeoutputvar(PROD_TURNOVER,prod_turnover.carbon);
#endif
  if(iswrite(output,AFRAC_WD_UNSUST))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
         vec[count++]=(float)(grid[cell].balance.awd_unsustainable/((grid[cell].balance.airrig+
            grid[cell].balance.aconv_loss_evap + grid[cell].balance.aconv_loss_drain)*grid[cell].coord.area));
    writedata(output,AFRAC_WD_UNSUST,vec,year,date,ndata,config);
  }
  writeoutputvar(WATERUSE_HIL,awateruse_hil);
  if(iswrite(output,AGB))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          foreachpft(pft,p,&stand->pftlist)
            vec[count]+=(float)(agb(pft)*stand->frac);
        count++;
      }
    writedata(output,AGB,vec,year,date,ndata,config);
  }
  if(iswrite(output,AGB_TREE))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
          foreachpft(pft,p,&stand->pftlist)
            if(istree(pft))
              vec[count]+=(float)(agb_tree(pft)*stand->frac);
        count++;
      }
    writedata(output,AGB_TREE,vec,year,date,ndata,config);
  }
  writeoutputvar(NEGC_FLUXES,neg_fluxes.carbon);
  writeoutputvar(NEGN_FLUXES,neg_fluxes.nitrogen);
  writeoutputvar(MEAN_VEGC_MANGRASS,mean_vegc_mangrass*ndate1);
  writeoutputvar(VEGC_AVG,vegc_avg*ndate1);
  writeoutputvar(RH_LITTER,mrh_litter);
  writeoutputvar(DISCHARGE,mdischarge*1e-9*ndate1);     /* daily mean discharge per month in 1.000.000 m3 per cell */
  writeoutputvar(WATERAMOUNT,mwateramount*1e-9*ndate1); /* mean wateramount per month in 1.000.000 m3 per cell */
  writeoutputvar(TRANSP_B,mtransp_b);
  writeoutputvar(EVAP_B,mevap_b);
  writeoutputvar(INTERC_B,minterc_b);
  if(iswrite(output,SWC))
    for(l=0;l<NSOILLAYER;l++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(float)(grid[cell].output.mswc[l]*ndate1);
      writepft(output,SWC,vec,year,date,ndata,l,NSOILLAYER,config);
    }
  writeoutputvar(SWC1,mswc2[0]*ndate1);
  writeoutputvar(SWC2,mswc2[1]*ndate1);
  writeoutputvar(SWC3,mswc2[2]*ndate1);
  writeoutputvar(SWC4,mswc2[3]*ndate1);
  writeoutputvar(SWC5,mswc2[4]*ndate1);
  writeoutputvar(ROOTMOIST,mrootmoist*ndate1);
  writeoutputvar(IRRIG,irrig);
  writeoutputvar(RETURN_FLOW_B,mreturn_flow_b);
  writeoutputvar(EVAP_LAKE,mevap_lake);
  writeoutputvar(EVAP_RES,mevap_res);
  writeoutputvar(PREC_RES,mprec_res);
  writeoutputvar(NFIRE,mnfire);
  writeoutputvar(FIREDI,mfiredi*ndate1);
  writeoutputvar(FIREEMISSION_CO2,mfireemission.co2);
  writeoutputvar(FIREEMISSION_CO,mfireemission.co);
  writeoutputvar(FIREEMISSION_CH4,mfireemission.ch4);
  writeoutputvar(FIREEMISSION_VOC,mfireemission.voc);
  writeoutputvar(FIREEMISSION_TPM,mfireemission.tpm);
  writeoutputvar(FIREEMISSION_NOX,mfireemission.nox);
  writeoutputvar(BURNTAREA,burntarea);
  writeoutputvar(TEMP_IMAGE,mtemp_image);
  writeoutputvar(PREC_IMAGE,mprec_image);
  writeoutputvar(SUN_IMAGE,msun_image);
  writeoutputvar(WET_IMAGE,mwet_image);
  if(iswrite(output,SOILTEMP))
    for(l=0;l<NSOILLAYER;l++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(float)(grid[cell].output.msoiltemp[l]*ndate1);
      writepft(output,SOILTEMP,vec,year,date,ndata,l,NSOILLAYER,config);
    }
  writeoutputvar(SOILTEMP1,msoiltemp2[0]*ndate1);
  writeoutputvar(SOILTEMP2,msoiltemp2[1]*ndate1);
  writeoutputvar(SOILTEMP3,msoiltemp2[2]*ndate1);
  writeoutputvar(SOILTEMP4,msoiltemp2[3]*ndate1);
  writeoutputvar(SOILTEMP5,msoiltemp2[4]*ndate1);
  writeoutputvar(SOILTEMP6,msoiltemp2[5]*ndate1);
  writeoutputvar(NUPTAKE,mn_uptake);
  writeoutputvar(LEACHING,mn_leaching);
  writeoutputvar(N2O_DENIT,mn2o_denit);
  writeoutputvar(N2O_NIT,mn2o_nit);
  writeoutputvar(N2_EMIS,mn2_emissions);
  writeoutputvar(BNF,mbnf);
  writeoutputvar(N_MINERALIZATION,mn_mineralization);
  writeoutputvar(N_VOLATILIZATION,mn_volatilization);
  writeoutputvar(N_IMMO,mn_immo);
  writeoutputvar(RES_STORAGE,mres_storage*1e-9*ndate1);  /* mean monthly reservoir storage in 1.000.000 m3 per cell */
  writeoutputvar(RES_DEMAND,mres_demand);
  writeoutputvar(TARGET_RELEASE,mtarget_release);
  writeoutputvar(RES_CAP,mres_cap);
  writeoutputvar(WD_UNSUST,mwd_unsustainable);
  writeoutputvar(UNMET_DEMAND,munmet_demand);
  writeoutputvar(WD_LOCAL,mwd_local);
  writeoutputvar(WD_NEIGHB,mwd_neighb);
  writeoutputvar(WD_RES,mwd_res);
  writeoutputvar(WD_RETURN,mwd_return);
  writeoutputvar(CONV_LOSS_EVAP,mconv_loss_evap);
  writeoutputvar(CONV_LOSS_DRAIN,mconv_loss_drain);
  writeoutputvar(STOR_RETURN,mstor_return);
  writeoutputvar(PREC,mprec);
  writeoutputvar(RAIN,mrain);
  writeoutputvar(SNOWF,msnowf);
  writeoutputvar(MELT,mmelt);
  writeoutputvar(SWE,mswe*ndate1);
  writeoutputvar(SNOWRUNOFF,msnowrunoff);
  writeoutputvar(RUNOFF_SURF,mrunoff_surf);
  writeoutputvar(RUNOFF_LAT,mrunoff_lat);
  writeoutputvar(SEEPAGE,mseepage);
  writeoutputvar(FAPAR,fapar*ndate1);
  writeoutputvar(ALBEDO,malbedo*ndate1);
  writeoutputvar(PHEN_TMIN,mphen_tmin*ndate1);
  writeoutputvar(PHEN_TMAX,mphen_tmax*ndate1);
  writeoutputvar(PHEN_LIGHT,mphen_light*ndate1);
  writeoutputvar(PHEN_WATER,mphen_water*ndate1);
  writeoutputvar(WSCAL,mwscal*ndate1);
  writeoutputvar(GCONS_RF,mgcons_rf);
  writeoutputvar(GCONS_IRR,mgcons_irr);
  writeoutputvar(BCONS_IRR,mbcons_irr);
  writeoutputvar(IRRIG_RW,mirrig_rw);
  writeoutputvar(LAKEVOL,mlakevol);
  writeoutputvar(LAKETEMP,mlaketemp);
  writeoutputshortvar(SDATE,sdate,2*ncft);
  writeoutputshortvar(HDATE,hdate,2*ncft);
  writeoutputvar2(PFT_NPP,pft_npp,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  writeoutputvar2(PFT_NUPTAKE,pft_nuptake,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  writeoutputvar2(PFT_NDEMAND,pft_ndemand,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  if(iswrite(output,PFT_NLIMIT))
  {
    outindex(output,PFT_NLIMIT,config->rank);
    for (i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                  vec[count] = (float)(pft->nlimit*ndate1);
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_NLIMIT,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_nlimit[p];
      }
      writepft(output,PFT_NLIMIT,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_VEGC))
  {
    outindex(output,PFT_VEGC,config->rank);
    for (i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                  vec[count] = (float)vegc_sum(pft);
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_VEGC,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_veg[p].carbon;
      }
      writepft(output,PFT_VEGC,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_VEGN))
  {
    outindex(output,PFT_VEGN,config->rank);
    for (i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                  vec[count] = (float)vegn_sum(pft);
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_VEGN,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_veg[p].nitrogen;
      }
      writepft(output,PFT_VEGN,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_CLEAF))
  {
    outindex(output,PFT_CLEAF,config->rank);
    for (i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                {
                  if (pft->par->type == TREE)
                  {
                    tree = pft->data;
                    vec[count] = (float)tree->ind.leaf.carbon;
                  }
                  else
                  {
                    grass = pft->data;
                    vec[count] = (float)grass->ind.leaf.carbon;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_CLEAF,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_leaf[p].carbon;
      }
      writepft(output,PFT_CLEAF,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_NLEAF))
  {
    outindex(output,PFT_NLEAF,config->rank);
    for (i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                {
                  if (pft->par->type == TREE)
                  {
                    tree = pft->data;
                    vec[count] = (float)tree->ind.leaf.nitrogen;
                  }
                  else
                  {
                    grass = pft->data;
                    vec[count] = (float)grass->ind.leaf.nitrogen;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_NLEAF,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_leaf[p].nitrogen;
      }
      writepft(output,PFT_NLEAF,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_GCGP))
  {
    outindex(output,PFT_GCGP,config->rank);
    for(i=0;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          if(grid[cell].output.gcgp_count[i] > 0)
            grid[cell].output.pft_gcgp[i]/=grid[cell].output.gcgp_count[i];
          else
            grid[cell].output.pft_gcgp[i]=-9;
          vec[count++]=(float)grid[cell].output.pft_gcgp[i];
        }
      writepft(output,PFT_GCGP,vec,year,date,ndata,i,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  writeoutputvaritem(PFT_HARVESTC,pft_harvest,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvaritem(PFT_HARVESTN,pft_harvest,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvaritem(PFT_RHARVESTC,pft_harvest,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvaritem(PFT_RHARVESTN,pft_harvest,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_CONSUMP_WATER_G,cft_consump_water_g,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_CONSUMP_WATER_B,cft_consump_water_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(GROWING_PERIOD,growing_period,2*(ncft+NGRASS));
  writeoutputvar2(FPC,fpc,npft-config->nbiomass+1);
  writeoutputvar2(FPC_BFT,fpc_bft,((config->nbiomass+config->ngrass*2)*2));
  if(iswrite(output,NV_LAI))
  {
    outindex(output,NV_LAI,config->rank);
    for(i=0;i<npft-config->nbiomass;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)\
          vec[count++]=(float)(grid[cell].output.nv_lai[i]*ndate1);
      writepft(output,NV_LAI,vec,year,date,ndata,i,npft-config->nbiomass,config);
    }
  }
  if(iswrite(output,SOILC_LAYER))
  {
    outindex(output,SOILC_LAYER,config->rank);
    forrootsoillayer(i)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          vec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(i==0)
              for(p=0;p<stand->soil.litter.n;p++)
                vec[count]+=(float)(stand->soil.litter.bg[p].carbon*stand->frac);
            vec[count]+=(float)((stand->soil.pool[i].slow.carbon+stand->soil.pool[i].fast.carbon)*stand->frac);
          }
          count++;
        }
      }
      writepft(output,SOILC_LAYER,vec,year,date,ndata,i,BOTTOMLAYER,config);
    }
  }
  if(iswrite(output,SOILN_LAYER))
  {
    outindex(output,SOILN_LAYER,config->rank);
    forrootsoillayer(i)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          vec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(i==0)
              for(p=0;p<stand->soil.litter.n;p++)
                vec[count]+=(float)(stand->soil.litter.bg[p].nitrogen*stand->frac);
            vec[count]+=(float)((stand->soil.pool[i].slow.nitrogen+stand->soil.pool[i].fast.nitrogen)*stand->frac);
          }
          count++;
        }
      }
      writepft(output,SOILN_LAYER,vec,year,date,ndata,i,BOTTOMLAYER,config);
    }
  }
  if(iswrite(output,SOILNO3_LAYER))
  {
    outindex(output, SOILNO3_LAYER, config->rank);
    forrootsoillayer(i)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            vec[count] += (float)(stand->soil.NO3[i]*stand->frac);
          }
          count++;
        }
      }
      writepft(output, SOILNO3_LAYER, vec,year,date,ndata,i, BOTTOMLAYER, config);
    }
  }
  if(iswrite(output,SOILNH4_LAYER))
  {
    outindex(output, SOILNH4_LAYER, config->rank);
    forrootsoillayer(i)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            vec[count] += (float)(stand->soil.NH4[i]*stand->frac);
          }
          count++;
        }
      }
      writepft(output,SOILNH4_LAYER,vec,year,date,ndata,i,BOTTOMLAYER,config);
    }
  }
  writeoutputvar2(CFT_PET,cft_pet,2*(ncft+NGRASS));
  writeoutputvar2(CFT_TRANSP,cft_transp,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_TRANSP_B,cft_transp_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_EVAP,cft_evap,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_EVAP_B,cft_evap_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_INTERC,cft_interc,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_INTERC_B,cft_interc_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_RETURN_FLOW_B,cft_return_flow_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputshortvar(CFT_IRRIG_EVENTS,cft_irrig_events,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_NIR,cft_nir,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_TEMP,cft_temp,2*(ncft+NGRASS));
  writeoutputvar2(CFT_PREC,cft_prec,2*(ncft+NGRASS));
  writeoutputvar2(CFT_SRAD,cft_srad,2*(ncft+NGRASS));
  writeoutputvar2(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFTFRAC,cftfrac,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_AIRRIG,cft_airrig,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_FPAR,cft_fpar,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(LUC_IMAGE,cft_luc_image,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvaritem(CFT_ABOVEGBMC,cft_aboveground_biomass,carbon,2*(ncft+NGRASS));
  writeoutputvaritem(CFT_ABOVEGBMN,cft_aboveground_biomass,nitrogen,2*(ncft+NGRASS));
  /* ATTENTION! Due to allocation rules, this writes away next year's LAImax for trees and grasses */
  if(iswrite(output,PFT_LAIMAX))
  {
    outindex(output,PFT_LAIMAX,config->rank);
    for(i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if(i == pft->par->id)
                  vec[count] = (float)pft->par->lai(pft);
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_LAIMAX,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_laimax[p];
      }
      writepft(output,PFT_LAIMAX,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_NROOT))
  {
    outindex(output,PFT_NROOT,config->rank);
    for(i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                {
                  if (pft->par->type == TREE)
                  {
                    tree = pft->data;
                    vec[count] = (float)tree->ind.root.nitrogen;
                  }
                  else
                  {
                    grass = pft->data;
                    vec[count] = (float)grass->ind.root.nitrogen;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_NROOT,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_root[p].nitrogen;
      }
      writepft(output,PFT_NROOT,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_NSAPW))
  {
    outindex(output,PFT_NSAPW,config->rank);
    for(i=0;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          vec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            { /* ignoring setaside stands here */
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                      vec[count]=(float)tree->ind.sapwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    vec[count]=(float)tree->ind.sapwood.nitrogen;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_NSAPW,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);    }
  }
  if(iswrite(output,PFT_NHAWO))
  {
    outindex(output,PFT_NHAWO,config->rank);
    for(i=0;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip)
        {
          vec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            { /* ignoring setaside stands here */
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                      vec[count]=(float)tree->ind.heartwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    vec[count]=(float)tree->ind.heartwood.nitrogen;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_NHAWO,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_CROOT))
  {
    outindex(output,PFT_CROOT,config->rank);
    for(i=0;i<npft-config->nbiomass;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          vec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                {
                  if (pft->par->type == TREE)
                  {
                    tree = pft->data;
                    vec[count] = (float)tree->ind.root.carbon;
                  }
                  else
                  {
                    grass = pft->data;
                    vec[count] = (float)grass->ind.root.carbon;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_CROOT,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++)
      {
        if (!grid[cell].skip)
          vec[count++] = (float)grid[cell].output.cft_root[p].carbon;
      }
      writepft(output,PFT_CROOT,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_CSAPW))
  {
    outindex(output,PFT_CSAPW,config->rank);
    for(i=0;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip)
        {
          vec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            { /* ignoring setaside stands here */
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist){
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                      vec[count]=(float)tree->ind.sapwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    vec[count]=(float)tree->ind.sapwood.carbon;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_CSAPW,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
  if(iswrite(output,PFT_CHAWO))
  {
    outindex(output,PFT_CHAWO,config->rank);
    for(i=0;i<npft-config->nbiomass+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip)
        {
          vec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            { /* ignoring setaside stands here */
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                      vec[count]=(float)tree->ind.heartwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    vec[count]=(float)tree->ind.heartwood.carbon;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_CHAWO,vec,year,date,ndata,i,npft-config->nbiomass+2*(ncft+NGRASS+NBIOMASSTYPE),config);
    }
  }
#ifdef DOUBLE_HARVEST
  writeoutputvaritem(PFT_HARVESTC2,pft_harvest2,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvaritem(PFT_HARVESTN2,pft_harvest2,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvaritem(PFT_RHARVESTC2,pft_harvest2,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvaritem(PFT_RHARVESTN2,pft_harvest2,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(GROWING_PERIOD2,growing_period2,2*(ncft+NGRASS));
  writeoutputvar2(CFT_PET2,cft_pet2,2*(ncft+NGRASS));
  writeoutputvar2(CFT_TRANSP2,cft_transp2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_EVAP2,cft_evap2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_INTERC2,cft_interc2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_NIR2,cft_nir2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_TEMP2,cft_temp2,2*(ncft+NGRASS));
  writeoutputvar2(CFT_PREC2,cft_prec2,2*(ncft+NGRASS));
  writeoutputvar2(CFT_SRAD2,cft_srad2,2*(ncft+NGRASS));
  writeoutputvar2(CFTFRAC2,cftfrac2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar2(CFT_AIRRIG2,cft_airrig2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputshortvar(SDATE2,sdate2,2*ncft);
  writeoutputshortvar(HDATE2,hdate2,2*ncft);
  writeoutputshortvar(SYEAR,syear,2*ncft);
  writeoutputshortvar(SYEAR2,syear2,2*ncft);
  writeoutputvaritem(CFT_ABOVEGBMC2,cft_aboveground_biomass2,carbon,2*(ncft+NGRASS));
  writeoutputvaritem(CFT_ABOVEGBMN2,cft_aboveground_biomass2,nitrogen,2*(ncft+NGRASS));
#endif
  index=0;
  for(i=D_LAI;i<=D_PET;i++)
  {
    if(isopen(output,i))
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
       if(!grid[cell].skip)
         vec[count++]=(float)((Real *)(&grid[cell].output.daily))[index];
       writedata(output,index,vec,year,date,ndata,config);
    }
    index++;
  }
  free(vec);
} /* of 'fwriteoutput' */
