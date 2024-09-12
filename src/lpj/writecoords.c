/**************************************************************************************/
/**                                                                                \n**/
/**                   w  r  i  t  e  c  o  o  r  d  s  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes grid coordinates into binary file or writes                \n**/
/**     cell index in NetCDF file                                                  \n**/
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

typedef struct
{
  double lon; /**< longitude */
  double lat; /**< latitude */
} Doublecoord;

int writecoords(Outputfile *output,  /**< output struct */
                int index,           /**< index in output file array */
                const Cell grid[],   /**< LPJ grid */
                const Config *config /**< LPJ configuration */
               )                     /** \return number of coordinates written */
{
#ifdef USE_MPI
  Intcoord *dst=NULL;
  Floatcoord *fdst=NULL;
  Doublecoord *ddst=NULL;
  MPI_Datatype type;
#endif
  Bool rc;
  int cell,count=0;
  int *cellid;
  Intcoord *vec=NULL;
  Floatcoord *fvec=NULL;
  Doublecoord *dvec=NULL;
  if(output->files[index].isopen && output->files[index].fmt==CDF)
  {
    if(isroot(*config))
    {
      cellid=newvec(int,config->total);
      if(cellid==NULL)
      {
        printallocerr("cellid");
        rc=TRUE;
      }
      else
      {
        for(cell=0;cell<config->total;cell++)
          cellid[cell]=cell;
        rc=write_int_netcdf(&output->files[index].fp.cdf,cellid,NO_TIME,config->total);
        count=config->total;
        free(cellid);
      }
      if(config->flush_output)
        flush_netcdf(&output->files[index].fp.cdf);
    }
    else
      rc=FALSE;
  }
  if(output->files[index].fmt!=CDF || output->files[index].issocket)
  {
    switch(config->grid_type)
    {
      case LPJ_FLOAT:
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
        break;
      case LPJ_DOUBLE:
        dvec=newvec(Doublecoord,config->count);
        if(dvec==NULL)
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
              dvec[count].lon=grid[cell].coord.lon;
              dvec[count].lat=grid[cell].coord.lat;
              count++;
            }
          rc=FALSE;
        }
        break;
      case LPJ_SHORT:
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
        break;
      default:
       fprintf(stderr,"ERROR204: Unsupported datatype in writecoords().\n");
       rc=TRUE;
    }
  }
  if(iserror(rc,config))
    return 0;
#ifdef USE_MPI
  if(output->files[index].issocket || output->files[index].fmt!=CDF)
  {
    switch(config->grid_type)
    {
      case LPJ_FLOAT:
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
          if(output->files[index].isopen)
            switch(output->files[index].fmt)
            {
              case RAW: case CLM:
                if(fwrite(fdst,sizeof(Floatcoord),config->total,output->files[index].fp.file)!=config->total)
                  fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
                if(config->flush_output)
                  fflush(output->files[index].fp.file);
                break;
              case TXT:
                for(cell=0;cell<config->total-1;cell++)
                  fprintf(output->files[index].fp.file,"%g%c%g%c",
                          fdst[cell].lon,config->csv_delimit,fdst[cell].lat,config->csv_delimit);
                fprintf(output->files[index].fp.file,"%g%c%g\n",
                        fdst[config->total-1].lon,config->csv_delimit,fdst[config->total-1].lat);
                if(config->flush_output)
                  fflush(output->files[index].fp.file);
                break;
            }
          if(output->files[index].issocket)
          {
            send_token_coupler(PUT_DATA,index,config);
            writefloat_socket(config->socket,fdst,config->total*2);
          }
          free(fdst);
        }
        free(fvec);
        break;
      case LPJ_DOUBLE:
        MPI_Type_contiguous(2,MPI_DOUBLE,&type);
        MPI_Type_commit(&type);
        if(isroot(*config))
        {
          ddst=newvec(Doublecoord,config->total);
          if(ddst==NULL)
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
          free(dvec);
          return 0;
        }
        MPI_Gatherv(dvec,config->count,type,ddst,output->counts,output->offsets,
                    type,0,config->comm);
        MPI_Type_free(&type);
        if(isroot(*config))
        {
          if(output->files[index].isopen)
            switch(output->files[index].fmt)
            {
              case RAW: case CLM:
                if(fwrite(ddst,sizeof(Doublecoord),config->total,output->files[index].fp.file)!=config->total)
                  fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
                if(config->flush_output)
                  fflush(output->files[index].fp.file);
                break;
              case TXT:
                for(cell=0;cell<config->total-1;cell++)
                  fprintf(output->files[index].fp.file,"%g%c%g%c",
                          ddst[cell].lon,config->csv_delimit,ddst[cell].lat,config->csv_delimit);
                fprintf(output->files[index].fp.file,"%g%c%g\n",
                        ddst[config->total-1].lon,config->csv_delimit,ddst[config->total-1].lat);
                if(config->flush_output)
                  fflush(output->files[index].fp.file);
                break;
            }
          if(output->files[index].issocket)
          {
            send_token_coupler(PUT_DATA,index,config);
            writedouble_socket(config->socket,ddst,config->total*2);
          }
          free(ddst);
        }
        free(dvec);
        break;
      case LPJ_SHORT:
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
          if(output->files[index].isopen)
            switch(output->files[index].fmt)
            {
              case RAW: case CLM:
                if(fwrite(dst,sizeof(Intcoord),config->total,output->files[index].fp.file)!=config->total)
                  fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
                if(config->flush_output)
                  fflush(output->files[index].fp.file);
                break;
              case TXT:
                for(cell=0;cell<config->total-1;cell++)
                  fprintf(output->files[index].fp.file,"%g%c%g%c",
                          dst[cell].lon*0.01,config->csv_delimit,dst[cell].lat*0.01,config->csv_delimit);
                fprintf(output->files[index].fp.file,"%g%c%g\n",
                        dst[config->total-1].lon*0.01,config->csv_delimit,dst[config->total-1].lat*0.01);
                if(config->flush_output)
                  fflush(output->files[index].fp.file);
                break;
            }
          if(output->files[index].issocket)
          {
            send_token_coupler(PUT_DATA,index,config);
            writeshort_socket(config->socket,dst,config->total*2);
          }
          free(dst);
        }
        free(vec);
        break;
      default:
       if(isroot(*config))
         fprintf(stderr,"ERROR204: Unsupported datatype in writecoords().\n");
    }
  }
#else
  switch(config->grid_type)
  {
    case LPJ_FLOAT:
      if(output->files[index].isopen)
        switch(output->files[index].fmt)
        {
          case RAW: case CLM:
            if(fwrite(fvec,sizeof(Floatcoord),count,output->files[index].fp.file)!=count)
              fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
            if(config->flush_output)
              fflush(output->files[index].fp.file);
            break;
          case TXT:
            for(cell=0;cell<count-1;cell++)
              fprintf(output->files[index].fp.file,"%g%c%g%c",
                      fvec[cell].lon,config->csv_delimit,fvec[cell].lat,config->csv_delimit);
            fprintf(output->files[index].fp.file,"%g%c%g\n",
                    fvec[count-1].lon,config->csv_delimit,fvec[count-1].lat);
            if(config->flush_output)
              fflush(output->files[index].fp.file);
            break;
      }
      if(output->files[index].issocket)
      {
        send_token_coupler(PUT_DATA,output->files[index].id,config);
        writefloat_socket(config->socket,fvec,count*2);
      }
      free(fvec);
      break;
    case LPJ_DOUBLE:
      if(output->files[index].isopen)
        switch(output->files[index].fmt)
        {
          case RAW: case CLM:
            if(fwrite(dvec,sizeof(Doublecoord),count,output->files[index].fp.file)!=count)
              fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
            if(config->flush_output)
              fflush(output->files[index].fp.file);
            break;
          case TXT:
            for(cell=0;cell<count-1;cell++)
              fprintf(output->files[index].fp.file,"%g%c%g%c",
                      dvec[cell].lon,config->csv_delimit,dvec[cell].lat,config->csv_delimit);
            fprintf(output->files[index].fp.file,"%g%c%g\n",
                    dvec[count-1].lon,config->csv_delimit,dvec[count-1].lat);
            if(config->flush_output)
              fflush(output->files[index].fp.file);
            break;
        }
      if(output->files[index].issocket)
      {
        send_token_coupler(PUT_DATA,output->files[index].id,config);
        writedouble_socket(config->socket,dvec,count*2);
      }
      free(dvec);
      break;
    case LPJ_SHORT:
      if(output->files[index].isopen)
        switch(output->files[index].fmt)
        {
          case RAW: case CLM:
            if(fwrite(vec,sizeof(Intcoord),count,output->files[index].fp.file)!=count)
              fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
            if(config->flush_output)
              fflush(output->files[index].fp.file);
            break;
          case TXT:
            for(cell=0;cell<count-1;cell++)
              fprintf(output->files[index].fp.file,"%g%c%g%c",
                      vec[cell].lon*0.01,config->csv_delimit,vec[cell].lat*0.01,config->csv_delimit);
            fprintf(output->files[index].fp.file,"%g%c%g\n",
                    vec[count-1].lon*0.01,config->csv_delimit,vec[count-1].lat*0.01);
            if(config->flush_output)
              fflush(output->files[index].fp.file);
            break;
        }
      if(output->files[index].issocket)
      {
        send_token_coupler(PUT_DATA,index,config);
        writeshort_socket(config->socket,vec,count*2);
      }
      free(vec);
      break;
    default:
      fprintf(stderr,"ERROR204: Unsupported datatype in writecoords().\n");
  }
#endif
  return count;
} /* of 'writecoords' */
