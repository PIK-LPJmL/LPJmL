/**************************************************************************************/
/**                                                                                \n**/
/**    f  w  r  i  t  e  o  u  t  p  u  t  _  p  f  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes PFT-specific output into file                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define writeoutputvar(index,name,n) if(isopen(output,index))\
  {\
    outindex(output,index,config->rank);\
    for(i=0;i<n;i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          fvec[count++]=(float)grid[cell].output.name[i];\
      writepft(output,index,fvec,n,year,i,config);\
    }\
  }

#define writeoutputshortvar(index,name,n) if(isopen(output,index))\
  {\
    outindex(output,index,config->rank);\
    vec=newvec(short,config->count);\
    check(vec);\
    for(i=0;i<n;i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          vec[count++]=(short)grid[cell].output.name[i];\
      writeshortpft(output,index,vec,n,year,i,config);\
    }\
    free(vec);\
  }

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
    data[i]=config->outnames[n].scale*data[i]+config->outnames[n].offset;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[n].fp.mpi_file,
                        ((year-config->outputyear)*size+index)*config->total+config->offset,
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
                               output->files[n].oneyear ? NO_TIME : year-config->outputyear,
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
                               output->files[n].oneyear ? NO_TIME : year-config->outputyear,
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
                        ((year-config->outputyear)*size+index)*config->total+config->offset,
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
                               year-config->outputyear,index,
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
                               year-config->outputyear,index,config->count);
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
  writeoutputshortvar(SDATE,sdate,2*ncft);
  writeoutputshortvar(HDATE,hdate,2*ncft);
  fvec=newvec(float,config->count);
  check(fvec);
  writeoutputvar(PFT_NPP,pft_npp,(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2);
  if(isopen(output,PFT_GCGP))
  {
    outindex(output,PFT_GCGP,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
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
      writepft(output,PFT_GCGP,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_HARVEST))
  {
    outindex(output,PFT_HARVEST,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].harvest;
      writepft(output,PFT_HARVEST,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_RHARVEST))
  {
    outindex(output,PFT_RHARVEST,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].residual;
      writepft(output,PFT_RHARVEST,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  writeoutputvar(CFT_CONSUMP_WATER_G,cft_consump_water_g,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_CONSUMP_WATER_B,cft_consump_water_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(GROWING_PERIOD,growing_period,2*(ncft+NGRASS));
  writeoutputvar(FPC,fpc,npft-config->nbiomass+1);
  if(isopen(output,SOILC_LAYER))
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
  writeoutputvar(CFT_PET,cft_pet,2*(ncft+NGRASS));
  writeoutputvar(CFT_TRANSP,cft_transp,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_TRANSP_B,cft_transp_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_EVAP,cft_evap,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_EVAP_B,cft_evap_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_INTERC,cft_interc,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_INTERC_B,cft_interc_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_RETURN_FLOW_B,cft_return_flow_b,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputshortvar(CFT_IRRIG_EVENTS,cft_irrig_events,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_NIR,cft_nir,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_TEMP,cft_temp,2*(ncft+NGRASS));
  writeoutputvar(CFT_PREC,cft_prec,2*(ncft+NGRASS));
  writeoutputvar(CFT_SRAD,cft_srad,2*(ncft+NGRASS));
  writeoutputvar(CFT_ABOVEGBM,cft_aboveground_biomass,2*(ncft+NGRASS));
  writeoutputvar(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFTFRAC,cftfrac,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_AIRRIG,cft_airrig,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_FPAR,cft_fpar,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(LUC_IMAGE,cft_luc_image,2*(ncft+NGRASS+NBIOMASSTYPE));
#ifdef DOUBLE_HARVEST
  if(isopen(output,PFT_HARVEST2))
  {
    outindex(output,PFT_HARVEST2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].harvest;
      writepft(output,PFT_HARVEST2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_RHARVEST2))
  {
    outindex(output,PFT_RHARVEST2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].residual;
      writepft(output,PFT_RHARVEST2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  writeoutputvar(GROWING_PERIOD2,growing_period2,2*(ncft+NGRASS));
  writeoutputvar(CFT_PET2,cft_pet2,2*(ncft+NGRASS));
  writeoutputvar(CFT_TRANSP2,cft_transp2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_EVAP2,cft_evap2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_INTERC2,cft_interc2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_NIR2,cft_nir2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_TEMP2,cft_temp2,2*(ncft+NGRASS));
  writeoutputvar(CFT_PREC2,cft_prec2,2*(ncft+NGRASS));
  writeoutputvar(CFT_SRAD2,cft_srad2,2*(ncft+NGRASS));
  writeoutputvar(CFT_ABOVEGBM2,cft_aboveground_biomass2,2*(ncft+NGRASS));
  writeoutputvar(CFTFRAC2,cftfrac2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputvar(CFT_AIRRIG2,cft_airrig2,2*(ncft+NGRASS+NBIOMASSTYPE));
  writeoutputshortvar(SDATE2,sdate2,2*ncft);
  writeoutputshortvar(HDATE2,hdate2,2*ncft);
  writeoutputshortvar(SYEAR,syear,2*ncft);
  writeoutputshortvar(SYEAR2,syear2,2*ncft);
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
