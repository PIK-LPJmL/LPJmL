/**************************************************************************************/
/**                                                                                \n**/
/**                  w  r  i  t  e  a  r  e  a  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes terrestrial areas (land + lakes, lakes) to output file     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int writearea(Outputfile *output,  /**< output file array */
              int index,           /**< output index */
              const Cell grid[]    /**< LPJ cell array */,
              const Config *config /**< LPJmL configuration*/
             )                     /** \return number of areas written */
{
  int cell,count;
  Bool rc;
  float *vec;
  vec=newvec(float,config->count);
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
  switch(index)
  {
    case TERR_AREA:
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(float)grid[cell].coord.area;
      break;
    case LAKE_AREA:
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
          vec[count++]=(float)(grid[cell].coord.area*grid[cell].lakefrac);
      break;
    default:
      if(isroot(*config))
        fprintf(stderr,"ERROR258: Invalid index=%d in writearea().\n",index);
      free(vec);
      return 0;
  }
#ifdef USE_MPI
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        mpi_write(output->files[index].fp.file,vec,MPI_FLOAT,config->total,
                  output->counts,output->offsets,config->rank,config->comm);
        break;
      case TXT:
        mpi_write_txt(output->files[index].fp.file,vec,MPI_FLOAT,config->total,
                      output->counts,output->offsets,config->rank,config->csv_delimit,config->comm);
        break;
      case CDF:
        mpi_write_netcdf(&output->files[index].fp.cdf,vec,MPI_FLOAT,config->total,
                         NO_TIME,
                         output->counts,output->offsets,config->rank,config->comm);
        break;
    }
  if(output->files[index].issocket)
  {
    if(isroot(*config))
      send_token_coupler(PUT_DATA,index,config);
    mpi_write_socket(config->socket,vec,MPI_FLOAT,config->total,
                     output->counts,output->offsets,config->rank,config->comm);
  }
#else
  if(output->files[index].isopen)
    switch(output->files[index].fmt)
    {
      case RAW: case CLM:
        if(fwrite(vec,sizeof(float),count,output->files[index].fp.file)!=count)
          fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
        break;
      case TXT:
        for(cell=0;cell<count-1;cell++)
          fprintf(output->files[index].fp.file,"%g%c",vec[cell],config->csv_delimit);
        fprintf(output->files[index].fp.file,"%g\n",vec[count-1]);
        break;
      case CDF:
        write_float_netcdf(&output->files[index].fp.cdf,vec,NO_TIME,count);
        break;
    }
  if(output->files[index].issocket)
  {
    send_token_coupler(PUT_DATA,output->files[index].id,config);
    writefloat_socket(config->socket,vec,count);
  }
#endif
  free(vec);
  return count;
} /* of 'writearea' */
