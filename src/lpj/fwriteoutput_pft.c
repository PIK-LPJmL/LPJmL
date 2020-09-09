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
#include "agriculture.h"
#include "grass.h"
#include "tree.h"

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

#define writeoutputvaritem(index,name,item,n) if(isopen(output,index))\
  {\
    outindex(output,index,config->rank);\
    for(i=0;i<n;i++)\
    {\
      count=0;\
      for(cell=0;cell<config->ngridcell;cell++)\
        if(!grid[cell].skip)\
          fvec[count++]=(float)grid[cell].output.name[i].item;\
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
  Real scale;
  int i;
#ifdef USE_MPI
  MPI_Status status;
#endif
  switch(config->outnames[n].time)
  {
    case SECOND:
      scale=1./NSECONDSDAY/NDAYYEAR;
      break;
    case DAY:
      scale=1./NDAYYEAR;
      break;
    default:
      scale=1;
  }
  for(i=0;i<config->count;i++)
    data[i]=config->outnames[n].scale*scale*data[i]+config->outnames[n].offset;

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
  const Stand* stand;
  const Pft *pft;
  const Pfttree *tree;
  const Pftgrass *grass;
  const Irrigation *data;
  writeoutputshortvar(SDATE,sdate,2*ncft);
  writeoutputshortvar(HDATE,hdate,2*ncft);
  fvec=newvec(float,config->count);
  check(fvec);
  writeoutputvar(PFT_NPP,pft_npp,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  writeoutputvar(PFT_NUPTAKE,pft_nuptake,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
  writeoutputvar(PFT_NDEMAND,pft_ndemand,(npft-config->nbiomass-config->nwft)+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2);
#if defined IMAGE || defined INCLUDEWP
  writeoutputvar(WFT_VEGC,wft_vegc,config->nwft);
#endif
  if(isopen(output,PFT_NLIMIT))
  {
    outindex(output,PFT_NLIMIT,config->rank);
    for (i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                  fvec[count] = (float)pft->nlimit / NDAYYEAR;
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_NLIMIT,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass-config->nwft;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_nlimit[p];
      }
      writepft(output,PFT_NLIMIT,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_VEGC))
  {
    outindex(output,PFT_VEGC,config->rank);
    for (i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                  fvec[count] = (float)vegc_sum(pft);
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_VEGC,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_veg[p].carbon;
      }
      writepft(output,PFT_VEGC,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_VEGN))
  {
    outindex(output,PFT_VEGN,config->rank);
    for (i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if (stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist)
              {
                if (i == pft->par->id)
                  fvec[count] = (float)vegn_sum(pft);
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_VEGN,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_veg[p].nitrogen;
      }
      writepft(output,PFT_VEGN,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_CLEAF))
  {
    outindex(output,PFT_CLEAF,config->rank);
    for (i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
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
                    fvec[count] = (float)tree->ind.leaf.carbon;
                  }
                  else
                  {
                    grass = pft->data;
                    fvec[count] = (float)grass->ind.leaf.carbon;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_CLEAF,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_leaf[p].carbon;
      }
      writepft(output,PFT_CLEAF,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_NLEAF))
  {
    outindex(output,PFT_NLEAF,config->rank);
    for (i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
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
                    fvec[count] = (float)tree->ind.leaf.nitrogen;
                  }
                  else
                  {
                    grass = pft->data;
                    fvec[count] = (float)grass->ind.leaf.nitrogen;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_NLEAF,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_leaf[p].nitrogen;
      }
      writepft(output,PFT_NLEAF,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_GCGP))
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
          fvec[count++]=(float)grid[cell].output.pft_gcgp[i];
        }
      writepft(output,PFT_GCGP,fvec,(npft-config->nbiomass-config->nwft)+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  writeoutputvaritem(PFT_HARVESTC,pft_harvest,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_HARVESTN,pft_harvest,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTC,pft_harvest,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTN,pft_harvest,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_CONSUMP_WATER_G,cft_consump_water_g,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_CONSUMP_WATER_B,cft_consump_water_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(GROWING_PERIOD,growing_period,2*(ncft+NGRASS));
  writeoutputvar(FPC,fpc,npft-config->nbiomass-config->nwft+1);
  writeoutputvar(PFT_MORT,pft_mort,npft-config->nbiomass-config->nwft);
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
                fvec[count]+=(float)(stand->soil.litter.bg[p].carbon*stand->frac);
            fvec[count]+=(float)((stand->soil.pool[i].slow.carbon+stand->soil.pool[i].fast.carbon)*stand->frac);
          }   
          count++;
        }
      }
      writepft(output,SOILC_LAYER,fvec,BOTTOMLAYER,year,i,config);
    }
  }
  if(isopen(output,SOILN_LAYER))
  {
    outindex(output,SOILN_LAYER,config->rank);
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
                fvec[count]+=(float)(stand->soil.litter.bg[p].nitrogen*stand->frac);
            fvec[count]+=(float)((stand->soil.pool[i].slow.nitrogen+stand->soil.pool[i].fast.nitrogen)*stand->frac);
          }
          count++;
        }
      }
      writepft(output,SOILN_LAYER,fvec,BOTTOMLAYER,year,i,config);
    }
  }
  if(isopen(output,SOILNO3_LAYER))
  {
    outindex(output, SOILNO3_LAYER, config->rank);
    forrootsoillayer(i)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            fvec[count] += (float)(stand->soil.NO3[i]*stand->frac);
          }
          count++;
        }
      }
      writepft(output, SOILNO3_LAYER, fvec, BOTTOMLAYER, year, i, config);
    }
  }
  if(isopen(output,SOILNH4_LAYER))
  {
    outindex(output, SOILNH4_LAYER, config->rank);
    forrootsoillayer(i)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            fvec[count] += (float)(stand->soil.NH4[i]*stand->frac);
          }
          count++;
        }
      }
      writepft(output,SOILNH4_LAYER,fvec,BOTTOMLAYER,year,i,config);
    }
  }
  writeoutputvar(CFT_PET,cft_pet,2*(ncft+NGRASS));
  writeoutputvar(CFT_TRANSP,cft_transp,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_TRANSP_B,cft_transp_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_EVAP,cft_evap,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_EVAP_B,cft_evap_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_INTERC,cft_interc,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_INTERC_B,cft_interc_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_RETURN_FLOW_B,cft_return_flow_b,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputshortvar(CFT_IRRIG_EVENTS,cft_irrig_events,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_NIR,cft_nir,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_TEMP,cft_temp,2*(ncft+NGRASS));
  writeoutputvar(CFT_PREC,cft_prec,2*(ncft+NGRASS));
  writeoutputvar(CFT_SRAD,cft_srad,2*(ncft+NGRASS));
  writeoutputvar(CFT_CONV_LOSS_EVAP,cft_conv_loss_evap,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_CONV_LOSS_DRAIN,cft_conv_loss_drain,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFTFRAC,cftfrac,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_AIRRIG,cft_airrig,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_FPAR,cft_fpar,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(LUC_IMAGE,cft_luc_image,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(CFT_ABOVEGBMC,cft_aboveground_biomass,carbon,2*(ncft+NGRASS));
  writeoutputvaritem(CFT_ABOVEGBMN,cft_aboveground_biomass,nitrogen,2*(ncft+NGRASS));
  /* ATTENTION! Due to allocation rules, this writes away next year's LAImax for trees and grasses */
  if(isopen(output,PFT_LAIMAX))
  {
    outindex(output,PFT_LAIMAX,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand,s,grid[cell].standlist)
          {
            if(stand->type->landusetype == NATURAL)
            { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) 
              {
                if(i == pft->par->id)
                  fvec[count] = (float)pft->par->lai(pft);
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_LAIMAX,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_laimax[p];
      }
      writepft(output,PFT_LAIMAX,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_NROOT))
  {
    outindex(output,PFT_NROOT,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
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
                    fvec[count] = (float)tree->ind.root.nitrogen;
                  }
                  else
                  {
                    grass = pft->data;
                    fvec[count] = (float)grass->ind.root.nitrogen;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_NROOT,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for (cell = 0; cell<config->ngridcell; cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_root[p].nitrogen;
      }
      writepft(output,PFT_NROOT,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_NSAPW))
  {
    outindex(output,PFT_NSAPW,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(!grid[cell].skip)
        {
          fvec[count]=0;
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
                    if(i==(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE))
                      fvec[count]=(float)tree->ind.sapwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.sapwood.nitrogen;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_NSAPW,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);    }
  }
  if(isopen(output,PFT_NHAWO))
  {
    outindex(output,PFT_NHAWO,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip)
        {
          fvec[count]=0;
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
                    if(i==(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE))
                      fvec[count]=(float)tree->ind.heartwood.nitrogen;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.heartwood.nitrogen;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_NHAWO,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_CROOT))
  {
    outindex(output,PFT_CROOT,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft;i++)
    {
      count = 0;
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
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
                    fvec[count] = (float)tree->ind.root.carbon;
                  }
                  else
                  {
                    grass = pft->data;
                    fvec[count] = (float)grass->ind.root.carbon;
                  }
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output,PFT_CROOT,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
    for (i=npft-config->nbiomass;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass-config->nwft);
      for (cell = 0; cell<config->ngridcell; cell++)
      {
        if (!grid[cell].skip)
          fvec[count++] = (float)grid[cell].output.cft_root[p].carbon;
      }
      writepft(output,PFT_CROOT,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_CSAPW))
  {
    outindex(output,PFT_CSAPW,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip)
        {
          fvec[count]=0;
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
                    if(i==(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE))
                      fvec[count]=(float)tree->ind.sapwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.sapwood.carbon;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_CSAPW,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
  if(isopen(output,PFT_CHAWO))
  {
    outindex(output,PFT_CHAWO,config->rank);
    for(i=0;i<npft-config->nbiomass-config->nwft+(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip)
        {
          fvec[count]=0;
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
                    if(i==(npft-config->nbiomass-config->nwft)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE))
                      fvec[count]=(float)tree->ind.heartwood.carbon;
                  }
                }
                break;
              case NATURAL:
                foreachpft(pft,p,&stand->pftlist)
                {
                  if(i==pft->par->id && pft->par->type==TREE)
                  {
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.heartwood.carbon;
                  }
                }
                break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_CHAWO,fvec,npft-config->nbiomass-config->nwft+2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE),year,i,config);
    }
  }
#ifdef DOUBLE_HARVEST
  writeoutputvaritem(PFT_HARVESTC2,pft_harvest2,harvest.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_HARVESTN2,pft_harvest2,harvest.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTC2,pft_harvest2,residual.carbon,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvaritem(PFT_RHARVESTN2,pft_harvest2,residual.nitrogen,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(GROWING_PERIOD2,growing_period2,2*(ncft+NGRASS));
  writeoutputvar(CFT_PET2,cft_pet2,2*(ncft+NGRASS));
  writeoutputvar(CFT_TRANSP2,cft_transp2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_EVAP2,cft_evap2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_INTERC2,cft_interc2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_NIR2,cft_nir2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_TEMP2,cft_temp2,2*(ncft+NGRASS));
  writeoutputvar(CFT_PREC2,cft_prec2,2*(ncft+NGRASS));
  writeoutputvar(CFT_SRAD2,cft_srad2,2*(ncft+NGRASS));
  writeoutputvar(CFTFRAC2,cftfrac2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputvar(CFT_AIRRIG2,cft_airrig2,2*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE));
  writeoutputshortvar(SDATE2,sdate2,2*ncft);
  writeoutputshortvar(HDATE2,hdate2,2*ncft);
  writeoutputshortvar(SYEAR,syear,2*ncft);
  writeoutputshortvar(SYEAR2,syear2,2*ncft);
  writeoutputvaritem(CFT_ABOVEGBMC2,cft_aboveground_biomass2,carbon,2*(ncft+NGRASS));
  writeoutputvaritem(CFT_ABOVEGBMN2,cft_aboveground_biomass2,nitrogen,2*(ncft+NGRASS));
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
