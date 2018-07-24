/**************************************************************************************/
/**                                                                                \n**/
/**      f  w  r  i  t  e  o  u  t  p  u  t  _  a  n  n  u  a  l  .  c             \n**/
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
#include "grass.h"
#include "tree.h"

static void writeannual(Outputfile *output,int index,float data[],int year,
                        const Config *config)
{
  int i;
#ifdef USE_MPI
  MPI_Status status;
#endif
  for(i=0;i<config->count;i++)
    data[i]*=(float)config->outnames[index].scale;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
         (year-config->firstyear)*config->total+config->offset,data,config->count,
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
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,config->total,
                           output->files[index].oneyear ? NO_TIME : year-config->firstyear,
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
        write_float_netcdf(&output->files[index].fp.cdf,data,
                           output->files[index].oneyear ? NO_TIME : year-config->firstyear,
                           config->count);
        break;
    }
  else
  {
    writeint_socket(output->socket,&index,1);
    writefloat_socket(output->socket,data,config->count);
  }
#endif
} /* of 'writeannual' */

static void writeshortannual(Outputfile *output,int index,short data[],int year,
                             const Config *config)
{
#ifdef USE_MPI
  MPI_Status status;
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
         (year-config->firstyear)*config->total+config->offset,data,config->count,
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
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_SHORT,config->total,
                           output->files[index].oneyear ? NO_TIME : year-config->firstyear,
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
        write_short_netcdf(&output->files[index].fp.cdf,data,
                           output->files[index].oneyear ? NO_TIME : year-config->firstyear,
                           config->count);
        break;
    }
  else
  {
    writeint_socket(output->socket,&index,1);
    writeshort_socket(output->socket,data,config->count);
  }
#endif
} /* of 'writeshortannual' */

static void writeannualall(Outputfile *output,int index,float data[],int year,
                           const Config *config)
{
  int i;
#ifdef USE_MPI
  int *counts,*offsets;
  MPI_Status status;
#endif
  for(i=0;i<config->ngridcell;i++)
    data[i]*=(float)config->outnames[index].scale;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,
        (year-config->firstyear)*config->nall+config->offset,data,config->ngridcell,
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
          mpi_write_netcdf(&output->files[index].fp.cdf,data,MPI_FLOAT,config->nall,
                           output->files[index].oneyear ? NO_TIME : year-config->firstyear,
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
        write_float_netcdf(&output->files[index].fp.cdf,data,
                           output->files[index].oneyear ? NO_TIME : year-config->firstyear,
                           config->ngridcell);
        break;
    }
  else
  {
    writeint_socket(output->socket,&index,1);
    writefloat_socket(output->socket,data,config->ngridcell);
  }
#endif
} /* of 'writeannualall' */

void fwriteoutput_annual(Outputfile *output,  /**< output file array */
                         const Cell grid[],   /**< grid cell array */
                         int year,            /**< simulation year (AD) */
                         const Config *config /**< LPJ configuration */
                        )
{
  int count,s,p,cell,l;
  Stand *stand;
  Pft *pft;
  float *vec;
  short *svec;
  if(output->files[SEASONALITY].isopen)
  {
    count=0;
    svec=newvec(short,config->ngridcell);
    check(svec);
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        svec[count++]=(short)(grid[cell].ml.seasonality_type);
    writeshortannual(output,SEASONALITY,svec,year,config);
    free(svec);
  }
  vec=newvec(float,config->ngridcell);
  check(vec);
  if(output->files[FIREC].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.firec;
    writeannual(output,FIREC,vec,year,config);
  }
  if(output->files[ABURNTAREA].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.aburntarea;
    writeannual(output,ABURNTAREA,vec,year,config);
  }
  if(output->files[FLUX_FIREWOOD].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.flux_firewood;
    writeannual(output,FLUX_FIREWOOD,vec,year,config);
  }
  if(output->files[FIREF].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.firef;
    writeannual(output,FIREF,vec,year,config);
  }
  if(output->files[VEGC].isopen)
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
    writeannual(output,VEGC,vec,year,config);
  }
  if(output->files[SOILC].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          for(p=0;p<stand->soil.litter.n;p++)
            vec[count]+=(float)(stand->soil.litter.bg[p]*stand->frac);
          forrootsoillayer(l)
            vec[count]+=(float)((stand->soil.cpool[l].slow+stand->soil.cpool[l].fast)*stand->frac);
        }
        count++;
      }
    writeannual(output,SOILC,vec,year,config);
  }
  if(output->files[SOILC_SLOW].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        vec[count]=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          forrootsoillayer(l)
            vec[count]+=(float)((stand->soil.cpool[l].slow)*stand->frac);
          vec[count]+=(float)(stand->soil.YEDOMA*stand->frac);
        }
        count++;
      }
    writeannual(output,SOILC_SLOW,vec,year,config);
  }
  if(output->files[LITC].isopen)
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
      writeannual(output,LITC,vec,year,config);
  }
  if(output->files[MAXTHAW_DEPTH].isopen)
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
      writeannual(output,MAXTHAW_DEPTH,vec,year,config);
  }
  if(output->files[FLUX_ESTAB].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.flux_estab;
    writeannual(output,FLUX_ESTAB,vec,year,config);
  }
  if(output->files[HARVEST].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.flux_harvest;
      writeannual(output,HARVEST,vec,year,config);
  }
  if(output->files[RHARVEST_BURNT].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.flux_rharvest_burnt;
      writeannual(output,RHARVEST_BURNT,vec,year,config);
  }
  if(output->files[RHARVEST_BURNT_IN_FIELD].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].output.flux_rharvest_burnt_in_field;
      writeannual(output,RHARVEST_BURNT_IN_FIELD,vec,year,config);
  }

  if(output->files[MG_VEGC].isopen)
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
      writeannual(output,MG_VEGC,vec,year,config);
  }
  if(output->files[MG_SOILC].isopen)
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
                vec[count]+=(float)(stand->soil.litter.bg[p]*stand->frac);
              forrootsoillayer(l)
              vec[count]+=(float)((stand->soil.cpool[l].slow+stand->soil.cpool[l].fast)*stand->frac);
            }
          }
          count++;
        }
      writeannual(output,MG_SOILC,vec,year,config);
  }  
  if(output->files[MG_LITC].isopen)
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
    writeannual(output,MG_LITC,vec,year,config);
  }
  if(output->files[APREC].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)grid[cell].balance.aprec;
    writeannual(output,APREC,vec,year,config);
  }
  if(output->files[INPUT_LAKE].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.input_lake*1e-9);
    writeannual(output,INPUT_LAKE,vec,year,config);
  }
  if(output->files[ADISCHARGE].isopen)
  {
    for(cell=0;cell<config->ngridcell;cell++)
      vec[cell]=(float)(grid[cell].output.adischarge*1e-9);
    writeannualall(output,ADISCHARGE,vec,year,config);
  }
  if(output->files[PROD_TURNOVER].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.prod_turnover);
    writeannual(output,PROD_TURNOVER,vec,year,config);
  }
  if(output->files[DEFOREST_EMIS].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.deforest_emissions);
    writeannual(output,DEFOREST_EMIS,vec,year,config);
  }
  if(output->files[TRAD_BIOFUEL].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.trad_biofuel);
    writeannual(output,TRAD_BIOFUEL,vec,year,config);
  }
  if(output->files[AIRRIG].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.airrig);
    writeannual(output,AIRRIG,vec,year,config);
  }
  if(output->files[FBURN].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.fburn);
    writeannual(output,FBURN,vec,year,config);
  }
  if(output->files[FTIMBER].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.ftimber);
    writeannual(output,FTIMBER,vec,year,config);
  }
  if(output->files[TIMBER_HARVEST].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.timber_harvest);
    writeannual(output,TIMBER_HARVEST,vec,year,config);
  }
  if(output->files[PRODUCT_POOL_FAST].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.product_pool_fast);
    writeannual(output,PRODUCT_POOL_FAST,vec,year,config);
  }
  if(output->files[PRODUCT_POOL_SLOW].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.product_pool_slow);
    writeannual(output,PRODUCT_POOL_SLOW,vec,year,config);
  }
  if(output->files[AFRAC_WD_UNSUST].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.awd_unsustainable/((grid[cell].output.airrig+
            grid[cell].output.aconv_loss_evap + grid[cell].output.aconv_loss_drain)*grid[cell].coord.area));
    writeannual(output,AFRAC_WD_UNSUST,vec,year,config);
  }
  if(output->files[ACONV_LOSS_EVAP].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.aconv_loss_evap);
    writeannual(output,ACONV_LOSS_EVAP,vec,year,config);
  }
  if(output->files[ACONV_LOSS_DRAIN].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.aconv_loss_drain);
    writeannual(output,ACONV_LOSS_DRAIN,vec,year,config);
  }
  if(output->files[AWATERUSE_HIL].isopen)
  {
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
        vec[count++]=(float)(grid[cell].output.awateruse_hil);
    writeannual(output,AWATERUSE_HIL,vec,year,config);
  }
  if(output->files[AGB].isopen)
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
    writeannual(output,AGB,vec,year,config);
  }
  if(output->files[AGB_TREE].isopen)
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
    writeannual(output,AGB_TREE,vec,year,config);
  }
  free(vec);
} /* of 'fwriteoutput_annual' */
