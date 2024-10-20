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

static void flush_output(Outputfile *output,int index)
{
  switch(output->files[index].fmt)
  {
    case RAW: case CLM: case TXT:
      fflush(output->files[index].fp.file);
      break;
    case CDF:
      flush_netcdf(&output->files[index].fp.cdf);
      break;
  }
}

#define iswrite(output,index) (isopen(output,index) && iswrite2(index,timestep,year,config))

#define writeoutputvar(index,scale) if(iswrite(output,index))\
  {\
    count=0;\
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)\
        vec[count++]=(float)(grid[cell].output.data[config->outputmap[index]]*scale);\
    writedata(output,index,vec,year,date,ndata,config);\
    if(isroot(*config) && config->flush_output)\
      flush_output(output,index);\
  }

#define writeoutputarray(index,scale) if(iswrite(output,index))\
  {\
    outindex(output,index,year,date,config);\
    for(i=0;i<config->outputsize[index];i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          vec[count++]=(float)(grid[cell].output.data[config->outputmap[index]+i]*scale);\
      writepft(output,index,vec,year,date,ndata,i,config);\
    }\
    if(isroot(*config) && config->flush_output)\
      flush_output(output,index);\
  }

#define writeoutputshortvar(index) if(iswrite(output,index))\
  {\
    outindex(output,index,year,date,config);\
    svec=newvec(short,config->count);\
    check(svec);\
    for(i=0;i<config->outputsize[index];i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          svec[count++]=(short)(grid[cell].output.data[config->outputmap[index]+i]);\
      writeshortpft(output,index,svec,year,date,ndata,i,config);\
    }\
    if(isroot(*config) && config->flush_output)\
      flush_output(output,index);\
    free(svec);\
  }

static Bool iswrite2(int index,int timestep,int year,const Config *config)
{
 if(timestep==ANNUAL && config->outnames[index].timestep>0)
    return (year>=config->outputyear && (year-config->outputyear+1) % config->outnames[index].timestep==0);
  else
    return config->outnames[index].timestep==timestep;
} /* of 'iswrite2' */

static void outindex(Outputfile *output,int index,int year,int date,const Config *config)
{
  if(output->files[index].issocket)
    send_output_coupler(output->files[index].id,year,date,config);
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
        scale=1./NSECONDSDAY;
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
  scale=getscale(date,ndata,(config->outnames[index].timestep==ANNUAL) ? 1 : config->outnames[index].timestep,config->outnames[index].time);
  for(i=0;i<config->count;i++)
    data[i]=(float)(config->outnames[index].scale*scale*data[i]+config->outnames[index].offset);
#ifdef USE_MPI
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        mpi_write(output->files[index].fp.file,data,MPI_FLOAT,config->total,
                  output->counts,output->offsets,config->rank,config->comm);
        break;
      case TXT:
        mpi_write_txt(output->files[index].fp.file,data,MPI_FLOAT,config->total,
                  output->counts,output->offsets,config->rank,config->csv_delimit,config->comm);
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
  if(output->files[index].issocket)
  {
    send_output_coupler(index,year,date,config);
    mpi_write_socket(config->socket,data,MPI_FLOAT,config->total,
                     output->counts,output->offsets,config->rank,config->comm);
  }
#else
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(data,sizeof(float),config->count,output->files[index].fp.file)!=config->count)
          fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[index].fp.file,"%g%c",data[i],config->csv_delimit);
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
  if(output->files[index].issocket)
  {
    send_output_coupler(index,year,date,config);
    writefloat_socket(config->socket,data,config->count);
  }
#endif
} /* of 'writedata' */

static void writeshortdata(Outputfile *output,int index,short data[],int year,int date,int ndata,
                           const Config *config)
{
  int offset;
#ifdef USE_MPI
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        mpi_write(output->files[index].fp.file,data,MPI_SHORT,config->total,
                  output->counts,output->offsets,config->rank,config->comm);
        break;
      case TXT:
        mpi_write_txt(output->files[index].fp.file,data,MPI_SHORT,config->total,
                      output->counts,output->offsets,config->rank,config->csv_delimit,config->comm);
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
  if(output->files[index].issocket)
  {
    send_output_coupler(index,year,date,config);
    mpi_write_socket(config->socket,data,MPI_SHORT,config->total,
                     output->counts,output->offsets,config->rank,config->comm);
  }
#else
  int i;
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        fwrite(data,sizeof(short),config->count,output->files[index].fp.file);
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[index].fp.file,"%d%c",data[i],config->csv_delimit);
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
  if(output->files[index].issocket)
  {
    send_output_coupler(index,year,date,config);
    writeshort_socket(config->socket,data,config->count);
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
#endif
  scale=getscale(date,ndata,(config->outnames[index].timestep==ANNUAL) ? 1 : config->outnames[index].timestep,config->outnames[index].time);
  for(i=0;i<config->ngridcell;i++)
    data[i]=(float)(config->outnames[index].scale*scale*data[i]+config->outnames[index].offset);
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  getcounts(counts,offsets,config->nall,1,config->ntask);
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        mpi_write(output->files[index].fp.file,data,MPI_FLOAT,config->nall,counts,
                  offsets,config->rank,config->comm);
        break;
      case TXT:
        mpi_write_txt(output->files[index].fp.file,data,MPI_FLOAT,config->nall,counts,
                      offsets,config->rank,config->csv_delimit,config->comm);
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
  if(output->files[index].issocket)
  {
    send_output_coupler(index,year,date,config);
    mpi_write_socket(config->socket,data,MPI_FLOAT,config->nall,counts,
                     offsets,config->rank,config->comm);
  }
  free(counts);
  free(offsets);
#else
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        fwrite(data,sizeof(float),config->ngridcell,output->files[index].fp.file);
        break;
      case TXT:
        for(i=0;i<config->ngridcell-1;i++)
          fprintf(output->files[index].fp.file,"%g%c",data[i],config->csv_delimit);
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
  if(output->files[index].issocket)
  {
    send_output_coupler(index,year,date,config);
    writefloat_socket(config->socket,data,config->ngridcell);
  }
#endif
} /* of 'writealldata' */

static void writepft(Outputfile *output,int index,float *data,int year,
                     int date,int ndata,int layer,const Config *config)
{
  Real scale;
  int i,offset;
  scale=getscale(date,ndata,(config->outnames[index].timestep==ANNUAL) ? 1 : config->outnames[index].timestep,config->outnames[index].time);
  for(i=0;i<config->count;i++)
    data[i]=(float)(config->outnames[index].scale*scale*data[i]+config->outnames[index].offset);
#ifdef USE_MPI
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        mpi_write(output->files[index].fp.file,data,MPI_FLOAT,config->total,
                  output->counts,output->offsets,config->rank,config->comm);
        break;
      case TXT:
        mpi_write_txt(output->files[index].fp.file,data,MPI_FLOAT,config->total,
                      output->counts,output->offsets,config->rank,config->csv_delimit,config->comm);
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
  if(output->files[index].issocket)
  {
    mpi_write_socket(config->socket,data,MPI_FLOAT,config->total,
                     output->counts,output->offsets,config->rank,config->comm);
  }
#else
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(data,sizeof(float),config->count,output->files[index].fp.file)!=config->count)
          fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
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
  if(output->files[index].issocket)
    writefloat_socket(config->socket,data,config->count);
#endif
} /* of 'writepft' */

static void writeshortpft(Outputfile *output,int index,short *data,int year,
                          int date,int ndata,int layer,const Config *config)
{
  int i,offset;
  for(i=0;i<config->count;i++)
    data[i]=(short)(config->outnames[index].scale*data[i]+config->outnames[index].offset);
#ifdef USE_MPI
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        mpi_write(output->files[index].fp.file,data,MPI_SHORT,config->total,
                  output->counts,output->offsets,config->rank,config->comm);
        break;
      case TXT:
        mpi_write_txt(output->files[index].fp.file,data,MPI_SHORT,config->total,
                      output->counts,output->offsets,config->rank,config->csv_delimit,config->comm);
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
  if(output->files[index].issocket)
    mpi_write_socket(config->socket,data,MPI_SHORT,config->total,
                     output->counts,output->offsets,config->rank,config->comm);
#else
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(data,sizeof(short),config->count,output->files[index].fp.file)!=config->count)
          fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[index].fp.file,"%d%c",data[i],config->csv_delimit);
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
  if(output->files[index].issocket)
    writeshort_socket(config->socket,data,config->count);
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
  int i,count,s,p,cell,l,ndata,nirrig,nnat;
  Real ndate1,sumfrac;
  const Stand *stand;
  const Pft *pft;
  const Pfttree *tree;
  const Pftgrass *grass;
  const Irrigation *data;
  float *vec;
  short *svec;
  Real depth=0;

  nirrig=2*getnirrig(ncft,config);
  nnat=getnnat(npft,config);
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
    outindex(output,SEASONALITY,year,date,config);
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
  writeoutputvar(LAND_AREA,1);
  writeoutputarray(FPC,1);
  writeoutputarray(WPC,1);
  writeoutputvar(NPP,1);
  writeoutputvar(GPP,1);
  writeoutputvar(TWS,ndate1);
  writeoutputvar(DAYLENGTH,ndate1);
  writeoutputvar(TEMP,ndate1);
  writeoutputvar(SUN,ndate1);
  writeoutputvar(NPP_AGR,1);
  writeoutputvar(RH,1);
  writeoutputvar(RA,1);
  writeoutputvar(RH_AGR,1);
  writeoutputvar(EVAP,1);
  writeoutputvar(INTERC,1);
  writeoutputvar(TRANSP,1);
  writeoutputvar(PET,1);
  writeoutputvar(RUNOFF,1);
  writeoutputvar(LITFALLC,1);
  writeoutputvar(LITFALLN,1);
  writeoutputvar(FIREC,1);
  writeoutputvar(FIREN,1);
  writeoutputvar(FIREF,1);
  writeoutputvar(BNF_AGR,1);
  writeoutputvar(NFERT_AGR,1);
  writeoutputvar(NMANURE_AGR,1);
  writeoutputvar(NDEPO_AGR,1);
  writeoutputvar(NMINERALIZATION_AGR,1);
  writeoutputvar(NIMMOBILIZATION_AGR,1);
  writeoutputvar(NUPTAKE_AGR,1);
  writeoutputvar(NLEACHING_AGR,1);
  writeoutputvar(N2O_DENIT_AGR,1);
  writeoutputvar(N2O_NIT_AGR,1);
  writeoutputvar(NH3_AGR,1);
  writeoutputvar(N2_AGR,1);
  writeoutputvar(LITFALLN_AGR,1);
  writeoutputvar(HARVESTC_AGR,1);
  writeoutputvar(HARVESTN_AGR,1);
  writeoutputvar(SEEDN_AGR,1);
  writeoutputvar(DELTA_NORG_SOIL_AGR,1);
  writeoutputvar(DELTA_NMIN_SOIL_AGR,1);
  writeoutputvar(DELTA_NVEG_SOIL_AGR,1);
  writeoutputvar(CELLFRAC_AGR,ndate1);
  writeoutputvar(LITFALLC_WOOD,1);
  writeoutputvar(LITFALLN_WOOD,1);
  writeoutputvar(UPTAKEN_MGRASS,1);
  writeoutputvar(FECESN_MGRASS,1);
  writeoutputvar(URINEN_MGRASS,1);
  writeoutputvar(YIELDN_MGRASS,1);
  writeoutputvar(NH3_MGRASS,1);
  writeoutputvar(NO3_LEACHING_MGRASS,1);
  writeoutputvar(N2_MGRASS,1);
  writeoutputvar(N2O_NIT_MGRASS,1);
  writeoutputvar(N2O_DENIT_MGRASS,1);
  writeoutputvar(FLUX_ESTABN_MG,1);
  writeoutputvar(NAPPLIED_MG, 1);
  writeoutputvar(BNF_MG, 1);
  writeoutputvar(NDEPO_MG,1);
  writeoutputvar(UPTAKEC_MGRASS,1);
  writeoutputvar(FECESC_MGRASS,1);
  writeoutputvar(URINEC_MGRASS,1);
  writeoutputvar(YIELDC_MGRASS,1);
  writeoutputvar(RESPC_MGRASS,1);
  writeoutputvar(METHANEC_MGRASS,1);
  writeoutputvar(DELTAC_MGRASS,1);
  writeoutputvar(RA_MGRASS,1);
  writeoutputvar(RH_MGRASS,1);
  if(iswrite(output,DECAY_WOOD_AGR))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)
        vec[count++]=(float)(1-getoutput(&grid[cell].output,DECAY_WOOD_AGR,config));
    writedata(output,DECAY_WOOD_AGR,vec,year,date,0,config);
  }
  if(iswrite(output,DECAY_WOOD_NV))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)
        vec[count++]=(float)(1-getoutput(&grid[cell].output,DECAY_WOOD_NV,config));
    writedata(output,DECAY_WOOD_NV,vec,year,date,0,config);
  }
  if(iswrite(output,DECAY_LEAF_AGR))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)
        vec[count++]=(float)(1-getoutput(&grid[cell].output,DECAY_LEAF_AGR,config));
    writedata(output,DECAY_LEAF_AGR,vec,year,date,0,config);
  }
  if(iswrite(output,DECAY_LEAF_NV))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)
        vec[count++]=(float)(1-getoutput(&grid[cell].output,DECAY_LEAF_NV,config));
    writedata(output,DECAY_LEAF_NV,vec,year,date,0,config);
  }
  writeoutputvar(LITBURNC,1);
  writeoutputvar(LITBURNC_WOOD,1);
  writeoutputarray(RESPONSE_LAYER_AGR,1);
  writeoutputarray(RESPONSE_LAYER_NV,1);
  writeoutputarray(CSHIFT_FAST_NV,ndate1);
  writeoutputarray(CSHIFT_SLOW_NV,ndate1);
  if(isopen(output,VEGC))
  {
    if(iswrite2(VEGC,timestep,year,config) || (timestep==ANNUAL && config->outnames[VEGC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            /*if(stand->type->landusetype==NATURAL) */
            foreachpft(pft,p,&stand->pftlist)
              getoutput(&grid[cell].output,VEGC,config)+=vegc_sum(pft)*stand->frac;
          }
        }
    }
    writeoutputvar(VEGC,1);
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
            /*if(stand->type->landusetype==NATURAL) */
            foreachpft(pft,p,&stand->pftlist)
            {
              if(pft->par->cultivation_type==ANNUAL_CROP)
                getoutput(&grid[cell].output,VEGN,config)+=vegn_sum(pft)*stand->frac;
              else
                getoutput(&grid[cell].output,VEGN,config)+=(vegn_sum(pft)+pft->bm_inc.nitrogen)*stand->frac;
            }
          }
        }
    }
    writeoutputvar(VEGN,1);
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
              getoutput(&grid[cell].output,SOILC,config)+=stand->soil.litter.item[p].bg.carbon*stand->frac;
            forrootsoillayer(l)
              getoutput(&grid[cell].output,SOILC,config)+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
          }
        }
    }
    writeoutputvar(SOILC,1);
  }
  if(isopen(output,SOILC_1m))
  {
    if(iswrite2(SOILC_1m,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILC_1m].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            for(p=0;p<stand->soil.litter.n;p++)
              getoutput(&grid[cell].output,SOILC_1m,config)+=stand->soil.litter.item[p].bg.carbon*stand->frac;
            forrootsoillayer(l)
            {
              depth+=soildepth[l];
              if(depth>=100)
                break;
              else
               getoutput(&grid[cell].output,SOILC_1m,config)+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
            }
          }
        }
    }
    writeoutputvar(SOILC_1m,1);
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
              getoutput(&grid[cell].output,SOILN,config)+=stand->soil.litter.item[p].bg.nitrogen*stand->frac;
            forrootsoillayer(l)
              getoutput(&grid[cell].output,SOILN,config)+=(stand->soil.pool[l].slow.nitrogen+stand->soil.pool[l].fast.nitrogen)*stand->frac;
          }
        }
    }
    writeoutputvar(SOILN,1);
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
              getoutput(&grid[cell].output,SOILC_SLOW,config)+=stand->soil.pool[l].slow.carbon*stand->frac;
            getoutput(&grid[cell].output,SOILC_SLOW,config)+=stand->soil.YEDOMA*stand->frac;
          }
        }
    }
    writeoutputvar(SOILC_SLOW,1);
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
              getoutput(&grid[cell].output,SOILN_SLOW,config)+=stand->soil.pool[l].slow.nitrogen*stand->frac;
          }
        }
    }
    writeoutputvar(SOILN_SLOW,1);
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
            getoutput(&grid[cell].output,LITC,config)+=(litter_agtop_sum(&stand->soil.litter)+litter_agsub_sum(&stand->soil.litter))*stand->frac;
        }
    }
    writeoutputvar(LITC,1);
  }
  if(isopen(output,LITC_AG))
  {
    if(iswrite2(LITC_AG,timestep,year,config) || (timestep==ANNUAL && config->outnames[LITC_AG].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          /* if(stand->type->landusetype==NATURAL) */
            getoutput(&grid[cell].output,LITC_AG,config)+=litter_agtop_sum(&stand->soil.litter)*stand->frac;
        }
    }
    writeoutputvar(LITC_AG,1);
  }
  if(isopen(output,LITC_ALL))
  {
    if(iswrite2(LITC_ALL,timestep,year,config) || (timestep==ANNUAL && config->outnames[LITC_ALL].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          /* if(stand->type->landusetype==NATURAL) */
            getoutput(&grid[cell].output,LITC_ALL,config)+=littercarbon(&stand->soil.litter)*stand->frac;
        }
    }
    writeoutputvar(LITC_ALL,1);
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
            getoutput(&grid[cell].output,LITN,config)+=(litter_agtop_sum_n(&stand->soil.litter)+litter_agsub_sum_n(&stand->soil.litter))*stand->frac;
        }
    }
    writeoutputvar(LITN,1);
  }
/*
  if(isopen(output,SOILC_AGR))
  {
    if(iswrite2(SOILC_AGR,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILC_AGR].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == SETASIDE_RF || stand->type->landusetype == SETASIDE_IR ||
                 stand->type->landusetype == AGRICULTURE|| stand->type->landusetype == SETASIDE_WETLAND)
            {
              for (p = 0; p<stand->soil.litter.n; p++)
                grid[cell].output.soilc_agr+=stand->soil.litter.item[p].bg.carbon*stand->frac;
              forrootsoillayer(l)
                grid[cell].output.soilc_agr+=(stand->soil.pool[l].slow.carbon + stand->soil.pool[l].fast.carbon)*stand->frac;
            }
          }
        }
    }
    writeoutputvar(SOILC_AGR,soilc_agr);
  }
*/
/*
  if(isopen(output,LITC_AGR))
  {
    if(iswrite2(LITC_AGR,timestep,year,config) || (timestep==ANNUAL && config->outnames[LITC_AGR].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == SETASIDE_RF || stand->type->landusetype == SETASIDE_IR ||
                 stand->type->landusetype == AGRICULTURE || stand->type->landusetype == SETASIDE_WETLAND)
            {
              grid[cell].output.litc_agr +=(litter_ag_sum(&stand->soil.litter) + litter_agsub_sum(&stand->soil.litter))*stand->frac;            }
          }
        }
    }
    writeoutputvar(LITC_AGR,litc_agr);
  }
*/
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
              getoutput(&grid[cell].output,SOILNO3,config)+=stand->soil.NO3[l]*stand->frac;
            /*vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);*/
            }
          }
        }
    }
    writeoutputvar(SOILNO3,1);
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
              getoutput(&grid[cell].output,SOILNH4,config)+=stand->soil.NH4[l]*stand->frac;
            /*vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);*/
            }
          }
        }
    }
    writeoutputvar(SOILNH4,1);
  }
  if(isopen(output,MAXTHAW_DEPTH))
  {
    if(iswrite2(MAXTHAW_DEPTH,timestep,year,config) || (timestep==ANNUAL && config->outnames[MAXTHAW_DEPTH].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          foreachstand(stand,s,grid[cell].standlist)
            getoutput(&grid[cell].output,MAXTHAW_DEPTH,config)+=stand->soil.maxthaw_depth*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    }
    writeoutputvar(MAXTHAW_DEPTH,1);
  }
  writeoutputvar(FLUX_ESTABC,1);
  writeoutputvar(FLUX_ESTABN,1);
  writeoutputvar(HARVESTC,1);
  writeoutputvar(HARVESTN,1);
  writeoutputvar(RHARVEST_BURNTC,1);
  writeoutputvar(RHARVEST_BURNTN,1);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDC,1);
  writeoutputvar(RHARVEST_BURNT_IN_FIELDN,1);
  if(isopen(output,MG_VEGC))
  {
    if(iswrite2(MG_VEGC,timestep,year,config) || (timestep==ANNUAL && config->outnames[MG_VEGC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND &&  stand->type->landusetype!=KILL)
              foreachpft(pft,p,&stand->pftlist)
                getoutput(&grid[cell].output,MG_VEGC,config)+=(vegc_sum(pft)*stand->frac);
        }
    }
    writeoutputvar(MG_VEGC,1);
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
            if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND &&  stand->type->landusetype!=KILL)
            {
              for(p=0;p<stand->soil.litter.n;p++)
                getoutput(&grid[cell].output,MG_SOILC,config)+=stand->soil.litter.item[p].bg.carbon*stand->frac;
              forrootsoillayer(l)
                getoutput(&grid[cell].output,MG_SOILC,config)+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
            }
          }
        }
    }
    writeoutputvar(MG_SOILC,1);
  }
  if(isopen(output,MG_LITC))
  {
    if(iswrite2(MG_LITC,timestep,year,config) || (timestep==ANNUAL && config->outnames[MG_LITC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND &&  stand->type->landusetype!=KILL)
              getoutput(&grid[cell].output,MG_LITC,config)+=(litter_agtop_sum(&stand->soil.litter)+litter_agsub_sum(&stand->soil.litter))*stand->frac;
        }
    }
    writeoutputvar(MG_LITC,1);
  }
  if(isopen(output,SOILC_AGR))
  {
    if(iswrite2(SOILC_AGR,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILC_AGR].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(isagriculture(stand->type->landusetype))
            {
              for(p=0;p<stand->soil.litter.n;p++)
                getoutput(&grid[cell].output,SOILC_AGR,config)+=stand->soil.litter.item[p].bg.carbon*stand->frac;
              forrootsoillayer(l)
                getoutput(&grid[cell].output,SOILC_AGR,config)+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
            }
          }
        }
    }
    writeoutputvar(SOILC_AGR,1);
  }
  if(isopen(output,LITC_AGR))
  {
    if(iswrite2(LITC_AGR,timestep,year,config) || (timestep==ANNUAL && config->outnames[LITC_AGR].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            if(isagriculture(stand->type->landusetype))
              getoutput(&grid[cell].output,LITC_AGR,config)+=(litter_agtop_sum(&stand->soil.litter)+litter_agsub_sum(&stand->soil.litter))*stand->frac;
        }
    }
    writeoutputvar(LITC_AGR,1);
  }
  writeoutputvar(INPUT_LAKE,1e-9);
  if(iswrite(output,ADISCHARGE))
  {
    for(cell=0;cell<config->ngridcell;cell++)
      vec[cell]=(float)(getoutput(&grid[cell].output,ADISCHARGE,config)*1e-9);
    writealldata(output,ADISCHARGE,vec,year,date,ndata,config);
  }
  writeoutputvar(DEFOREST_EMIS,1);
  writeoutputvar(DEFOREST_EMIS_N,1);
  writeoutputvar(TRAD_BIOFUEL,1);
  writeoutputvar(FBURN,1);
  writeoutputvar(FTIMBER,1);
  writeoutputvar(TIMBER_HARVESTC,1);
  writeoutputvar(TIMBER_HARVESTN,1);
  writeoutputvar(PRODUCT_POOL_FAST,1);
  writeoutputvar(PRODUCT_POOL_SLOW,1);
  writeoutputvar(PRODUCT_POOL_FAST_N,1);
  writeoutputvar(PRODUCT_POOL_SLOW_N,1);
  writeoutputvar(PROD_TURNOVER,1);
  writeoutputvar(PROD_TURNOVER_N,1);
  if(iswrite(output,AFRAC_WD_UNSUST))
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
         vec[count++]=(float)(grid[cell].balance.awd_unsustainable/((grid[cell].balance.airrig+
            grid[cell].balance.aconv_loss_evap + grid[cell].balance.aconv_loss_drain)*grid[cell].coord.area));
    writedata(output,AFRAC_WD_UNSUST,vec,year,date,ndata,config);
  }
  writeoutputvar(WATERUSE_HIL,1);
  writeoutputvar(WATERUSECONS,1);
  writeoutputvar(WATERUSEDEM,1);
  if(isopen(output,AGB))
  {
    if(iswrite2(AGB,timestep,year,config) || (timestep==ANNUAL && config->outnames[AGB].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          foreachstand(stand,s,grid[cell].standlist)
            foreachpft(pft,p,&stand->pftlist)
              getoutput(&grid[cell].output,AGB,config)+=agb(pft)*stand->frac;
        }
    }
    writeoutputvar(AGB,1);
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
                getoutput(&grid[cell].output,AGB_TREE,config)+=agb(pft)*stand->frac;
        }
    }
    writeoutputvar(AGB_TREE,1);
  }
  writeoutputvar(NEGC_FLUXES,1);
  writeoutputvar(NEGN_FLUXES,1);
  writeoutputvar(MEANVEGCMANGRASS,ndate1);
  writeoutputvar(VEGC_AVG,ndate1);
  writeoutputvar(RH_LITTER,1);
  writeoutputvar(DISCHARGE,1e-9);     /* daily mean discharge per month in 1.000.000 m3 per cell */
  writeoutputvar(WATERAMOUNT,1e-9); /* mean wateramount per month in 1.000.000 m3 per cell */
  writeoutputvar(TRANSP_B,1);
  writeoutputvar(EVAP_B,1);
  writeoutputvar(INTERC_B,1);
  writeoutputarray(WHC_NAT,ndate1);
  writeoutputarray(WHC_GRASS,ndate1);
  writeoutputarray(KS_AGR,ndate1);
  writeoutputarray(KS_NAT,ndate1);
  writeoutputarray(KS_GRASS,ndate1);
  writeoutputarray(WHC_AGR,ndate1);
  writeoutputarray(SWC,ndate1);
  writeoutputvar(SWC1,ndate1);
  writeoutputvar(SWC2,ndate1);
  writeoutputvar(SWC3,ndate1);
  writeoutputvar(SWC4,ndate1);
  writeoutputvar(SWC5,ndate1);
  writeoutputvar(ROOTMOIST,ndate1);
  writeoutputvar(IRRIG,1);
  writeoutputvar(RETURN_FLOW_B,1);
  writeoutputvar(EVAP_LAKE,1);
  writeoutputvar(EVAP_RES,1);
  writeoutputvar(PREC_RES,1);
  writeoutputvar(NFIRE,1);
  writeoutputvar(FIREDI,ndate1);
  writeoutputvar(FIREEMISSION_CO2,1);
  writeoutputvar(FIREEMISSION_CO,1);
  writeoutputvar(FIREEMISSION_CH4,1);
  writeoutputvar(FIREEMISSION_VOC,1);
  writeoutputvar(FIREEMISSION_TPM,1);
  writeoutputvar(FIREEMISSION_NOX,1);
  writeoutputvar(BURNTAREA,1);
  writeoutputvar(TEMP_IMAGE,ndate1);
  writeoutputvar(PREC_IMAGE,1);
  writeoutputvar(SUN_IMAGE,ndate1);
  writeoutputvar(WET_IMAGE,1);
  writeoutputarray(PERC,1);
  writeoutputarray(SOILTEMP,ndate1);
  writeoutputvar(SOILTEMP1,ndate1);
  writeoutputvar(SOILTEMP2,ndate1);
  writeoutputvar(SOILTEMP3,ndate1);
  writeoutputvar(SOILTEMP4,ndate1);
  writeoutputvar(SOILTEMP5,ndate1);
  writeoutputvar(SOILTEMP6,ndate1);
  writeoutputvar(NUPTAKE,1);
  writeoutputvar(LEACHING,1);
  writeoutputvar(N2O_DENIT,1);
  writeoutputvar(N2O_NIT,1);
  writeoutputvar(N2_EMIS,1);
  writeoutputvar(BNF,1);
  writeoutputvar(N_MINERALIZATION,1);
  writeoutputvar(N_VOLATILIZATION,1);
  writeoutputvar(N_IMMO,1);
  writeoutputvar(RES_STORAGE,ndate1);
  writeoutputvar(GW_OUTFLUX,1);
  writeoutputvar(GW_STORAGE,ndate1);
  writeoutputvar(CH4_EMISSIONS,1);
  writeoutputvar(CH4_EMISSIONS_WET,1);
  writeoutputvar(CH4_OXIDATION,1);
  writeoutputvar(CH4_SETASIDE,1);
  writeoutputvar(CH4_RICE_EM,1);
  writeoutputvar(CH4_EBULLITION,1);
  writeoutputvar(CH4_SINK,1);
  writeoutputvar(CH4_PLANT_GAS,1);
  writeoutputvar(WTAB,ndate1);
  writeoutputvar(MT_WATER,1);
  writeoutputvar(MWATER,ndate1);
  writeoutputvar(WETFRAC,1);
  writeoutputvar(MEANSOILO2,ndate1);
  writeoutputvar(MEANSOILCH4,ndate1);
  writeoutputvar(RES_DEMAND,1);
  writeoutputvar(TARGET_RELEASE,1);
  writeoutputvar(RES_CAP,1);
  writeoutputvar(WD_UNSUST,1);
  writeoutputvar(UNMET_DEMAND,1);
  writeoutputvar(WD_LOCAL,1);
  writeoutputvar(WD_NEIGHB,1);
  writeoutputvar(WD_RES,1);
  writeoutputvar(CONV_LOSS_EVAP,1);
  writeoutputvar(CONV_LOSS_DRAIN,1);
  writeoutputvar(STOR_RETURN,1);
  writeoutputvar(PREC,1);
  writeoutputvar(RAIN,1);
  writeoutputvar(SNOWF,1);
  writeoutputvar(MELT,1);
  writeoutputvar(SWE,ndate1);
  writeoutputvar(LITTERTEMP,ndate1);
  writeoutputvar(SNOWRUNOFF,1);
  writeoutputvar(RUNOFF_SURF,1);
  writeoutputvar(RUNOFF_LAT,1);
  writeoutputvar(SEEPAGE,1);
  writeoutputvar(FAPAR,ndate1);
  writeoutputvar(ALBEDO,ndate1);
  writeoutputvar(PHEN_TMIN,ndate1);
  writeoutputvar(PHEN_TMAX,ndate1);
  writeoutputvar(PHEN_LIGHT,ndate1);
  writeoutputvar(PHEN_WATER,ndate1);
  writeoutputvar(WSCAL,ndate1);
  writeoutputvar(GCONS_RF,1);
  writeoutputvar(GCONS_IRR,1);
  writeoutputvar(BCONS_IRR,1);
  writeoutputvar(IRRIG_RW,1);
  writeoutputvar(LAKEVOL,ndate1);
  writeoutputvar(RIVERVOL,ndate1);
  writeoutputarray(SWC_VOL,ndate1);
  writeoutputvar(IRRIG_STOR,ndate1);
  writeoutputvar(LAKETEMP,ndate1);
  writeoutputshortvar(SDATE);
  writeoutputshortvar(HDATE);
  if(iswrite(output,CFT_SWC))
  {
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(!grid[cell].skip)
      {
        for(i=0;i<ncft*2;i++)
          if(getoutputindex(&grid[cell].output,NDAY_MONTH,i,config)>0)
            getoutputindex(&grid[cell].output,CFT_SWC,i,config)/=getoutputindex(&grid[cell].output,NDAY_MONTH,i,config);
          else
            getoutputindex(&grid[cell].output,CFT_SWC,i,config)=0;
      }
    }
    writeoutputarray(CFT_SWC,1);
  }
  writeoutputarray(PFT_NPP,1);
  writeoutputarray(PFT_NUPTAKE,1);
  writeoutputarray(PFT_BNF,1);
  writeoutputarray(PFT_NDEMAND,1);
  writeoutputarray(HUSUM,1);
  writeoutputarray(CFT_RUNOFF,1);
  writeoutputarray(CFT_N2O_DENIT,1);
  writeoutputarray(CFT_N2O_NIT,1);
  writeoutputarray(CFT_N2_EMIS,1);
  writeoutputarray(CFT_LEACHING,1);
  writeoutputarray(CFT_C_EMIS,1);

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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
              foreachpft(pft,p,&stand->pftlist)
                getoutputindex(&grid[cell].output,PFT_NLIMIT,pft->par->id,config)+=pft->nlimit*ndate1;
          }
        }
      }
    }
    writeoutputarray(PFT_NLIMIT,1)
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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
              foreachpft(pft,p,&stand->pftlist)
                getoutputindex(&grid[cell].output,PFT_VEGC,pft->par->id,config)+=vegc_sum(pft);
          }
        }
      }
    }
    writeoutputarray(PFT_VEGC,1);
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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
              foreachpft(pft,p,&stand->pftlist)
                getoutputindex(&grid[cell].output,PFT_VEGN,pft->par->id,config)+=vegn_sum(pft)+pft->bm_inc.nitrogen;
          }
        }
      }
    }
    writeoutputarray(PFT_VEGN,1);
  }
  if(iswrite(output,PFT_GCGP))
  {
    outindex(output,PFT_GCGP,year,date,config);
    for(i=0;i<nnat+nirrig;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          if(getoutputindex(&grid[cell].output,PFT_GCGP_COUNT,i,config) > 0)
            getoutputindex(&grid[cell].output,PFT_GCGP,i,config)/=getoutputindex(&grid[cell].output,PFT_GCGP_COUNT,i,config);
          else
            getoutputindex(&grid[cell].output,PFT_GCGP,i,config)=-9;
          vec[count++]=(float)getoutputindex(&grid[cell].output,PFT_GCGP,i,config);
        }
      writepft(output,PFT_GCGP,vec,year,date,ndata,i,config);
    }
  }
  writeoutputarray(PFT_HARVESTC,1);
  writeoutputarray(PFT_HARVESTN,1);
  writeoutputarray(PFT_RHARVESTC,1);
  writeoutputarray(PFT_RHARVESTN,1);
  writeoutputarray(CFT_CONSUMP_WATER_G,1);
  writeoutputarray(CFT_CONSUMP_WATER_B,1);
  writeoutputarray(GROWING_PERIOD,1);
  writeoutputarray(PFT_MORT,1);
  writeoutputarray(FPC_BFT,1);
  writeoutputarray(NV_LAI,ndate1);
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
              getoutputindex(&grid[cell].output,SOILC_LAYER,0,config)+=stand->soil.litter.item[p].bg.carbon*stand->frac;
            forrootsoillayer(i)
              getoutputindex(&grid[cell].output,SOILC_LAYER,i,config)+=(stand->soil.pool[i].slow.carbon+stand->soil.pool[i].fast.carbon)*stand->frac;
          }
        }
      }
    }
    writeoutputarray(SOILC_LAYER,1);
  }
  if(isopen(output,SOILC_AGR_LAYER))
  {
    if(iswrite2(SOILC_AGR_LAYER,timestep,year,config) || (timestep==ANNUAL && config->outnames[SOILC_AGR_LAYER].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          sumfrac=0;
          foreachstand(stand,s,grid[cell].standlist)
            if(isagriculture(stand->type->landusetype))
              sumfrac+=stand->frac;

          foreachstand(stand,s,grid[cell].standlist)
          {
            if(isagriculture(stand->type->landusetype))
            {
              for(p=0;p<stand->soil.litter.n;p++)
                getoutputindex(&grid[cell].output,SOILC_AGR_LAYER,0,config)+=stand->soil.litter.item[p].bg.carbon*stand->frac/sumfrac;
              forrootsoillayer(i)
                getoutputindex(&grid[cell].output,SOILC_AGR_LAYER,i,config)+=(stand->soil.pool[i].slow.carbon+stand->soil.pool[i].fast.carbon)*stand->frac/sumfrac;
            }
          }
        }
      }
    }
    writeoutputarray(SOILC_AGR_LAYER,1);
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
              getoutputindex(&grid[cell].output,SOILN_LAYER,0,config)+=stand->soil.litter.item[p].bg.nitrogen*stand->frac;
            forrootsoillayer(i)
              getoutputindex(&grid[cell].output,SOILN_LAYER,i,config)+=(stand->soil.pool[i].slow.nitrogen+stand->soil.pool[i].fast.nitrogen)*stand->frac;
          }
        }
      }
    }
    writeoutputarray(SOILN_LAYER,1);
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
              getoutputindex(&grid[cell].output,SOILNO3_LAYER,i,config)+=stand->soil.NO3[i]*stand->frac;
          }
        }
      }
    }
    writeoutputarray(SOILNO3_LAYER,1);
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
              getoutputindex(&grid[cell].output,SOILNH4_LAYER,i,config)+=stand->soil.NH4[i]*stand->frac;
          }
        }
      }
    }
    writeoutputarray(SOILNH4_LAYER,1);
  }
  if(config->nwft)
    writeoutputarray(WFT_VEGC,1);
  writeoutputarray(CFT_PET,1);
  writeoutputarray(CFT_TRANSP,1);
  writeoutputarray(CFT_TRANSP_B,1);
  writeoutputarray(CFT_EVAP,1);
  writeoutputarray(CFT_EVAP_B,1);
  writeoutputarray(CFT_INTERC,1);
  writeoutputarray(CFT_INTERC_B,1);
  writeoutputarray(CFT_RETURN_FLOW_B,1);
  writeoutputshortvar(CFT_IRRIG_EVENTS);
  writeoutputarray(CFT_NIR,1);
  writeoutputarray(CFT_TEMP,ndate1);
  writeoutputarray(CFT_PREC,1);
  writeoutputarray(CFT_SRAD,ndate1);
  writeoutputarray(CFT_CONV_LOSS_EVAP,1);
  writeoutputarray(CFT_CONV_LOSS_DRAIN,1);
  writeoutputarray(CFTFRAC,1);
  writeoutputarray(CFT_NHARVEST,1);
  writeoutputarray(CFT_AIRRIG,1);
  writeoutputarray(CFT_FPAR,ndate1);
  writeoutputarray(LUC_IMAGE,1);
  writeoutputarray(CFT_NFERT,1);
  writeoutputarray(CFT_ABOVEGBMC,1);
  writeoutputarray(CFT_ABOVEGBMN,1);
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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
              foreachpft(pft,p,&stand->pftlist)
                getoutputindex(&grid[cell].output,PFT_LAIMAX,pft->par->id,config)+=pft->par->lai(pft);
          }
        }
      }
    }
    writeoutputarray(PFT_LAIMAX,1);
  }
  writeoutputarray(PFT_LAI,ndate1); /* FS 2022-12-06: added since this output was never written out */
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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
            {
              foreachpft(pft,p,&stand->pftlist)
                switch(pft->par->type)
                {
                  case TREE:
                    tree = pft->data;
                    getoutputindex(&grid[cell].output,PFT_NROOT,pft->par->id,config)+=tree->ind.root.nitrogen;
                    break;
                  case GRASS:
                    grass = pft->data;
                    getoutputindex(&grid[cell].output,PFT_NROOT,pft->par->id,config)+=grass->ind.root.nitrogen;
                    break;
                }
            }
          }
        }
      }
    }
    writeoutputarray(PFT_NROOT,1);
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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
            {
              foreachpft(pft,p,&stand->pftlist)
                switch(pft->par->type)
                {
                  case TREE:
                    tree = pft->data;
                    getoutputindex(&grid[cell].output,PFT_CROOT,pft->par->id,config)+=tree->ind.root.carbon;
                    break;
                  case GRASS:
                    grass = pft->data;
                    getoutputindex(&grid[cell].output,PFT_CROOT,pft->par->id,config)+=grass->ind.root.carbon;
                    break;
                }
            }
          }
        }
      }
    }
    writeoutputarray(PFT_CROOT,1);
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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
            {
              foreachpft(pft,p,&stand->pftlist)
                switch(pft->par->type)
                {
                  case TREE:
                    tree = pft->data;
                    getoutputindex(&grid[cell].output,PFT_CLEAF,pft->par->id,config)+=tree->ind.leaf.carbon;
                    break;
                  case GRASS:
                    grass = pft->data;
                    getoutputindex(&grid[cell].output,PFT_CLEAF,pft->par->id,config)+=grass->ind.leaf.carbon;
                    break;
                }
            }
          }
        }
      }
    }
    writeoutputarray(PFT_CLEAF,1);
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
            if(stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND)
            {
              foreachpft(pft,p,&stand->pftlist)
                switch(pft->par->type)
                {
                  case TREE:
                    tree = pft->data;
                    getoutputindex(&grid[cell].output,PFT_NLEAF,pft->par->id,config)+=tree->ind.leaf.nitrogen;
                    break;
                  case GRASS:
                    grass = pft->data;
                    getoutputindex(&grid[cell].output,PFT_NLEAF,pft->par->id,config)+=grass->ind.leaf.nitrogen;
                    break;
                }
            }
          }
        }
      }
    }
    writeoutputarray(PFT_NLEAF,1);
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
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CSAPW,nnat+rwp(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.sapwood.carbon;
                  }
                }
                break;
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CSAPW,nnat+rbtree(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.sapwood.carbon;
                  }
                }
                break;
              case AGRICULTURE_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CSAPW,nnat+agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.sapwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CSAPW,pft->par->id,config)+=tree->ind.sapwood.carbon;
                  }
                }
                break;
              default:
                /* do nothing */
                break;
            }
          }
        }
      }
    }
    writeoutputarray(PFT_CSAPW,1);
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
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NSAPW,nnat+rwp(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.sapwood.nitrogen;
                  }
                }
                break;
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NSAPW,nnat+rbtree(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.sapwood.nitrogen;
                  }
                }
                break;
              case AGRICULTURE_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NSAPW,nnat+agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.sapwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NSAPW,pft->par->id,config)+=tree->ind.sapwood.nitrogen;
                  }
                }
                break;
              default:
                /* do nothing */
                break;
            }
          }
        }
      }
    }
    writeoutputarray(PFT_NSAPW,1);
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
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CHAWO,nnat+rwp(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.heartwood.carbon;
                  }
                }
                break;
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CHAWO,nnat+rbtree(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.heartwood.carbon;
                  }
                }
                break;
              case AGRICULTURE_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CHAWO,nnat+agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.heartwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_CHAWO,pft->par->id,config)+=tree->ind.heartwood.carbon;
                  }
                }
                break;
              default:
                /* do nothing */
                break;
            }
          }
        }
      }
    }
    writeoutputarray(PFT_CHAWO,1);
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
              case WOODPLANTATION:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NHAWO,nnat+rwp(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.heartwood.nitrogen;
                  }
                }
                break;
              case BIOMASS_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NHAWO,nnat+rbtree(ncft)+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.heartwood.nitrogen;
                  }
                }
                break;
              case AGRICULTURE_TREE:
                data=stand->data;
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NHAWO,nnat+agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+data->irrigation*getnirrig(ncft,config),config)+=tree->ind.heartwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(pft->par->type==TREE)
                  {
                    tree=pft->data;
                    getoutputindex(&grid[cell].output,PFT_NHAWO,pft->par->id,config)+=tree->ind.heartwood.nitrogen;
                  }
                }
                break;
              default:
                /* do nothing */
                break;
            } /* of switch */
          }
        }
      }
    }
    writeoutputarray(PFT_NHAWO,1);
  }
  if(isopen(output,MGRASS_SOILC))
  {
    if(iswrite2(MGRASS_SOILC,timestep,year,config) || (timestep==ANNUAL && config->outnames[MGRASS_SOILC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          sumfrac=0;
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype==GRASSLAND)
              sumfrac+=stand->frac;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype==GRASSLAND)
            {
              for(p=0;p<stand->soil.litter.n;p++)
                getoutput(&grid[cell].output,MGRASS_SOILC,config)+=stand->soil.litter.item[p].bg.carbon*stand->frac/sumfrac;
              forrootsoillayer(l)
                getoutput(&grid[cell].output,MGRASS_SOILC,config)+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac/sumfrac;
            }
          }
        }
    }
    writeoutputvar(MGRASS_SOILC,1);
  }
  if(isopen(output,MGRASS_SOILN))
  {
    if(iswrite2(MGRASS_SOILN,timestep,year,config) || (timestep==ANNUAL && config->outnames[MGRASS_SOILN].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          sumfrac=0;
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype==GRASSLAND)
              sumfrac+=stand->frac;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype==GRASSLAND)
            {
              for(p=0;p<stand->soil.litter.n;p++)
                getoutput(&grid[cell].output,MGRASS_SOILN,config)+=stand->soil.litter.item[p].bg.nitrogen*stand->frac/sumfrac;
              forrootsoillayer(l)
                getoutput(&grid[cell].output,MGRASS_SOILN,config)+=(stand->soil.pool[l].slow.nitrogen+stand->soil.pool[l].fast.nitrogen)*stand->frac/sumfrac;
            }
          }
        }
    }
    writeoutputvar(MGRASS_SOILN,1);
  }
  if(isopen(output,MGRASS_LITC))
  {
    if(iswrite2(MGRASS_LITC,timestep,year,config) || (timestep==ANNUAL && config->outnames[MGRASS_LITC].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          sumfrac=0;
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype==GRASSLAND)
              sumfrac+=stand->frac;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype==GRASSLAND)
              getoutput(&grid[cell].output,MGRASS_LITC,config)+=(litter_agtop_sum(&stand->soil.litter)+litter_agsub_sum(&stand->soil.litter))*stand->frac/sumfrac;
          }
        }
    }
    writeoutputvar(MGRASS_LITC,1);
  }
  if(isopen(output,MGRASS_LITN))
  {
    if(iswrite2(MGRASS_LITN,timestep,year,config) || (timestep==ANNUAL && config->outnames[MGRASS_LITN].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          sumfrac=0;
          foreachstand(stand,s,grid[cell].standlist)
            if(stand->type->landusetype==GRASSLAND)
              sumfrac+=stand->frac;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype==GRASSLAND)
              getoutput(&grid[cell].output,MGRASS_LITN,config)+=(litter_agtop_sum_n(&stand->soil.litter)+litter_agsub_sum_n(&stand->soil.litter))*stand->frac/sumfrac;
          }
        }
    }
    writeoutputvar(MGRASS_LITN,1);
  }
  if(isopen(output,ESTAB_STORAGE_C))
  {
    if(iswrite2(ESTAB_STORAGE_C,timestep,year,config) || (timestep==ANNUAL && config->outnames[ESTAB_STORAGE_C].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          getoutput(&grid[cell].output,ESTAB_STORAGE_C,config)+=grid[cell].balance.estab_storage_tree[0].carbon +
          grid[cell].balance.estab_storage_tree[1].carbon +
          grid[cell].balance.estab_storage_grass[0].carbon +
          grid[cell].balance.estab_storage_grass[1].carbon;
        }
    }
    writeoutputvar(ESTAB_STORAGE_C,1);
  }
  if(isopen(output,ESTAB_STORAGE_N))
  {
    if(iswrite2(ESTAB_STORAGE_N,timestep,year,config) || (timestep==ANNUAL && config->outnames[ESTAB_STORAGE_N].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          getoutput(&grid[cell].output,ESTAB_STORAGE_N,config)+=grid[cell].balance.estab_storage_tree[0].nitrogen +
          grid[cell].balance.estab_storage_tree[1].nitrogen +
          grid[cell].balance.estab_storage_grass[0].nitrogen +
          grid[cell].balance.estab_storage_grass[1].nitrogen;
        }
    }
    writeoutputvar(ESTAB_STORAGE_N,1);
  }
  if(isopen(output,NBP))
  {
    if(iswrite2(NBP,timestep,year,config) || (timestep==ANNUAL && config->outnames[NBP].timestep>0))
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip && grid[cell].lakefrac+grid[cell].ml.reservoirfrac<1)
        {
          if(config->natNBP_only)
          {
            getoutput(&grid[cell].output,NBP,config)+=grid[cell].balance.nat_fluxes;
          }
          else
            getoutput(&grid[cell].output,NBP,config)+=(grid[cell].balance.anpp-grid[cell].balance.arh-grid[cell].balance.fire.carbon+
                    grid[cell].balance.flux_estab.carbon-grid[cell].balance.flux_harvest.carbon-grid[cell].balance.biomass_yield.carbon-
                    grid[cell].balance.neg_fluxes.carbon+grid[cell].balance.influx.carbon-grid[cell].balance.deforest_emissions.carbon-
                    grid[cell].balance.prod_turnover.fast.carbon-grid[cell].balance.prod_turnover.slow.carbon-grid[cell].balance.trad_biofuel.carbon);
       }
    }
    writeoutputvar(NBP,1);
  }
  writeoutputvar(RD,1);
  writeoutputarray(PFT_WATER_DEMAND,1);
  writeoutputarray(PFT_WATER_SUPPLY,1);
  writeoutputvar(NDEPOS,1);

  if(config->separate_harvests)
  {
    writeoutputarray(PFT_HARVESTC2,1);
    writeoutputarray(PFT_HARVESTN2,1);
    writeoutputarray(PFT_RHARVESTC2,1);
    writeoutputarray(PFT_RHARVESTN2,1);
    writeoutputarray(GROWING_PERIOD2,1);
    writeoutputarray(CFT_PET2,1);
    writeoutputarray(CFT_TRANSP2,1);
    writeoutputarray(CFT_EVAP2,1);
    writeoutputarray(CFT_INTERC2,1);
    writeoutputarray(CFT_NIR2,1);
    writeoutputarray(CFT_TEMP2,ndate1);
    writeoutputarray(CFT_PREC2,1);
    writeoutputarray(CFT_SRAD2,ndate1);
    writeoutputarray(CFTFRAC2,1)
    writeoutputarray(CFT_AIRRIG2,1)
    writeoutputshortvar(SDATE2);
    writeoutputshortvar(HDATE2);
    writeoutputshortvar(SYEAR);
    writeoutputshortvar(SYEAR2);
    writeoutputarray(HUSUM2,1);
    writeoutputarray(CFT_ABOVEGBMC2,1);
    writeoutputarray(CFT_ABOVEGBMN2,1);
    writeoutputarray(CFT_RUNOFF2,1);
    writeoutputarray(CFT_N2O_DENIT2,1);
    writeoutputarray(CFT_N2O_NIT2,1);
    writeoutputarray(CFT_N2_EMIS2,1);
    writeoutputarray(CFT_LEACHING2,1);
    writeoutputarray(CFT_C_EMIS2,1);
    writeoutputarray(CFT_NFERT2,1);
    writeoutputarray(PFT_NUPTAKE2,1);
  }
  free(vec);
} /* of 'fwriteoutput' */
