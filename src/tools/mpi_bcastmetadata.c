/**************************************************************************************/
/**                                                                                \n**/
/**               o  p  e  n  m  e  t  a  f  i  l  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions broadcasts metadata to all tasks                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_MPI

void mpi_bcastmetadata(Metadata *metadata, /**< metadata information */
                       int rank,           /**< MPI rank */
                       MPI_Comm comm       /**< MPI communicator */
                      )
{
  int i,len;
  if(rank==0)
  {
    if(metadata->map==NULL)
      len=-1;
    else
      len=getmapsize(metadata->map);
  }
  MPI_Bcast(&len,1,MPI_INT,0,comm);
  if(len>=0)
  {
    if(rank)
      metadata->map=newmap(FALSE,len);
    for(i=0;i<getmapsize(metadata->map);i++)
    {
      if(rank==0)
        len=(getmapitem(metadata->map,i)==NULL) ? 0 : strlen(getmapitem(metadata->map,i))+1;
      MPI_Bcast(&len,1,MPI_INT,0,comm);
      if(len==0)
        getmapitem(metadata->map,i)=NULL;
      else
      {
        if(rank)
        {
          getmapitem(metadata->map,i)=malloc(len);
          check(getmapitem(metadata->map,i));
        }
        MPI_Bcast(getmapitem(metadata->map,i),len,MPI_CHAR,0,comm);
      }
    }
  }
  if(rank==0)
  {
    if(metadata->attrs==NULL)
      len=-1;
    else
      len=metadata->n_attr;
  }
  MPI_Bcast(&len,1,MPI_INT,0,comm);
  if(len>=0)
  {
    metadata->n_attr=len;
    if(rank)
      metadata->attrs=newvec(Attr,len);
    for(i=0;i<metadata->n_attr;i++)
    {
      if(rank==0)
        len=strlen(metadata->attrs[i].name)+1;
      MPI_Bcast(&len,1,MPI_INT,0,comm);
      if(rank)
      {
        metadata->attrs[i].name=malloc(len);
        check(metadata->attrs[i].name);
      }
      MPI_Bcast(metadata->attrs[i].name,len,MPI_CHAR,0,comm);
      if(rank==0)
        len=strlen(metadata->attrs[i].value)+1;
      MPI_Bcast(&len,1,MPI_INT,0,comm);
      if(rank)
      {
        metadata->attrs[i].value=malloc(len);
        check(metadata->attrs[i].value);
      }
      MPI_Bcast(metadata->attrs[i].value,len,MPI_CHAR,0,comm);
    }
  }
  if(rank==0)
  {
    if(metadata->basetemp==NULL)
      len=-1;
    else
      len=metadata->basetemp_size;
  }
  MPI_Bcast(&len,1,MPI_INT,0,comm);
  if(len>=0)
  {
    metadata->basetemp_size=len;
    if(rank)
    {
      metadata->basetemp=newvec(Limit,metadata->basetemp_size);
      check(metadata->basetemp);
    }
    MPI_Bcast(metadata->basetemp,len*sizeof(Limit),MPI_BYTE,0,comm);
  }
  if(rank==0)
  {
    if(metadata->hlimit==NULL)
      len=-1;
    else
      len=metadata->hlimit_size;
  }
  MPI_Bcast(&len,1,MPI_INT,0,comm);
  if(len>=0)
  {
    metadata->hlimit_size=len;
    if(rank)
    {
      metadata->hlimit=newvec(int,metadata->hlimit_size);
      check(metadata->hlimit);
    }
    MPI_Bcast(metadata->hlimit,len,MPI_INT,0,comm);
  }
} /* of 'mpi_bcastmetadata' */
#endif
