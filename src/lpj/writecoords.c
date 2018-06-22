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

int writecoords(Outputfile *output,  /**< output struct */
                int index,           /**< index in output file array */
                const Cell grid[],   /**< LPJ grid */
                const Config *config /**< LPJ configuration */
               )                     /** \return number of coordinates written */
{
#ifdef USE_MPI
  Intcoord *dst=NULL;
  MPI_Datatype type;
  MPI_Status status;
  int *offsets,*counts;
#endif
  Bool rc;
  int cell,count;
  short *soilcode;
  Intcoord *vec;
  if((output->method==LPJ_GATHER || output->method==LPJ_FILES) &&
     output->files[index].fmt==CDF)
  {
    soilcode=newvec(short,config->ngridcell);
    if(soilcode==NULL)
    {
      printallocerr("soilcode");
      rc=TRUE;
    }
    else
    {
      count=0;
      for(cell=0;cell<config->ngridcell;cell++)
        if(grid[cell].skip)
          soilcode[cell]=0;
        else if(isempty(grid[cell].standlist))
          soilcode[cell]=MISSING_VALUE_SHORT;
        else
          soilcode[cell]=(short)(getstand(grid[cell].standlist,0)->soil.par->type+1);
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
  if(iserror(rc,config))
    return 0;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,2*config->offset,
                        vec,2*count,MPI_SHORT,&status);
      free(vec);
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
          free(soilcode);
          free(counts);
          free(offsets);
          return 0;
        }
        getcounts(counts,offsets,config->nall,1,config->ntask);
        mpi_write_netcdf(&output->files[index].fp.cdf,soilcode,MPI_SHORT,
                         config->nall,NO_TIME,
                         counts,offsets,config->rank,config->comm);
        free(counts);
        free(offsets);
        free(soilcode);
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
                  fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
                break;
              case TXT:
                for(cell=0;cell<config->total-1;cell++)
                  fprintf(output->files[index].fp.file,"%g %g ",
                          dst[cell].lon*0.01,dst[cell].lat*0.01);
                fprintf(output->files[index].fp.file,"%g %g\n",
                        dst[config->total-1].lon*0.01,dst[config->total-1].lat*0.01);
                break;
            }
          free(dst);
        }
        free(vec);
      }
      break;
  }
#else
  if(output->method==LPJ_FILES)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(vec,sizeof(Intcoord),count,output->files[index].fp.file)!=count)
          fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
        free(vec);
        break;
      case TXT:
        for(cell=0;cell<count-1;cell++)
          fprintf(output->files[index].fp.file,"%g %g ",
                  vec[cell].lon*0.01,vec[cell].lat*0.01);
        fprintf(output->files[index].fp.file,"%g %g\n",
                vec[count-1].lon*0.01,vec[count-1].lat*0.01);
        free(vec);
        break;
      case CDF:
        write_short_netcdf(&output->files[index].fp.cdf,soilcode,NO_TIME,config->nall);
        free(soilcode);
        break;
    }
  else
  {
    writeshort_socket(output->socket,vec,count*2);
    free(vec);
  }
#endif
  return count;
} /* of 'writecoords' */
