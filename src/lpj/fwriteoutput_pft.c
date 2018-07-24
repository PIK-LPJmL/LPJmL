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
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"
#include "grass.h"
#include "tree.h"

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
  const Pft *pft;
  Pfttree *tree;
  Pftgrass *grass;
  Irrigation *data;
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
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_npp[i];
      writepft(output,PFT_NPP,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_NUPTAKE].isopen)
  {
    outindex(output,PFT_NUPTAKE,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_nuptake[i];
      writepft(output,PFT_NUPTAKE,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_NDEMAND].isopen)
  {
    outindex(output,PFT_NDEMAND,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_ndemand[i];
      writepft(output,PFT_NDEMAND,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_NLIMIT].isopen)
  {
    outindex(output,PFT_NLIMIT,config->rank);
    for (i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for (cell = 0; cell < config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist) {
            if (stand->type->landusetype == NATURAL) { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) {
                if (i == pft->par->id) {
                  fvec[count] = (float)pft->nlimit / NDAYYEAR;
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output, PFT_NLIMIT, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_nlimit[p];
        }
      }
      writepft(output, PFT_NLIMIT, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
  if(output->files[PFT_VEGC].isopen)
  {
    outindex(output,PFT_VEGC,config->rank);
    for (i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for (cell = 0; cell < config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist) {
            if (stand->type->landusetype == NATURAL) { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) {
                if (i == pft->par->id) {
                  fvec[count] = (float)vegc_sum(pft);
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output, PFT_VEGC, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_veg[p].carbon;
        }
      }
      writepft(output, PFT_VEGC, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
  if(output->files[PFT_VEGN].isopen)
  {
    outindex(output,PFT_VEGN,config->rank);
    for (i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for (cell = 0; cell < config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist) {
            if (stand->type->landusetype == NATURAL) { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) {
                if (i == pft->par->id) {
                  fvec[count] = (float)vegn_sum(pft);
                }
              }
            }
          } /* stand */
          count++;
        } /* skip */
      } /* for cell */
      writepft(output, PFT_VEGN, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_veg[p].nitrogen;
        }
      }
      writepft(output, PFT_VEGN, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
  if(output->files[PFT_CLEAF].isopen)
  {
    outindex(output,PFT_CLEAF,config->rank);
    for (i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for (cell = 0; cell < config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist) {
            if (stand->type->landusetype == NATURAL) { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) {
                if (i == pft->par->id) {
                  if (pft->par->type == TREE) {
                    tree = pft->data;
                    fvec[count] = (float)tree->ind.leaf.carbon;
                  }
                  else {
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
      writepft(output, PFT_CLEAF, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_leaf[p].carbon;
        }
      }
      writepft(output, PFT_CLEAF, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
  if(output->files[PFT_NLEAF].isopen)
  {
    outindex(output,PFT_NLEAF,config->rank);
    for (i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for (cell = 0; cell < config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist) {
            if (stand->type->landusetype == NATURAL) { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) {
                if (i == pft->par->id) {
                  if (pft->par->type == TREE) {
                    tree = pft->data;
                    fvec[count] = (float)tree->ind.leaf.nitrogen;
                  }
                  else {
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
      writepft(output, PFT_NLEAF, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_leaf[p].nitrogen;
        }
      }
      writepft(output, PFT_NLEAF, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
  if(output->files[PFT_GCGP].isopen)
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
  if(output->files[PFT_HARVESTC].isopen)
  {
    outindex(output,PFT_HARVESTC,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].harvest.carbon;
      writepft(output,PFT_HARVESTC,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_RHARVESTC].isopen)
  {
    outindex(output,PFT_RHARVESTC,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].residual.carbon;
      writepft(output,PFT_RHARVESTC,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_HARVESTN].isopen)
  {
    outindex(output,PFT_HARVESTN,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].harvest.nitrogen;
      writepft(output,PFT_HARVESTN,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_RHARVESTN].isopen)
  {
    outindex(output,PFT_RHARVESTN,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest[i].residual.nitrogen;
      writepft(output,PFT_RHARVESTN,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_CONSUMP_WATER_G].isopen)
  {
    outindex(output,CFT_CONSUMP_WATER_G,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_consump_water_g[i];
      writepft(output,CFT_CONSUMP_WATER_G,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_CONSUMP_WATER_B].isopen)
  {
    outindex(output,CFT_CONSUMP_WATER_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_consump_water_b[i];
      writepft(output,CFT_CONSUMP_WATER_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
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
                fvec[count]+=(float)(stand->soil.litter.bg[p].carbon*stand->frac);
            fvec[count]+=(float)((stand->soil.pool[i].slow.carbon+stand->soil.pool[i].fast.carbon)*stand->frac);
          }   
          count++;
        }
      }
      writepft(output,SOILC_LAYER,fvec,BOTTOMLAYER,year,i,config);
    }
  }
  if(output->files[SOILN_LAYER].isopen)
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
  if (output->files[SOILNO3_LAYER].isopen)
  {
    outindex(output, SOILNO3_LAYER, config->rank);
    forrootsoillayer(i)
    {
      count = 0;
      for (cell = 0; cell<config->ngridcell; cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist)
          {
            fvec[count] += (float)(stand->soil.NO3[i]*stand->frac);
          }
          count++;
        }
      }
      writepft(output, SOILNO3_LAYER, fvec, BOTTOMLAYER, year, i, config);
    }
  }
  if (output->files[SOILNH4_LAYER].isopen)
  {
    outindex(output, SOILNH4_LAYER, config->rank);
    forrootsoillayer(i)
    {
      count = 0;
      for (cell = 0; cell<config->ngridcell; cell++)
      {
        if (!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist)
          {
            fvec[count] += (float)(stand->soil.NH4[i]*stand->frac);
          }
          count++;
        }
      }
      writepft(output, SOILNH4_LAYER, fvec, BOTTOMLAYER, year, i, config);
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
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_transp[i];
      writepft(output,CFT_TRANSP,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_TRANSP_B].isopen)
  {
    outindex(output,CFT_TRANSP_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_transp_b[i];
      writepft(output,CFT_TRANSP_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_EVAP].isopen)
  {
    outindex(output,CFT_EVAP,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_evap[i];
        writepft(output,CFT_EVAP,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_EVAP_B].isopen)
  {
    outindex(output,CFT_EVAP_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_evap_b[i];
        writepft(output,CFT_EVAP_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_INTERC].isopen)
  {
    outindex(output,CFT_INTERC,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_interc[i];
      writepft(output,CFT_INTERC,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_INTERC_B].isopen)
  {
    outindex(output,CFT_INTERC_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_interc_b[i];
      writepft(output,CFT_INTERC_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_RETURN_FLOW_B].isopen)
  {
    outindex(output,CFT_RETURN_FLOW_B,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_return_flow_b[i];
      writepft(output,CFT_RETURN_FLOW_B,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_IRRIG_EVENTS].isopen)
  {
    outindex(output,CFT_IRRIG_EVENTS,config->rank);
    vec=newvec(short,config->count);
    check(vec);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(short)grid[cell].output.cft_irrig_events[i];
      writeshortpft(output,CFT_IRRIG_EVENTS,vec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
    free(vec);
  }
  if(output->files[CFT_NIR].isopen)
  {
    outindex(output,CFT_NIR,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
            fvec[count++]=(float)grid[cell].output.cft_nir[i];
        writepft(output,CFT_NIR,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
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
  if(output->files[CFT_ABOVEGBMC].isopen)
  {
    outindex(output,CFT_ABOVEGBMC,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_aboveground_biomass[i].carbon;
      writepft(output,CFT_ABOVEGBMC,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_ABOVEGBMN].isopen)
  {
    outindex(output,CFT_ABOVEGBMN,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_aboveground_biomass[i].nitrogen;
      writepft(output,CFT_ABOVEGBMN,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_CONV_LOSS_EVAP].isopen)
  {
    outindex(output,CFT_CONV_LOSS_EVAP,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
            fvec[count++]=(float)grid[cell].output.cft_conv_loss_evap[i];
        writepft(output,CFT_CONV_LOSS_EVAP,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
      }
    }
  if(output->files[CFT_CONV_LOSS_DRAIN].isopen)
  {
    outindex(output,CFT_CONV_LOSS_DRAIN,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
            fvec[count++]=(float)grid[cell].output.cft_conv_loss_drain[i];
        writepft(output,CFT_CONV_LOSS_DRAIN,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
      }
    }
  if(output->files[CFTFRAC].isopen)
  {
    outindex(output,CFTFRAC,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cftfrac[i];
      writepft(output,CFTFRAC,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_AIRRIG].isopen)
  {
    outindex(output,CFT_AIRRIG,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_airrig[i];
      writepft(output,CFT_AIRRIG,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_FPAR].isopen)
  {
    outindex(output,CFT_FPAR,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_fpar[i];
      writepft(output,CFT_FPAR,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[LUC_IMAGE].isopen)
  {
    outindex(output,LUC_IMAGE,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_luc_image[i];
      writepft(output,LUC_IMAGE,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  /* ATTENTION! Due to allocation rules, this writes away next year's LAImax for trees and grasses */
  if(output->files[PFT_LAIMAX].isopen)
  {
    outindex(output,PFT_LAIMAX,config->rank);
    for(i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for(cell = 0; cell < config->ngridcell; cell++)
      {
        if(!grid[cell].skip)
        {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist)
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
      writepft(output, PFT_LAIMAX, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_laimax[p];
        }
      }
      writepft(output, PFT_LAIMAX, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
  if(output->files[PFT_NROOT].isopen)
  {
    outindex(output,PFT_NROOT,config->rank);
    for (i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for (cell = 0; cell < config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist) {
            if (stand->type->landusetype == NATURAL) { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) {
                if (i == pft->par->id) {
                  if (pft->par->type == TREE) {
                    tree = pft->data;
                    fvec[count] = (float)tree->ind.root.nitrogen;
                  }
                  else {
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
      writepft(output, PFT_NROOT, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_root[p].nitrogen;
        }
      }
      writepft(output, PFT_NROOT, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
  if(output->files[PFT_NSAPW].isopen)
  {
    outindex(output,PFT_NSAPW,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip){
          fvec[count]=0;
          foreachstand(stand,s,grid[cell].standlist){
            switch(stand->type->landusetype){ /* ignoring setaside stands here */
            case BIOMASS_TREE:
              data=stand->data;
              foreachpft(pft,p,&stand->pftlist){
                if(pft->par->type==TREE){
                  tree=pft->data;
                  if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                    fvec[count]=(float)tree->ind.sapwood.nitrogen;
                }
              }
              break;
            case NATURAL:
              foreachpft(pft,p,&stand->pftlist){
                if(i==pft->par->id){
                  if(pft->par->type==TREE){
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.sapwood.nitrogen;
                  }
                }
              }
              break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_NSAPW,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);    }
  }
  if(output->files[PFT_NHAWO].isopen)
  {
    outindex(output,PFT_NHAWO,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip){
          fvec[count]=0;
          foreachstand(stand,s,grid[cell].standlist){
            switch(stand->type->landusetype){ /* ignoring setaside stands here */
            case BIOMASS_TREE:
              data=stand->data;
              foreachpft(pft,p,&stand->pftlist){
                if(pft->par->type==TREE){
                  tree=pft->data;
                  if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                    fvec[count]=(float)tree->ind.heartwood.nitrogen;
                }
              }
              break;
            case NATURAL:
              foreachpft(pft,p,&stand->pftlist){
                if(i==pft->par->id){
                  if(pft->par->type==TREE){
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.heartwood.nitrogen;
                  }
                }
              }
              break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_NHAWO,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_CROOT].isopen)
  {
    outindex(output,PFT_CROOT,config->rank);
    for (i = 0; i < (npft - config->nbiomass); i++)
    {
      count = 0;
      for (cell = 0; cell < config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count] = 0;
          foreachstand(stand, s, grid[cell].standlist) {
            if (stand->type->landusetype == NATURAL) { /* ignoring setaside stands here */
              foreachpft(pft, p, &stand->pftlist) {
                if (i == pft->par->id) {
                  if (pft->par->type == TREE) {
                    tree = pft->data;
                    fvec[count] = (float)tree->ind.root.carbon;
                  }
                  else {
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
      writepft(output, PFT_CROOT, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
    for (i = (npft - config->nbiomass); i < (npft - config->nbiomass) + (ncft + NGRASS + NBIOMASSTYPE) * 2; i++)
    {
      count = 0;
      p = i - (npft - config->nbiomass);
      for (cell = 0; cell<config->ngridcell; cell++) {
        if (!grid[cell].skip) {
          fvec[count++] = (float)grid[cell].output.cft_root[p].carbon;
        }
      }
      writepft(output, PFT_CROOT, fvec, (npft - config->nbiomass) + 2 * (ncft + NGRASS + NBIOMASSTYPE), year, i, config);
    }
  }
if(output->files[PFT_CSAPW].isopen)
  {
    outindex(output,PFT_CSAPW,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip){
          fvec[count]=0;
          foreachstand(stand,s,grid[cell].standlist){
            switch(stand->type->landusetype){ /* ignoring setaside stands here */
            case BIOMASS_TREE:
              data=stand->data;
              foreachpft(pft,p,&stand->pftlist){
                if(pft->par->type==TREE){
                  tree=pft->data;
                  if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                    fvec[count]=(float)tree->ind.sapwood.carbon;
                }
              }
              break;
            case NATURAL:
              foreachpft(pft,p,&stand->pftlist){
                if(i==pft->par->id){
                  if(pft->par->type==TREE){
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.sapwood.carbon;
                  }
                }
              }
              break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_CSAPW,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);    }
  }
  if(output->files[PFT_CHAWO].isopen)
  {
    outindex(output,PFT_CHAWO,config->rank);
    for(i=0;i<(npft-config->nbiomass)+(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++){
        if(!grid[cell].skip){
          fvec[count]=0;
          foreachstand(stand,s,grid[cell].standlist){
            switch(stand->type->landusetype){ /* ignoring setaside stands here */
            case BIOMASS_TREE:
              data=stand->data;
              foreachpft(pft,p,&stand->pftlist){
                if(pft->par->type==TREE){
                  tree=pft->data;
                  if(i==(npft-config->nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE))
                    fvec[count]=(float)tree->ind.heartwood.carbon;
                }
              }
              break;
            case NATURAL:
              foreachpft(pft,p,&stand->pftlist){
                if(i==pft->par->id){
                  if(pft->par->type==TREE){
                    tree=pft->data;
                    fvec[count]=(float)tree->ind.heartwood.carbon;
                  }
                }
              }
              break;
            } /* switch */
          }
          count++;
        }
      }
      writepft(output,PFT_CHAWO,fvec,(npft-config->nbiomass)+2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
#ifdef DOUBLE_HARVEST
  if(output->files[PFT_HARVESTC2].isopen)
  {
    outindex(output,PFT_HARVESTC2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].harvest.carbon;
      writepft(output,PFT_HARVESTC2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_HARVESTN2].isopen)
  {
    outindex(output,PFT_HARVESTN2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].harvest.nitrogen;
      writepft(output,PFT_HARVESTN2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_RHARVESTC2].isopen)
  {
    outindex(output,PFT_RHARVESTC2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].residual.carbon;
      writepft(output,PFT_RHARVESTC2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[PFT_RHARVESTN2].isopen)
  {
    outindex(output,PFT_RHARVESTN2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.pft_harvest2[i].residual.nitrogen;
      writepft(output,PFT_RHARVESTN2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
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
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_transp2[i];
      writepft(output,CFT_TRANSP2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_EVAP2].isopen)
  {
    outindex(output,CFT_EVAP2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_evap2[i];
        writepft(output,CFT_EVAP2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_INTERC2].isopen)
  {
    outindex(output,CFT_INTERC2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_interc2[i];
      writepft(output,CFT_INTERC2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_NIR2].isopen)
  {
    outindex(output,CFT_NIR2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_nir2[i];
      writepft(output,CFT_NIR2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
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
  if(output->files[CFT_ABOVEGBMC2].isopen)
  {
    outindex(output,CFT_ABOVEGBMC2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_aboveground_biomass2[i].carbon;
      writepft(output,CFT_ABOVEGBMC2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFT_ABOVEGBMN2].isopen)
  {
    outindex(output,CFT_ABOVEGBMN2,config->rank);
    for(i=0;i<(ncft+NGRASS)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_aboveground_biomass2[i].nitrogen;
      writepft(output,CFT_ABOVEGBMN2,fvec,2*(ncft+NGRASS),year,i,config);
    }
  }
  if(output->files[CFTFRAC2].isopen)
  {
    outindex(output,CFTFRAC2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cftfrac2[i];
      writepft(output,CFTFRAC2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
    }
  }
  if(output->files[CFT_AIRRIG2].isopen)
  {
    outindex(output,CFT_AIRRIG2,config->rank);
    for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          fvec[count++]=(float)grid[cell].output.cft_airrig2[i];
      writepft(output,CFT_AIRRIG2,fvec,2*(ncft+NGRASS+NBIOMASSTYPE),year,i,config);
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
