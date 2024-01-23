/**************************************************************************************/
/**                                                                                \n**/
/**  f  w  r  i  t  e  o  u  t  p  u  t  _  m  o  n  t  h  l  y  2  .  c           \n**/
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

static void writemonth(Outputfile *output,int index,float *data,int year,
                       const Config *config)
{
  int i,month;
#ifdef USE_MPI
  MPI_Status status;
#endif
  if(output->files[index].isopen)
  {
    for(i=0;i<config->count*NMONTH;i++)
      data[i]*=config->outnames[index].scale;
#ifdef USE_MPI
    switch(output->method)
    {
      case LPJ_MPI2:
        for(month=0;month<NMONTH;month++)
          MPI_File_write_at(output->files[index].fp.mpi_file,
                          ((year-config->firstyear)*NMONTH+month)*config->total+config->offset,
                          data+config->count*month,config->count,MPI_FLOAT,
                          &status);
        break;
      case LPJ_GATHER:
        switch(output->files[index].fmt)
        {
          case RAW: case CLM:
            for(month=0;month<NMONTH;month++)
              mpi_write(output->files[index].fp.file,data+config->count*month,
                        MPI_FLOAT,config->total,output->counts,
                        output->offsets,config->rank,config->comm);
             break;
          case TXT:
            for(month=0;month<NMONTH;month++)
              mpi_write_txt(output->files[index].fp.file,data+config->count*month,
                            MPI_FLOAT,config->total,output->counts,
                            output->offsets,config->rank,config->comm);
            break;
          case CDF:
            for(month=0;month<NMONTH;month++)
              mpi_write_netcdf(&output->files[index].fp.cdf,
                               data+config->count*month,
                               MPI_FLOAT,
                               config->total,
                               output->files[index].oneyear ? month : (year-config->firstyear)*12+month,
                               output->counts,output->offsets,config->rank,
                               config->comm);
             break;
        }
        break;
      case LPJ_SOCKET:
        for(month=0;month<NMONTH;month++)
        {
          if(isroot(*config))
            writeint_socket(output->socket,&index,1);
          mpi_write_socket(output->socket,data+config->count*month,MPI_FLOAT,
                           config->total,output->counts,output->offsets,
                           config->rank,config->comm);
        }
        break;
    } /* of switch */
#else
    if(output->method==LPJ_SOCKET)
      for(month=0;month<NMONTH;month++)
      {
        writeint_socket(output->socket,&index,1);
        writefloat_socket(output->socket,data+config->count*month,config->count);
      }
    else
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          if(fwrite(data,sizeof(float),config->count*NMONTH,output->files[index].fp.file)!=config->count*NMONTH)
            fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
          break;
        case TXT:
          for(i=0;i<config->count*NMONTH-1;i++)
            fprintf(output->files[index].fp.file,"%g ",data[i]);
          fprintf(output->files[index].fp.file,"%g\n",data[config->count*NMONTH-1]);
          break;
        case CDF:
          for(month=0;month<NMONTH;month++)
            write_float_netcdf(&output->files[index].fp.cdf,data+config->count*month,
                               output->files[index].oneyear ? month : (year-config->firstyear)*12+month,
                               config->count);
          break;
      }
#endif
  }
} /* of 'writemonth' */

void fwriteoutput_monthly2(Outputfile *output,
                           const Outputmonth *data,
                           int year,
                           const Config *config)
{
  writemonth(output,MNPP,data->mnpp,year,config);
  writemonth(output,MGPP,data->mgpp,year,config);
  writemonth(output,MRH,data->mrh,year,config);
  writemonth(output,MRUNOFF,data->mrunoff,year,config);
  writemonth(output,MDISCHARGE,data->mdischarge,year,config);
  writemonth(output,MWATERAMOUNT,data->mwateramount,year,config);
  writemonth(output,MTRANSP,data->mtransp,year,config);
  writemonth(output,MTRANSP_B,data->mtransp_b,year,config);
  writemonth(output,MEVAP,data->mevap,year,config);
  writemonth(output,MEVAP_B,data->mevap_b,year,config);
  writemonth(output,MINTERC,data->minterc,year,config);
  writemonth(output,MINTERC_B,data->minterc_b,year,config);
  writemonth(output,MCONV_LOSS_EVAP,data->mconv_loss_evap,year,config);
  writemonth(output,MCONV_LOSS_DRAIN,data->mconv_loss_drain,year,config);
  writemonth(output,MPET,data->mpet,year,config);
  writemonth(output,MSWC1,data->mswc1,year,config);
  writemonth(output,MSWC2,data->mswc2,year,config);
  writemonth(output,MSWC3,data->mswc3,year,config);
  writemonth(output,MSWC4,data->mswc4,year,config);
  writemonth(output,MSWC5,data->mswc5,year,config);
  writemonth(output,MIRRIG,data->mirrig,year,config);
  writemonth(output,MRETURN_FLOW_B,data->mreturn_flow_b,year,config);
  writemonth(output,MWD_LOCAL,data->mwd_local,year,config);
  writemonth(output,MWD_NEIGHB,data->mwd_neighb,year,config);
  writemonth(output,MWD_RES,data->mwd_res,year,config);
  writemonth(output,MEVAP_LAKE,data->mevap_lake,year,config);
  writemonth(output,MEVAP_RES,data->mevap_res,year,config);
  writemonth(output,MPREC_RES,data->mprec_res,year,config);
  writemonth(output,MFIREC,data->mfirec,year,config);
  writemonth(output,MNFIRE,data->mnfire,year,config);
  writemonth(output,MFIREDI,data->mfiredi,year,config);
  writemonth(output,MFIREEMISSION,data->mfireemission,year,config);
  writemonth(output,MTEMP_IMAGE,data->mtemp_image,year,config);
  writemonth(output,MPREC_IMAGE,data->mprec_image,year,config);
  writemonth(output,MSUN_IMAGE,data->msun_image,year,config);
  writemonth(output,MWET_IMAGE,data->mwet_image,year,config);
  writemonth(output,MSOILTEMP1,data->msoiltemp1,year,config);
  writemonth(output,MSOILTEMP2,data->msoiltemp2,year,config);
  writemonth(output,MSOILTEMP3,data->msoiltemp3,year,config);
  writemonth(output,MSOILTEMP4,data->msoiltemp4,year,config);
  writemonth(output,MSOILTEMP5,data->msoiltemp5,year,config);
  writemonth(output,MSOILTEMP6,data->msoiltemp6,year,config);
  writemonth(output,MPREC,data->mprec,year,config);
  writemonth(output,MRAIN,data->mrain,year,config);
  writemonth(output,MSNOWF,data->msnowf,year,config);
  writemonth(output,MMELT,data->mmelt,year,config);
  writemonth(output,MSWE,data->mswe,year,config);
  writemonth(output,MSNOWRUNOFF,data->msnowrunoff,year,config);
  writemonth(output,MRUNOFF_SURF,data->mrunoff_surf,year,config);
  writemonth(output,MRUNOFF_LAT,data->mrunoff_lat,year,config);
  writemonth(output,MSEEPAGE,data->mseepage,year,config);
  writemonth(output,MROOTMOIST,data->mrootmoist,year,config);
  writemonth(output,MGCONS_RF,data->mgcons_rf,year,config);
  writemonth(output,MGCONS_IRR,data->mgcons_irr,year,config);
  writemonth(output,MBCONS_IRR,data->mbcons_irr,year,config);
  writemonth(output,MBURNTAREA,data->mburntarea,year,config);
  writemonth(output,MFAPAR,data->mfapar,year,config);  
  writemonth(output,MALBEDO,data->malbedo,year,config);  
  writemonth(output,MPHEN_TMIN,data->mphen_tmin,year,config);  
  writemonth(output,MPHEN_TMAX,data->mphen_tmax,year,config); 
  writemonth(output,MPHEN_LIGHT,data->mphen_light,year,config); 
  writemonth(output,MPHEN_WATER,data->mphen_water,year,config); 
  writemonth(output,MWSCAL,data->mwscal,year,config);   
  writemonth(output,MIRRIG_RW,data->mirrig_rw,year,config);
  writemonth(output,MUNMET_DEMAND,data->munmet_demand,year,config);
  writemonth(output,MLAKEVOL,data->mlakevol,year,config);
  writemonth(output,MLAKETEMP,data->mlaketemp,year,config);
} /* of 'fwriteoutput_monthly2' */
