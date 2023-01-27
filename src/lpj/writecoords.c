/**************************************************************************************/
/**                                                                                \n**/
/**                   w  r  i  t  e  c  o  o  r  d  s  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes grid coordinates into binary file or writes                \n**/
/**     soil code in NetCDF file                                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

typedef struct
{
  float lon; /**< longitude */
  float lat; /**< latitude */
} Floatcoord;

int writecoords(Outputfile *output,  /**< output struct */
                int index,           /**< index in output file array */
                const Cell grid[],   /**< LPJ grid */
                const Config *config /**< LPJ configuration */
               )                     /** \return number of coordinates written */
{
#ifdef USE_MPI
  Intcoord *dst=NULL;
  Floatcoord *fdst=NULL;
  MPI_Datatype type;
  MPI_Status status;
  int *offsets,*counts;
#endif
  Bool rc;
  int cell,count;
  int *cellid;
  Intcoord *vec;
  Floatcoord *fvec;
  if((output->method==LPJ_GATHER || output->method==LPJ_FILES) &&
     output->files[index].fmt==CDF)
  {
    cellid=newvec(int,config->ngridcell);
    if(cellid==NULL)
    {
      printallocerr("cellid");
      rc=TRUE;
    }
    else
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(grid[cell].skip)
          cellid[cell]=-1;
        else
          cellid[cell]=cell+config->startgrid;
      rc=FALSE;
    }
  }
  else
  {
    if(config->float_grid)
    {
      fvec=newvec(Floatcoord,config->count);
      if(fvec==NULL)
      {
        printallocerr("fvec");
        rc=TRUE;
      }
      else
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
          {
            fvec[count].lon=(float)grid[cell].coord.lon;
            fvec[count].lat=(float)grid[cell].coord.lat;
            count++;
          }
        rc=FALSE;
      }
    }
    else
    {
      vec=newvec(Intcoord,config->count);
      if(vec==NULL)
      {
        printallocerr("vec");
        rc=TRUE;
      }
      else
      {
        count=0;
        for(cell=0;cell<config->ngridcell;cell++)
          if(!grid[cell].skip)
          {
            vec[count].lon=(short)(grid[cell].coord.lon*100);
            vec[count].lat=(short)(grid[cell].coord.lat*100);
            count++;
          }
        rc=FALSE;
      }
    }
  }
  if(iserror(rc,config))
    return 0;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      if(config->float_grid)
      {
        MPI_File_write_at(output->files[index].fp.mpi_file,2*config->offset,
                          fvec,2*count,MPI_FLOAT,&status);
        free(fvec);
      }
      else
      {
        MPI_File_write_at(output->files[index].fp.mpi_file,2*config->offset,
                          vec,2*count,MPI_SHORT,&status);
        free(vec);
      }
      break;
    case LPJ_GATHER: case LPJ_SOCKET:
      if(output->method==LPJ_GATHER && output->files[index].fmt==CDF)
      {
        rc=FALSE;
        counts=newvec(int,config->ntask);
        if(counts==NULL)
        {
          printallocerr("counts");
          rc=TRUE;
        }
        offsets=newvec(int,config->ntask);
        if(offsets==NULL)
        {
          printallocerr("offsets");
          rc=TRUE;
        }
        if(iserror(rc,config))
        {
          free(cellid);
          free(counts);
          free(offsets);
          return 0;
        }
        getcounts(counts,offsets,config->nall,1,config->ntask);
        mpi_write_netcdf(&output->files[index].fp.cdf,cellid,MPI_INT,
                         config->nall,NO_TIME,
                         counts,offsets,config->rank,config->comm);
        free(counts);
        free(offsets);
        free(cellid);
      }
      else
      {
        if(config->float_grid)
        {
          MPI_Type_contiguous(2,MPI_FLOAT,&type);
          MPI_Type_commit(&type);
          if(isroot(*config))
          {
            fdst=newvec(Floatcoord,config->total);
            if(fdst==NULL)
            {
              printallocerr("fdst");
              rc=TRUE;
            }
            else
              rc=FALSE;
          }
          MPI_Bcast(&rc,1,MPI_INT,0,config->comm);
          if(rc)
          {
            free(fvec);
            return 0;
          }
          MPI_Gatherv(fvec,config->count,type,fdst,output->counts,output->offsets,
                      type,0,config->comm);
          MPI_Type_free(&type);
          if(isroot(*config))
          {
            if(output->method==LPJ_SOCKET)
              writefloat_socket(output->socket,fdst,config->total*2);
            else
              switch(output->files[index].fmt)
              {
                case RAW: case CLM:
                  if(fwrite(fdst,sizeof(Floatcoord),config->total,output->files[index].fp.file)!=config->total)
                    fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
                  break;
                case TXT:
                  for(cell=0;cell<config->total-1;cell++)
                    fprintf(output->files[index].fp.file,"%g%c%g%c",
                            fdst[cell].lon,config->csv_delimit,fdst[cell].lat,config->csv_delimit);
                  fprintf(output->files[index].fp.file,"%g%c%g\n",
                          fdst[config->total-1].lon,config->csv_delimit,fdst[config->total-1].lat);
                  break;
              }
            free(fdst);
          }
          free(fvec);
        }
        else
        {
          MPI_Type_contiguous(2,MPI_SHORT,&type);
          MPI_Type_commit(&type);
          if(isroot(*config))
          {
            dst=newvec(Intcoord,config->total);
            if(dst==NULL)
            {
              printallocerr("dst");
              rc=TRUE;
            }
            else
              rc=FALSE;
          }
          MPI_Bcast(&rc,1,MPI_INT,0,config->comm);
          if(rc)
          {
            free(vec);
            return 0;
          }
          MPI_Gatherv(vec,config->count,type,dst,output->counts,output->offsets,
                      type,0,config->comm);
          MPI_Type_free(&type);
          if(isroot(*config))
          {
            if(output->method==LPJ_SOCKET)
              writeshort_socket(output->socket,dst,config->total*2);
            else
              switch(output->files[index].fmt)
              {
                case RAW: case CLM:
                  if(fwrite(dst,sizeof(Intcoord),config->total,output->files[index].fp.file)!=config->total)
                    fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
                  break;
                case TXT:
                  for(cell=0;cell<config->total-1;cell++)
                    fprintf(output->files[index].fp.file,"%g%c%g%c",
                            dst[cell].lon*0.01,config->csv_delimit,dst[cell].lat*0.01,config->csv_delimit);
                  fprintf(output->files[index].fp.file,"%g%c%g\n",
                          dst[config->total-1].lon*0.01,config->csv_delimit,dst[config->total-1].lat*0.01);
                  break;
              }
            free(dst);
          }
          free(vec);
        }
      }
      break;
  }
#else
  if(output->method==LPJ_FILES)
  {
    if(config->float_grid)
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          if(fwrite(fvec,sizeof(Floatcoord),count,output->files[index].fp.file)!=count)
            fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
          free(fvec);
          break;
        case TXT:
          for(cell=0;cell<count-1;cell++)
            fprintf(output->files[index].fp.file,"%g%c%g%c",
                    fvec[cell].lon,config->csv_delimit,fvec[cell].lat*0.01,config->csv_delimit);
          fprintf(output->files[index].fp.file,"%g%c%g\n",
                  fvec[count-1].lon,config->csv_delimit,fvec[count-1].lat);
          free(fvec);
          break;
        case CDF:
          write_int_netcdf(&output->files[index].fp.cdf,cellid,NO_TIME,config->nall);
          free(cellid);
          break;
      }
    else
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          if(fwrite(vec,sizeof(Intcoord),count,output->files[index].fp.file)!=count)
            fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
          free(vec);
          break;
        case TXT:
          for(cell=0;cell<count-1;cell++)
            fprintf(output->files[index].fp.file,"%g%c%g%c",
                    vec[cell].lon*0.01,config->csv_delimit,vec[cell].lat*0.01,config->csv_delimit);
          fprintf(output->files[index].fp.file,"%g%c%g\n",
                  vec[count-1].lon*0.01,config->csv_delimit,vec[count-1].lat*0.01);
          free(vec);
          break;
        case CDF:
          write_int_netcdf(&output->files[index].fp.cdf,cellid,NO_TIME,config->nall);
          free(cellid);
          break;
      }
  }
  else
  {
    if(config->float_grid)
    {
      writefloat_socket(output->socket,fvec,count*2);
      free(fvec);
    }
    else
    {
      writeshort_socket(output->socket,vec,count*2);
      free(vec);
    }
  }
#endif
  return count;
} /* of 'writecoords' */
