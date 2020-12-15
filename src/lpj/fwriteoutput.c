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

#define iswrite(output,index) (isopen(output,index) && iswrite2(index,timestep,year,config))

#define writeoutputvar(index,name) if(iswrite(output,index))\
  {\
    count=0;\
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)\
        vec[count++]=(float)(grid[cell].output.name);\
    writedata(output,index,vec,year,date,ndata,config);\
  }

#define writeoutputvar2(index,name,scale,n) if(iswrite(output,index))\
  {\
    outindex(output,index,config->rank);\
    for(i=0;i<n;i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          vec[count++]=(float)(grid[cell].output.name[i]*scale);\
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

static Bool iswrite2(int index,int timestep,int year,const Config *config)
{
 if(timestep==ANNUAL && config->outnames[index].timestep>0)
    return (year>=config->outputyear && (year-config->outputyear+1) % config->outnames[index].timestep==0);
  else
    return config->outnames[index].timestep==timestep;
} /* of 'iswrite2' */

static void outindex(Outputfile *output,int index,int rank)
{
  if(output->method==LPJ_SOCKET && rank==0)
    writeint_socket(output->socket,&index,1);
} /* of 'outindex' */

static Real getscale(int date,int ndata,int timestep,Time time)
{
  Real scale;
  switch(ndata)
  {
    case 1: /* annual output */
      switch(time)
      {
        case SECOND:
          scale=1./NDAYYEAR/NSECONDSDAY/timestep;
          break;
        case DAY:
          scale=1./NDAYYEAR/timestep;
          break;
        default:
          scale=1./timestep;
      }
      break;
    case NMONTH: /* monthly output */
      switch(time)
      {
        case SECOND:
          scale=ndaymonth1[date]/NSECONDSDAY;
          break;
        case DAY:
          scale=ndaymonth1[date];
          break;
        default:
           scale=1;
      }
      break;
    case NDAYYEAR: /* daily output */
      if(time==SECOND)
        scale=1/NSECONDSDAY;
      else
        scale=1;
      break;
    default:
      scale=1;
  }
  return scale;
} /* of 'getscale' */

static void writedata(Outputfile *output,int index,float data[],int year,int date,int ndata,
                      const Config *config)
{
  Real scale;
  int i,offset;
#ifdef USE_MPI
  MPI_Status status;
#endif
  scale=getscale(date,ndata,(config->outnames[index].timestep==ANNUAL) ? 1 : config->outnames[index].timestep,config->outnames[index].time);
  for(i=0;i<config->count;i++)
    data[i]=(float)(config->outnames[index].scale*scale*data[i]+config->outnames[index].offset);
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
            offset=(config->outnames[index].timestep>0) ? (year-config->outputyear)/config->outnames[index].timestep : (year-config->outputyear)*ndata+date;
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
          offset=(config->outnames[index].timestep>0) ? (year-config->outputyear)/config->outnames[index].timestep : (year-config->outputyear)*ndata+date;
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
            offset=(config->outnames[index].timestep>0) ? (year-config->outputyear)/config->outnames[index].timestep : (year-config->outputyear)*ndata+date;
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
          offset=(config->outnames[index].timestep>0) ? (year-config->outputyear)/config->outnames[index].timestep : (year-config->outputyear)*ndata+date;
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
  Real scale;
  int i,offset;
#ifdef USE_MPI
  int *counts,*offsets;
  MPI_Status status;
#endif
  scale=getscale(date,ndata,(config->outnames[index].timestep==ANNUAL) ? 1 : config->outnames[index].timestep,config->outnames[index].time);
  for(i=0;i<config->ngridcell;i++)
    data[i]=(float)(config->outnames[index].scale*scale*data[i]+config->outnames[index].offset);
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
  Real scale;
  int i,offset;
#ifdef USE_MPI
  MPI_Status status;
#endif
  scale=getscale(date,ndata,(config->outnames[index].timestep==ANNUAL) ? 1 : config->outnames[index].timestep,config->outnames[index].time);
  for(i=0;i<config->count;i++)
    data[i]=config->outnames[index].scale*scale*data[i]+config->outnames[index].offset;
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
            offset=(config->outnames[index].timestep>0) ? (year-config->outputyear)/config->outnames[index].timestep : (year-config->outputyear)*ndata+date;
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
          offset=(config->outnames[index].timestep>0) ? (year-config->outputyear)/config->outnames[index].timestep : (year-config->outputyear)*ndata+date;
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
                  Cell grid[],         /**< grid cell array */
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
    case MONTHLY:
      ndate1=ndaymonth1[date];
      ndata=NMONTH;
      break;
    case DAILY:
      ndate1=1.;
      ndata=NDAYYEAR;
      break;
    default:
      ndate1=1./NDAYYEAR;
      ndata=1;
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
  writeoutputvar(DAYLENGTH,daylength*ndate1);
  writeoutputvar(TEMP,temp*ndate1);
  writeoutputvar(SUN,sun*ndate1);
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
  if(isopen(output,VEGC))
  {
    if(iswrite2(VEGC,timestep,year,config) || (timestep==ANNUAL && config->outnames[VEGC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            foreachstand(stand,s,grid[cell].standlist)
              /*if(stand->type->landusetype==NATURAL) */
              foreachpft(pft,p,&stand->pftlist)
                grid[cell].output.veg.carbon+=vegc_sum(pft)*stand->frac;
          }
        }
    }
    writeoutputvar(VEGC,veg.carbon);
  }
  if(isopen(output,VEGN))
  {
    if(iswrite2(VEGN,timestep,year,config) || (timestep==ANNUAL && config->outnames[VEGN].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            foreachstand(stand,s,grid[cell].standlist)
              /*if(stand->type->landusetype==NATURAL) */
              foreachpft(pft,p,&stand->pftlist)
                grid[cell].output.veg.nitrogen+=vegn_sum(pft)*stand->frac;
          }
        }
    }
    writeoutputvar(VEGN,veg.carbon);
  }
  if(isopen(output,SOILC))
  {
    if(iswrite2(SOILC,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            for(p=0;p<stand->soil.litter.n;p++)
              grid[cell].output.soil.carbon+=stand->soil.litter.bg[p].carbon*stand->frac;
            forrootsoillayer(l)
              grid[cell].output.soil.carbon+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
          }
        }
    }
    writeoutputvar(SOILC,soil.carbon);
  }
  if(isopen(output,SOILN))
  {
    if(iswrite2(SOILN,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILN].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            for(p=0;p<stand->soil.litter.n;p++)
              grid[cell].output.soil.nitrogen+=stand->soil.litter.bg[p].nitrogen*stand->frac;
            forrootsoillayer(l)
              grid[cell].output.soil.nitrogen+=(stand->soil.pool[l].slow.nitrogen+stand->soil.pool[l].fast.nitrogen)*stand->frac;
          }
        }
    }
    writeoutputvar(SOILN,soil.nitrogen);
  }
  if(isopen(output,SOILC_SLOW))
  {
    if(iswrite2(SOILC_SLOW,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILC_SLOW].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            forrootsoillayer(l)
              grid[cell].output.soil_slow.carbon+=stand->soil.pool[l].slow.carbon*stand->frac;
            grid[cell].output.soil.carbon+=stand->soil.YEDOMA*stand->frac;
          }
        }
    }
    writeoutputvar(SOILC_SLOW,soil_slow.carbon);
  }
  if(isopen(output,SOILN_SLOW))
  {
    if(iswrite2(SOILN_SLOW,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILN_SLOW].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            forrootsoillayer(l)
              grid[cell].output.soil_slow.nitrogen+=stand->soil.pool[l].slow.nitrogen*stand->frac;
          }
        }
    }
    writeoutputvar(SOILN_SLOW,soil_slow.nitrogen);
  }
  if(isopen(output,LITC))
  {
    if(iswrite2(LITC,timestep,year,config) || (timestep==ANNUAL && config->outnames[LITC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          /* if(stand->type->landusetype==NATURAL) */
            grid[cell].output.litter.carbon+=litter_ag_sum(&stand->soil.litter)*stand->frac;
        }
    }
    writeoutputvar(LITC,litter.carbon);
  }
  if(isopen(output,LITN))
  {
    if(iswrite2(LITN,timestep,year,config) || (timestep==ANNUAL && config->outnames[LITN].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          /* if(stand->type->landusetype==NATURAL) */
            grid[cell].output.litter.nitrogen+=litter_ag_sum_n(&stand->soil.litter)*stand->frac;
        }
    }
    writeoutputvar(LITN,litter.nitrogen);
  }
  if(isopen(output,SOILNO3))
  {
    if(iswrite2(SOILNO3,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILNO3].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            forrootsoillayer(l)
            {
              if(stand->soil.mean_maxthaw>=layerbound[l])
                grid[cell].output.soilno3+=stand->soil.NO3[l]*stand->frac;
            /*vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);*/
            }
          }
        }
    }
    writeoutputvar(SOILNO3,soilno3);
  }
  if(isopen(output,SOILNH4))
  {
    if(iswrite2(SOILNH4,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILNH4].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            forrootsoillayer(l)
            {
              if(stand->soil.mean_maxthaw>=layerbound[l])
                grid[cell].output.soilnh4+=stand->soil.NH4[l]*stand->frac;
            /*vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);*/
            }
          }
        }
    }
    writeoutputvar(SOILNH4,soilnh4);
  }
  if(isopen(output,MAXTHAW_DEPTH))
  {
    if(iswrite2(MAXTHAW_DEPTH,timestep,year,config) || (timestep==ANNUAL && config->outnames[MAXTHAW_DEPTH].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          foreachstand(stand,s,grid[cell].standlist)
            grid[cell].output.maxthaw_depth+=stand->soil.maxthaw_depth*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    }
    writeoutputvar(MAXTHAW_DEPTH,maxthaw_depth);
  }
  writeoutputvar(FLUX_ESTABC,flux_estab.carbon);
  writeoutputvar(FLUX_ESTABN,flux_estab.nitrogen);
  writeoutputvar(HARVESTC,flux_harvest.carbon);
  writeoutputvar(HARVESTN,flux_harvest.nitrogen);
  writeoutputvar(RHARVEST_BURNTC,flux_rharvest_burnt.carbon);
  writeoutputvar(RHARVEST_BURNTN,flux_rharvest_burnt.nitrogen);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDC,flux_rharvest_burnt_in_field.carbon);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDN,flux_rharvest_burnt_in_field.nitrogen);
  if(isopen(output,MG_VEGC))
  {
    if(iswrite2(MG_VEGC,timestep,year,config) || (timestep==ANNUAL && config->outnames[MG_VEGC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype!=NATURAL)
              foreachpft(pft,p,&stand->pftlist)
                grid[cell].output.mg_vegc+=(float)(vegc_sum(pft)*stand->frac);
        }
    }
    writeoutputvar(MG_VEGC,mg_vegc);
  }
  if(isopen(output,MG_SOILC))
  {
    if(iswrite2(MG_SOILC,timestep,year,config) || (timestep==ANNUAL && config->outnames[MG_SOILC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype!=NATURAL)
            {
              for(p=0;p<stand->soil.litter.n;p++)
                grid[cell].output.mg_soilc+=stand->soil.litter.bg[p].carbon*stand->frac;
              forrootsoillayer(l)
                grid[cell].output.mg_soilc+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
            }
          }
        }
    }
    writeoutputvar(MG_SOILC,mg_soilc);
  }
  if(isopen(output,MG_LITC))
  {
    if(iswrite2(MG_LITC,timestep,year,config) || (timestep==ANNUAL && config->outnames[MG_LITC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype!=NATURAL)
              grid[cell].output.mg_litc+=litter_ag_sum(&stand->soil.litter)*stand->frac;
        }
    }
    writeoutputvar(MG_LITC,mg_litc);
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
  writeoutputvar(PRODUCT_POOL_FAST,product_pool.fast);
  writeoutputvar(PRODUCT_POOL_SLOW,product_pool.slow);
  writeoutputvar(PROD_TURNOVER,prod_turnover);
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
  writeoutputvar(WATERUSECONS,waterusecons);
  writeoutputvar(WATERUSEDEM,waterusedem);
  if(isopen(output,AGB))
  {
    if(iswrite2(AGB,timestep,year,config) || (timestep==ANNUAL && config->outnames[AGB].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            foreachpft(pft,p,&stand->pftlist)
              grid[cell].output.agb+=agb(pft)*stand->frac;
        }
    }
    writeoutputvar(AGB,agb);
  }
  if(isopen(output,AGB_TREE))
  {
    if(iswrite2(AGB_TREE,timestep,year,config) || (timestep==ANNUAL && config->outnames[AGB_TREE].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            foreachpft(pft,p,&stand->pftlist)
              if(istree(pft))
                grid[cell].output.agb_tree+=agb(pft)*stand->frac;
        }
    }
    writeoutputvar(AGB_TREE,agb_tree);
  }
  writeoutputvar(NEGC_FLUXES,neg_fluxes.carbon);
  writeoutputvar(NEGN_FLUXES,neg_fluxes.nitrogen);
  writeoutputvar(MEAN_VEGC_MANGRASS,mean_vegc_mangrass*ndate1);
  writeoutputvar(VEGC_AVG,vegc_avg*ndate1);
  writeoutputvar(RH_LITTER,rh_litter);
  writeoutputvar(DISCHARGE,mdischarge*1e-9);     /* daily mean discharge per month in 1.000.000 m3 per cell */
  writeoutputvar(WATERAMOUNT,mwateramount*1e-9); /* mean wateramount per month in 1.000.000 m3 per cell */
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
  writeoutputvar(CONV_LOSS_EVAP,mconv_loss_evap);
  writeoutputvar(CONV_LOSS_DRAIN,mconv_loss_drain);
  writeoutputvar(STOR_RETURN,mstor_return);
  writeoutputvar(PREC,prec);
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
  writeoutputvar2(PFT_NPP,pft_npp,1,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  writeoutputvar2(PFT_NUPTAKE,pft_nuptake,1,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  writeoutputvar2(PFT_NDEMAND,pft_ndemand,1,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  if(isopen(output,PFT_NLIMIT))
  {
    if(iswrite2(PFT_NLIMIT,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_NLIMIT].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
              grid[cell].output.pft_nlimit[pft->par->id]+=pft->nlimit*ndate1;
          }
        }
      }
    }
    writeoutputvar2(PFT_NLIMIT,pft_nlimit,1,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_VEGC))
  {
    if(iswrite2(PFT_VEGC,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_VEGC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
              grid[cell].output.pft_veg[pft->par->id].carbon+=vegc_sum(pft);
          }
        }
      }
    }
    writeoutputvaritem(PFT_VEGC,pft_veg,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_VEGN))
  {
    if(iswrite2(PFT_VEGN,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_VEGN].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
              grid[cell].output.pft_veg[pft->par->id].nitrogen+=vegn_sum(pft);
          }
        }
      }
    }
    writeoutputvaritem(PFT_VEGN,pft_veg,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(iswrite(output,PFT_GCGP))
  {
    outindex(output,PFT_GCGP,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
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
      writepft(output,PFT_GCGP,vec,year,date,ndata,i,(npft-config->nbiomass-config->nwft)+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),config);
    }
  }
  writeoutputvaritem(PFT_HARVESTC,pft_harvest,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_HARVESTN,pft_harvest,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTC,pft_harvest,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTN,pft_harvest,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_CONSUMP_WATER_G,cft_consump_water_g,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_CONSUMP_WATER_B,cft_consump_water_b,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(GROWING_PERIOD,growing_period,1,2*(ncft+NGRASS));
  writeoutputvar2(FPC,fpc,1,npft-config->nbiomass-config->nwft+1);
  writeoutputvar2(FPC_BFT,fpc_bft,1,((config->nbiomass+config->ngrass*2)*2));
  if(iswrite(output,NV_LAI))
  {
    outindex(output,NV_LAI,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)\
          vec[count++]=(float)(grid[cell].output.nv_lai[i]*ndate1);
      writepft(output,NV_LAI,vec,year,date,ndata,i,npft-config->nbiomass-config->nwft,config);
    }
  }
  if(isopen(output,SOILC_LAYER))
  {
    if(iswrite2(SOILC_LAYER,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILC_LAYER].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            for(p=0;p<stand->soil.litter.n;p++)
              grid[cell].output.soil_layer[0].carbon+=stand->soil.litter.bg[p].carbon*stand->frac;
            forrootsoillayer(i)
              grid[cell].output.soil_layer[i].carbon+=(stand->soil.pool[i].slow.carbon+stand->soil.pool[i].fast.carbon)*stand->frac;
          }
        }
      }
    }
    writeoutputvaritem(SOILC_LAYER,soil_layer,carbon,BOTTOMLAYER);
  }
  if(isopen(output,SOILN_LAYER))
  {
    if(iswrite2(SOILN_LAYER,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILN_LAYER].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            for(p=0;p<stand->soil.litter.n;p++)
              grid[cell].output.soil_layer[0].nitrogen+=stand->soil.litter.bg[p].nitrogen*stand->frac;
            forrootsoillayer(i)
              grid[cell].output.soil_layer[i].nitrogen+=(stand->soil.pool[i].slow.nitrogen+stand->soil.pool[i].fast.nitrogen)*stand->frac;
          }
        }
      }
    }
    writeoutputvaritem(SOILN_LAYER,soil_layer,nitrogen,BOTTOMLAYER);
  }
  if(isopen(output,SOILNO3_LAYER))
  {
    if(iswrite2(SOILNO3_LAYER,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILNO3_LAYER].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            forrootsoillayer(i)
              grid[cell].output.soilno3_layer[i]+=stand->soil.NO3[i]*stand->frac;
          }
        }
      }
    }
    writeoutputvar2(SOILNO3_LAYER,soilno3_layer,1,BOTTOMLAYER);
  }
  if(isopen(output,SOILNH4_LAYER))
  {
    if(iswrite2(SOILNH4_LAYER,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILNH4_LAYER].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            forrootsoillayer(i)
              grid[cell].output.soilnh4_layer[i]+=stand->soil.NH4[i]*stand->frac;
          }
        }
      }
    }
    writeoutputvar2(SOILNH4_LAYER,soilnh4_layer,1,BOTTOMLAYER);
  }
#if defined IMAGE || defined INCLUDEWP
  writeoutputvar2(WFT_VEGC,wft_vegc,1,config->nwft);
#endif
  writeoutputvar2(CFT_PET,cft_pet,1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_TRANSP,cft_transp,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_TRANSP_B,cft_transp_b,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_EVAP,cft_evap,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_EVAP_B,cft_evap_b,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_INTERC,cft_interc,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_INTERC_B,cft_interc_b,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_RETURN_FLOW_B,cft_return_flow_b,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputshortvar(CFT_IRRIG_EVENTS,cft_irrig_events,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_NIR,cft_nir,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_TEMP,cft_temp,ndate1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_PREC,cft_prec,1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_SRAD,cft_srad,ndate1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFTFRAC,cftfrac,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_AIRRIG,cft_airrig,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_FPAR,cft_fpar,ndate1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(LUC_IMAGE,cft_luc_image,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(CFT_ABOVEGBMC,cft_aboveground_biomass,carbon,2*(ncft+NGRASS));
  writeoutputvaritem(CFT_ABOVEGBMN,cft_aboveground_biomass,nitrogen,2*(ncft+NGRASS));
  /* ATTENTION! Due to allocation rules, this writes away next year's LAImax for trees and grasses */
  if(isopen(output,PFT_LAIMAX))
  {
    if(iswrite2(PFT_LAIMAX,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_LAIMAX].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
              foreachpft(pft,p,&stand->pftlist)
                grid[cell].output.pft_laimax[pft->par->id]+=pft->par->lai(pft);
          }
        }
      }
    }
    writeoutputvar2(PFT_LAIMAX,pft_laimax,1,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_NROOT))
  {
    if(iswrite2(PFT_NROOT,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_NROOT].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
            {
              foreachpft(pft,p,&stand->pftlist)
                switch(pft->par->type)
                {
                  case TREE:
                    tree = pft->data;
                    grid[cell].output.pft_root[pft->par->id].nitrogen+=tree->ind.root.nitrogen;
                    break;
                  case GRASS:
                    grass = pft->data;
                    grid[cell].output.pft_root[pft->par->id].nitrogen+=grass->ind.root.nitrogen;
                    break;
                }
            }
          }
        }
      }
    }
    writeoutputvaritem(PFT_NROOT,pft_root,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_CROOT))
  {
    if(iswrite2(PFT_CROOT,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_CROOT].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
            {
              switch(pft->par->type)
              {
                case TREE:
                  tree = pft->data;
                  grid[cell].output.pft_root[pft->par->id].carbon+=tree->ind.root.carbon;
                  break;
                case GRASS:
                  grass = pft->data;
                  grid[cell].output.pft_root[pft->par->id].carbon+=grass->ind.root.carbon;
                  break;
              }
            }
          }
        }
      }
    }
    writeoutputvaritem(PFT_CROOT,pft_root,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_CLEAF))
  {
    if(iswrite2(PFT_CLEAF,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_CLEAF].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
            {
              foreachpft(pft,p,&stand->pftlist)
                switch(pft->par->type)
                {
                  case TREE:
                    tree = pft->data;
                    grid[cell].output.pft_leaf[pft->par->id].carbon+=tree->ind.leaf.carbon;
                    break;
                  case GRASS:
                    grass = pft->data;
                    grid[cell].output.pft_leaf[pft->par->id].carbon+=grass->ind.leaf.carbon;
                    break;
                }
            }
          }
        }
      }
    }
    writeoutputvaritem(PFT_CLEAF,pft_leaf,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_NLEAF))
  {
    if(iswrite2(PFT_NLEAF,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_NLEAF].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
            {
              foreachpft(pft,p,&stand->pftlist)
                switch(pft->par->type)
                {
                  case TREE:
                    tree = pft->data;
                    grid[cell].output.pft_leaf[pft->par->id].nitrogen+=tree->ind.leaf.nitrogen;
                    break;
                  case GRASS:
                    grass = pft->data;
                    grid[cell].output.pft_leaf[pft->par->id].nitrogen+=grass->ind.leaf.nitrogen;
                    break;
                }
            }
          }
        }
      }
    }
    writeoutputvaritem(PFT_NLEAF,pft_leaf,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_CSAPW))
  {
    if(iswrite2(PFT_CSAPW,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_CSAPW].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            {
#if defined(IMAGE) || defined(INCLUDEWP)
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_sapw[(npft-config->nbiomass-config->nwft)+rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].carbon+=tree->ind.sapwood.carbon;
                  }
                }
                break;
#endif
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_sapw[(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].carbon+=tree->ind.sapwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_sapw[pft->par->id].carbon+=tree->ind.sapwood.carbon;
                  }
                }
                break;
            }
          }
        }
      }
    }
    writeoutputvaritem(PFT_CSAPW,pft_sapw,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_NSAPW))
  {
    if(iswrite2(PFT_NSAPW,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_NSAPW].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            {
#if defined(IMAGE) || defined(INCLUDEWP)
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_sapw[(npft-config->nbiomass-config->nwft)+rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].nitrogen+=tree->ind.sapwood.nitrogen;
                  }
                }
                break;
#endif
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_sapw[(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].nitrogen+=tree->ind.sapwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_sapw[pft->par->id].nitrogen+=tree->ind.sapwood.nitrogen;
                  }
                }
                break;
            }
          }
        }
      }
    }
    writeoutputvaritem(PFT_NSAPW,pft_sapw,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_CHAWO))
  {
    if(iswrite2(PFT_CHAWO,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_CHAWO].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            {
#if defined(IMAGE) || defined(INCLUDEWP)
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_hawo[(npft-config->nbiomass-config->nwft)+rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].carbon+=tree->ind.heartwood.carbon;
                  }
                }
                break;
#endif
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_hawo[(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].carbon+=tree->ind.heartwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_hawo[pft->par->id].carbon+=tree->ind.heartwood.carbon;
                  }
                }
                break;
            }
          }
        }
      }
    }
    writeoutputvaritem(PFT_CHAWO,pft_hawo,carbon,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
  if(isopen(output,PFT_NHAWO))
  {
    if(iswrite2(PFT_NHAWO,timestep,year,config) || (timestep==ANNUAL && config->outnames[PFT_NHAWO].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            switch(stand->type->landusetype)
            {
#if defined(IMAGE) || defined(INCLUDEWP)
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_hawo[(npft-config->nbiomass-config->nwft)+rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].nitrogen+=tree->ind.heartwood.nitrogen;
                  }
                }
                break;
#endif
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_hawo[(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)].nitrogen+=tree->ind.heartwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    grid[cell].output.pft_hawo[pft->par->id].nitrogen+=tree->ind.heartwood.nitrogen;
                  }
                }
                break;
            } /* of switch */
          }
        }
      }
    }
    writeoutputvaritem(PFT_NHAWO,pft_hawo,nitrogen,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  }
#ifdef DOUBLE_HARVEST
  writeoutputvaritem(PFT_HARVESTC2,pft_harvest2,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_HARVESTN2,pft_harvest2,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTC2,pft_harvest2,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTN2,pft_harvest2,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(GROWING_PERIOD2,growing_period2,1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_PET2,cft_pet2,1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_TRANSP2,cft_transp2,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_EVAP2,cft_evap2,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_INTERC2,cft_interc2,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_NIR2,cft_nir2,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_TEMP2,cft_temp2,ndate1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_PREC2,cft_prec2,1,2*(ncft+NGRASS));
  writeoutputvar2(CFT_SRAD2,cft_srad2,ndate1,2*(ncft+NGRASS));
  writeoutputvar2(CFTFRAC2,cftfrac2,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar2(CFT_AIRRIG2,cft_airrig2,1,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
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
       writedata(output,i,vec,year,date,ndata,config);
    }
    index++;
  }
  free(vec);
} /* of 'fwriteoutput' */
