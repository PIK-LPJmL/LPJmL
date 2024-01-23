/**************************************************************************************/
/**                                                                                \n**/
/**   f  w  r  i  t  e  o  u  t  p  u  t  _  m  o  n  t  h  l  y  .  c             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes monthly output into files                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define writeoutputvar(index,name) if(isopen(output,index))\
  {\
    count=0;\
    for(cell=0;cell<config->ngridcell;cell++)\
      if(!grid[cell].skip)\
        fvec[count++]=(float)grid[cell].output.name;\
    writemonth(output,index,fvec,year,month,config);\
  }

static void writemonth(Outputfile *output,int index,float *data,int year,
                       int month,
                       const Config *config)
{
  int i;
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
                        ((year-config->outputyear)*NMONTH+month)*config->total+config->offset,
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
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,
                           config->total,
                           output->files[index].oneyear ? month : (year-config->outputyear)*12+month,
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
        write_float_netcdf(&output->files[index].fp.cdf,data,
                           output->files[index].oneyear ? month : (year-config->outputyear)*12+month,
                           config->count);
        break;
    }
#endif
} /* of 'writemonth' */

static void writemonth2(Outputfile *output,int index,float *data,int year,
                        int month,int layer,int nlayer,const Config *config)
{
  int i;
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
                        ((year-config->outputyear)*NMONTH*nlayer+month*nlayer+layer)*config->total+config->offset,
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
          mpi_write_pft_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,
                               config->total,
                               output->files[index].oneyear ? month : (year-config->outputyear)*12+month,
                               layer,
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
        write_pft_float_netcdf(&output->files[index].fp.cdf,data,
                               output->files[index].oneyear ? month : (year-config->outputyear)*12+month,
                               layer,
                               config->count);
        break;
    }
#endif
} /* of 'writemonth2' */

void fwriteoutput_monthly(Outputfile *output, /**< Output data */
                          const Cell grid[],  /**< LPJ cell array */
                          int month,          /**< month of year (0..11) */
                          int year,           /**< year (AD) */
                          const Config *config /**< LPJmL configuration */
                         )
{

  int l,count,cell;
  float *fvec;
  fvec=newvec(float,config->count);
  check(fvec);
  writeoutputvar(MNPP,mnpp);
  writeoutputvar(MGPP,mgpp);
  writeoutputvar(MRH,mrh);
  writeoutputvar(MRUNOFF,mrunoff);
  writeoutputvar(MDISCHARGE,mdischarge);
  writeoutputvar(MWATERAMOUNT,mwateramount);
  writeoutputvar(MTRANSP,mtransp);
  writeoutputvar(MTRANSP_B,mtransp_b);
  writeoutputvar(MEVAP,mevap);
  writeoutputvar(MEVAP_B,mevap_b);
  writeoutputvar(MINTERC,minterc);
  writeoutputvar(MINTERC_B,minterc_b);
  writeoutputvar(MPET,mpet);
  if(isopen(output,MSWC))
    for(l=0;l<NSOILLAYER;l++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.mswc[l];
      writemonth2(output,MSWC,fvec,year,month,l,NSOILLAYER,config);
    }
  writeoutputvar(MSWC1,mswc[0]);
  writeoutputvar(MSWC2,mswc[1]);
  writeoutputvar(MSWC3,mswc[2]);
  writeoutputvar(MSWC4,mswc[3]);
  writeoutputvar(MSWC5,mswc[4]);
  writeoutputvar(MROOTMOIST,mrootmoist);
  writeoutputvar(MIRRIG,mirrig);
  writeoutputvar(MRETURN_FLOW_B,mreturn_flow_b);
  writeoutputvar(MEVAP_LAKE,mevap_lake);
  writeoutputvar(MEVAP_RES,mevap_res);
  writeoutputvar(MPREC_RES,mprec_res);
  writeoutputvar(MFIREC,mfirec);
  writeoutputvar(MNFIRE,mnfire);
  writeoutputvar(MFIREDI,mfiredi);
  writeoutputvar(MFIREEMISSION_CO2,mfireemission.co2);
  writeoutputvar(MFIREEMISSION_CO,mfireemission.co);
  writeoutputvar(MFIREEMISSION_CH4,mfireemission.ch4);
  writeoutputvar(MFIREEMISSION_VOC,mfireemission.voc);
  writeoutputvar(MFIREEMISSION_TPM,mfireemission.tpm);
  writeoutputvar(MFIREEMISSION_NOX,mfireemission.nox);
  writeoutputvar(MBURNTAREA,mburntarea);
  writeoutputvar(MTEMP_IMAGE,mtemp_image);
  writeoutputvar(MPREC_IMAGE,mprec_image);
  writeoutputvar(MSUN_IMAGE,msun_image);
  writeoutputvar(MWET_IMAGE,mwet_image);
  if(isopen(output,MSOILTEMP))
    for(l=0;l<NSOILLAYER;l++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.msoiltemp[l];
      writemonth2(output,MSOILTEMP,fvec,year,month,l,NSOILLAYER,config);
    }
  writeoutputvar(MSOILTEMP1,msoiltemp[0]);
  writeoutputvar(MSOILTEMP2,msoiltemp[1]);
  writeoutputvar(MSOILTEMP3,msoiltemp[2]);
  writeoutputvar(MSOILTEMP4,msoiltemp[3]);
  writeoutputvar(MSOILTEMP5,msoiltemp[4]);
  writeoutputvar(MSOILTEMP6,msoiltemp[5]);
  writeoutputvar(MRES_STORAGE,mres_storage);
  writeoutputvar(MRES_DEMAND,mres_demand);
  writeoutputvar(MTARGET_RELEASE,mtarget_release);
  writeoutputvar(MRES_CAP,mres_cap);
  writeoutputvar(MWD_UNSUST,mwd_unsustainable);
  writeoutputvar(MUNMET_DEMAND,munmet_demand);
  writeoutputvar(MWD_LOCAL,mwd_local);
  writeoutputvar(MWD_NEIGHB,mwd_neighb);
  writeoutputvar(MWD_RES,mwd_res);
  writeoutputvar(MWD_RETURN,mwd_return);
  writeoutputvar(MCONV_LOSS_EVAP,mconv_loss_evap);
  writeoutputvar(MCONV_LOSS_DRAIN,mconv_loss_drain);
  writeoutputvar(MSTOR_RETURN,mstor_return);
  writeoutputvar(MPREC,mprec);
  writeoutputvar(MRAIN,mrain);
  writeoutputvar(MSNOWF,msnowf);
  writeoutputvar(MMELT,mmelt);
  writeoutputvar(MSWE,mswe);
  writeoutputvar(MSNOWRUNOFF,msnowrunoff);
  writeoutputvar(MRUNOFF_SURF,mrunoff_surf);
  writeoutputvar(MRUNOFF_LAT,mrunoff_lat);
  writeoutputvar(MSEEPAGE,mseepage);
  writeoutputvar(MFAPAR,mfapar);
  writeoutputvar(MALBEDO,malbedo);
  writeoutputvar(MPHEN_TMIN,mphen_tmin);
  writeoutputvar(MPHEN_TMAX,mphen_tmax);
  writeoutputvar(MPHEN_LIGHT,mphen_light);
  writeoutputvar(MPHEN_WATER,mphen_water);
  writeoutputvar(MWSCAL,mwscal);
  writeoutputvar(MGCONS_RF,mgcons_rf);
  writeoutputvar(MGCONS_IRR,mgcons_irr);
  writeoutputvar(MBCONS_IRR,mbcons_irr);
  writeoutputvar(MIRRIG_RW,mirrig_rw);
  writeoutputvar(MLAKEVOL,mlakevol);
  writeoutputvar(MLAKETEMP,mlaketemp);
  free(fvec);
} /* of 'fwriteoutput_monthly' */
