/**************************************************************************************/
/**                                                                                \n**/
/**    f  w  r  i  t  e  o  u  t  p  u  t  _  p  f  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes out PFT specific output data                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static void outindex(Outputfile *output,int index,int rank)
{
  if(output->method==LPJ_SOCKET && rank==0)
    writeint_socket(output->socket,&index,1);
} /* of 'outindex' */

static void writepft(Outputfile *output,int n,float *data,int size,int year,
                     int index,
                     const Config *config)
{
  int i;
#ifdef USE_MPI
  MPI_Status status;
#endif
  for(i=0;i<config->count;i++)
    data[i]*=config->outnames[n].scale;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[n].fp.mpi_file,
                        ((year-config->firstyear)*size+index)*config->total+config->offset,
                        data,config->count,MPI_FLOAT,&status);
      break;
    case LPJ_GATHER:
      switch(output->files[n].fmt)
      {
        case RAW: case CLM:
          mpi_write(output->files[n].fp.file,data,MPI_FLOAT,config->total,
                    output->counts,output->offsets,config->rank,config->comm);
          break;
        case TXT:
          mpi_write_txt(output->files[n].fp.file,data,MPI_FLOAT,config->total,
                        output->counts,output->offsets,config->rank,config->comm);
          break;
        case CDF:
          mpi_write_pft_netcdf(&output->files[n].fp.cdf,data,MPI_FLOAT,config->total,
                               output->files[n].oneyear ? NO_TIME : year-config->firstyear,
                               index,
                               output->counts,output->offsets,config->rank,config->comm);
          break;
      }
      break;
    case LPJ_SOCKET:
      mpi_write_socket(output->socket,data,MPI_FLOAT,config->total,
                       output->counts,output->offsets,config->rank,config->comm);
      break;
  } /* of switch */
#else
  if(output->method==LPJ_FILES)
    switch(output->files[n].fmt)
    {
      case RAW: case CLM:
        if(fwrite(data,sizeof(float),config->count,output->files[n].fp.file)!=config->count)
          fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[n].fp.file,"%g ",data[i]);
        fprintf(output->files[n].fp.file,"%g\n",data[config->count-1]);
        break;
      case CDF:
        write_pft_float_netcdf(&output->files[n].fp.cdf,data,
                               output->files[n].oneyear ? NO_TIME : year-config->firstyear,
                               index,config->count);
        break;
    }
  else
    writefloat_socket(output->socket,data,config->count);
#endif
} /* of *writepft' */

static void writeshortpft(Outputfile *output,int n,short *data,int size,
                          int year,
                          int index,
                          const Config *config)
{
#ifdef USE_MPI
  MPI_Status status;
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[n].fp.mpi_file,
                        ((year-config->firstyear)*size+index)*config->total+config->offset,
                        data,config->count,MPI_SHORT,&status);
      break;
    case LPJ_GATHER:
      switch(output->files[n].fmt)
      {
        case RAW: case CLM:
          mpi_write(output->files[n].fp.file,data,MPI_SHORT,config->total,
                    output->counts,output->offsets,config->rank,config->comm);
          break;
        case TXT:
          mpi_write_txt(output->files[n].fp.file,data,MPI_SHORT,config->total,
                        output->counts,output->offsets,config->rank,config->comm);
          break;
        case CDF:
          mpi_write_pft_netcdf(&output->files[n].fp.cdf,data,MPI_SHORT,config->total,
                               year-config->firstyear,index,
                               output->counts,output->offsets,config->rank,config->comm);
          break;
      }
      break;
    case LPJ_SOCKET:
      mpi_write_socket(output->socket,data,MPI_SHORT,config->total,
                       output->counts,output->offsets,config->rank,config->comm);
      break;
  } /* of switch */
   
#else
  int i;
  if(output->method==LPJ_FILES)
    switch(output->files[n].fmt)
    {
      case RAW: case CLM:
        fwrite(data,sizeof(short),config->count,output->files[n].fp.file);
        break;
      case TXT:
        for(i=0;i<config->count-1;i++)
          fprintf(output->files[n].fp.file,"%d ",data[i]);
        fprintf(output->files[n].fp.file,"%d\n",data[config->count-1]);
        break;
      case CDF:
        write_pft_short_netcdf(&output->files[n].fp.cdf,data,
                               year-config->firstyear,index,config->count);
        break;
    }
  else
    writeshort_socket(output->socket,data,config->count);
#endif
} /* of 'writeshortpft' */

void fwriteoutput_pft(Outputfile *output,  /**< Output file array */
                      Cell grid[],         /**< cell array */
                      int npft,            /**< number of natural PFTs */
                      int ncft,            /**< number of crop PFTs */
                      int year,            /**< simulation year (AD) */
                      const Config *config /**< LPJmL configuration */
                     )
{
  int count,i,cell,p,s;
  short *vec;
  float *fvec;
  Stand* stand;
  if(isopen(output,SDATE))
  {
    outindex(output,SDATE,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<ncft*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.sdate[i];
      writeshortpft(output,SDATE,vec,2*ncft,year,i,config);
    }
    free(vec);
  }
  if(isopen(output,HDATE))
  {
    outindex(output,HDATE,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<ncft*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.hdate[i];
      writeshortpft(output,HDATE,vec,2*ncft,year,i,config);
    }
    free(vec);
  }
  
  fvec=newvec(float,config->count);
  check(fvec);
  if(output->files[PFT_NPP].isopen)
  {
    outindex(output,PFT_NPP,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_npp[i];
      writepft(output,PFT_NPP,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
#ifdef IMAGE
  if (output->files[WFT_VEGC].isopen)
  {
    outindex(output, WFT_VEGC, config->rank);
    for (i = 0;i<config->nwft;i++)
    {    
      count = 0;
      for (cell = 0;cell<config->ngridcell;cell++)
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.wft_vegc[i];
      writepft(output, WFT_VEGC, fvec, config->nwft, year, i, config);
    }
  }
#endif
  if(output->files[PFT_GCGP].isopen)
  {
    outindex(output,PFT_GCGP,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          if(grid[cell].output.gcgp_count[i] > 0)
            grid[cell].output.pft_gcgp[i]/=grid[cell].output.gcgp_count[i];
          else
            grid[cell].output.pft_gcgp[i]=-9;
          fvec[count++]=(float)grid[cell].output.pft_gcgp[i];
        }
      writepft(output,PFT_GCGP,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[PFT_HARVEST].isopen)
  {
    outindex(output,PFT_HARVEST,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].harvest;
      writepft(output,PFT_HARVEST,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[PFT_RHARVEST].isopen)
  {
    outindex(output,PFT_RHARVEST,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].residual;
      writepft(output,PFT_RHARVEST,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_CONSUMP_WATER_G].isopen)
  {
    outindex(output,CFT_CONSUMP_WATER_G,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_consump_water_g[i];
      writepft(output,CFT_CONSUMP_WATER_G,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_CONSUMP_WATER_B].isopen)
  {
    outindex(output,CFT_CONSUMP_WATER_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_consump_water_b[i];
      writepft(output,CFT_CONSUMP_WATER_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[GROWING_PERIOD].isopen)
  {
    outindex(output,GROWING_PERIOD,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.growing_period[i];
      writepft(output,GROWING_PERIOD,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[FPC].isopen)
  {
    outindex(output,FPC,config->rank);
    for(i=0; i<(npft-config->nbiomass)+1;++i)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.fpc[i];
      writepft(output,FPC,fvec,(npft-config->nbiomass)+1,year,i,config);
    }
  }
  if(output->files[SOILC_LAYER].isopen)
  {
    outindex(output,SOILC_LAYER,config->rank);
    forrootsoillayer(i)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          fvec[count]=0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(i==0)
              for(p=0;p<stand->soil.litter.n;p++)
                fvec[count]+=(float)(stand->soil.litter.bg[p]*stand->frac);
            fvec[count]+=(float)((stand->soil.cpool[i].slow+stand->soil.cpool[i].fast)*stand->frac);
          }   
          count++;
        }
      }
      writepft(output,SOILC_LAYER,fvec,BOTTOMLAYER,year,i,config);
    }
  }
  if(output->files[CFT_PET].isopen)
  {
    outindex(output,CFT_PET,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_pet[i];
      writepft(output,CFT_PET,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_TRANSP].isopen)
  {
    outindex(output,CFT_TRANSP,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_transp[i];
      writepft(output,CFT_TRANSP,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_TRANSP_B].isopen)
  {
    outindex(output,CFT_TRANSP_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_transp_b[i];
      writepft(output,CFT_TRANSP_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_EVAP].isopen)
  {
    outindex(output,CFT_EVAP,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_evap[i];
        writepft(output,CFT_EVAP,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_EVAP_B].isopen)
  {
    outindex(output,CFT_EVAP_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_evap_b[i];
        writepft(output,CFT_EVAP_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_INTERC].isopen)
  {
    outindex(output,CFT_INTERC,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_interc[i];
      writepft(output,CFT_INTERC,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_INTERC_B].isopen)
  {
    outindex(output,CFT_INTERC_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_interc_b[i];
      writepft(output,CFT_INTERC_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_RETURN_FLOW_B].isopen)
  {
    outindex(output,CFT_RETURN_FLOW_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_return_flow_b[i];
      writepft(output,CFT_RETURN_FLOW_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_IRRIG_EVENTS].isopen)
  {
    outindex(output,CFT_IRRIG_EVENTS,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.cft_irrig_events[i];
      writeshortpft(output,CFT_IRRIG_EVENTS,vec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    free(vec);
  }
  if(output->files[CFT_NIR].isopen)
  {
    outindex(output,CFT_NIR,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
            fvec[count++]=(float)grid[cell].output.cft_nir[i];
        writepft(output,CFT_NIR,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
      }
    }
  if(output->files[CFT_TEMP].isopen)
  {
    outindex(output,CFT_TEMP,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_temp[i];
      writepft(output,CFT_TEMP,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_PREC].isopen)
  {
    outindex(output,CFT_PREC,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_prec[i];
      writepft(output,CFT_PREC,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_SRAD].isopen)
  {
    outindex(output,CFT_SRAD,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_srad[i];
      writepft(output,CFT_SRAD,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_ABOVEGBM].isopen)
  {
    outindex(output,CFT_ABOVEGBM,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_aboveground_biomass[i];
      writepft(output,CFT_ABOVEGBM,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_CONV_LOSS_EVAP].isopen)
  {
    outindex(output,CFT_CONV_LOSS_EVAP,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
            fvec[count++]=(float)grid[cell].output.cft_conv_loss_evap[i];
        writepft(output,CFT_CONV_LOSS_EVAP,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
      }
    }
  if(output->files[CFT_CONV_LOSS_DRAIN].isopen)
  {
    outindex(output,CFT_CONV_LOSS_DRAIN,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
            fvec[count++]=(float)grid[cell].output.cft_conv_loss_drain[i];
        writepft(output,CFT_CONV_LOSS_DRAIN,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
      }
    }
  if(output->files[CFTFRAC].isopen)
  {
    outindex(output,CFTFRAC,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cftfrac[i];
      writepft(output,CFTFRAC,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_AIRRIG].isopen)
  {
    outindex(output,CFT_AIRRIG,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_airrig[i];
      writepft(output,CFT_AIRRIG,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_FPAR].isopen)
  {
    outindex(output,CFT_FPAR,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_fpar[i];
      writepft(output,CFT_FPAR,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[LUC_IMAGE].isopen)
  {
    outindex(output,LUC_IMAGE,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_luc_image[i];
      writepft(output,LUC_IMAGE,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
#ifdef DOUBLE_HARVEST
  if(output->files[PFT_HARVEST2].isopen)
  {
    outindex(output,PFT_HARVEST2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].harvest;
      writepft(output,PFT_HARVEST2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[PFT_RHARVEST2].isopen)
  {
    outindex(output,PFT_RHARVEST2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].residual;
      writepft(output,PFT_RHARVEST2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[GROWING_PERIOD2].isopen)
  {
    outindex(output,GROWING_PERIOD2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.growing_period2[i];
      writepft(output,GROWING_PERIOD2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_PET2].isopen)
  {
    outindex(output,CFT_PET2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_pet2[i];
      writepft(output,CFT_PET2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_TRANSP2].isopen)
  {
    outindex(output,CFT_TRANSP2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_transp2[i];
      writepft(output,CFT_TRANSP2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_EVAP2].isopen)
  {
    outindex(output,CFT_EVAP2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_evap2[i];
        writepft(output,CFT_EVAP2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_INTERC2].isopen)
  {
    outindex(output,CFT_INTERC2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_interc2[i];
      writepft(output,CFT_INTERC2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_NIR2].isopen)
  {
    outindex(output,CFT_NIR2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_nir2[i];
      writepft(output,CFT_NIR2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_TEMP2].isopen)
  {
    outindex(output,CFT_TEMP2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_temp2[i];
      writepft(output,CFT_TEMP2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_PREC2].isopen)
  {
    outindex(output,CFT_PREC2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_prec2[i];
      writepft(output,CFT_PREC2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_SRAD2].isopen)
  {
    outindex(output,CFT_SRAD2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_srad2[i];
      writepft(output,CFT_SRAD2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_ABOVEGBM2].isopen)
  {
    outindex(output,CFT_ABOVEGBM2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_aboveground_biomass2[i];
      writepft(output,CFT_ABOVEGBM2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFTFRAC2].isopen)
  {
    outindex(output,CFTFRAC2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cftfrac2[i];
      writepft(output,CFTFRAC2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(output->files[CFT_AIRRIG2].isopen)
  {
    outindex(output,CFT_AIRRIG2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_airrig2[i];
      writepft(output,CFT_AIRRIG2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,SDATE2))
  {
    outindex(output,SDATE2,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<ncft*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.sdate2[i];
      writeshortpft(output,SDATE2,vec,2*ncft,year,i,config);
    }
    free(vec);
  }
  if(isopen(output,HDATE2))
  {
    outindex(output,HDATE2,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<ncft*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.hdate2[i];
      writeshortpft(output,HDATE2,vec,2*ncft,year,i,config);
    }
    free(vec);
  }
  if(isopen(output,SYEAR))
  {
    outindex(output,SYEAR,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<ncft*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.syear[i];
      writeshortpft(output,SYEAR,vec,2*ncft,year,i,config);
    }
    free(vec);
  }
  if(isopen(output,SYEAR2))
  {
    outindex(output,SYEAR2,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<ncft*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.syear2[i];
      writeshortpft(output,SYEAR2,vec,2*ncft,year,i,config);
    }
    free(vec);
  }

#endif
  free(fvec);
} /* of 'fwriteoutput_pft' */

/*
- order of PFT-specific output:

0-8  9-19   20-21           22               23              24-34     35-36        37-38
PFT  CFT    PASTURE/OTHEr   BIOMASS-GRASS   BIOMASS-TREE    CFT_irr   PASTURE_irr  biomass-irr
 


 - order of CFT-specific output:

0-10   12-12           13              14              15-25     26-27        28-29
CFT    PASTURE/OTHEr   BIOMASS-GRASS   BIOMASS-TREE    CFT_irr   PASTURE_irr  biomass-irr


*/      
