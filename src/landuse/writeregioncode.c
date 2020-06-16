/**************************************************************************************/
/**                                                                                \n**/
/**            w  r  i  t  e  r  e  g  i  o  n  c  o  d  e  .  c                   \n**/
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

int writeregioncode(Outputfile *output, /**< output file array */
                    int index, /**< output index */
                    const Cell grid[] /**< LPJ cell array */,
                    const Config *config /**< LPJmL configuration*/
                   )                     /** \return number of region codes written */
{
#ifdef USE_MPI
  MPI_Status status;
#endif
  short *vec;
  int cell,count;
  Bool rc;
  vec=newvec(short,config->count);
  if(vec==NULL)
  {
    printallocerr("vec");
    rc=TRUE;
  }
  else
    rc=FALSE;
  if(iserror(rc,config))
    return 0; 
  count=0;
  for(cell=0;cell<config->ngridcell;cell++)
    if(!grid[cell].skip)
      vec[count++]=(short)grid[cell].ml.manage.regpar->id;
#ifdef USE_MPI
  switch(output->method)
  {
    case LPJ_MPI2:
      MPI_File_write_at(output->files[index].fp.mpi_file,config->offset,vec,
                        count,MPI_SHORT,&status);
      break;
    case LPJ_GATHER:
      switch(output->files[index].fmt)
      {
        case RAW: case CLM:
          mpi_write(output->files[index].fp.file,vec,MPI_SHORT,config->total,
                    output->counts,output->offsets,config->rank,config->comm);
          break;
        case TXT:
          mpi_write_txt(output->files[index].fp.file,vec,MPI_SHORT,config->total,
                        output->counts,output->offsets,config->rank,config->comm);
          break;
        case CDF:
          mpi_write_netcdf(&output->files[index].fp.cdf,vec,MPI_SHORT,config->total,
                           NO_TIME,
                           output->counts,output->offsets,config->rank,config->comm);
          break;
      }
      break;
    case LPJ_SOCKET:
      mpi_write_socket(output->socket,vec,MPI_SHORT,config->total,
                       output->counts,output->offsets,config->rank,config->comm);
      break;
  } /* of 'switch' */
#else
  if(output->method==LPJ_FILES)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(vec,sizeof(short),count,output->files[index].fp.file)!=count)
          fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
        break;
      case TXT:
        for(cell=0;cell<count-1;cell++)
          fprintf(output->files[index].fp.file,"%d ",vec[cell]);
        fprintf(output->files[index].fp.file,"%d\n",vec[count-1]);
        break;
      case CDF:
        write_short_netcdf(&output->files[index].fp.cdf,vec,NO_TIME,count);
        break;
    }
  else
    writeshort_socket(output->socket,vec,count);
#endif
  free(vec);
  return count;
} /* of 'writeregioncode' */
