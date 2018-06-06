/****************************************************************************************/
/**                                                                                  \n**/
/**                     c  p  l  _  i  n  i  t  .  c                                 \n**/
/**                                                                                  \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "types.h"
#include "errmsg.h"
#include "cpl.h"

#ifdef USE_MPI

typedef  struct
{
  int index;
  int x,y;
  int taskid;
} Coord_t;

static int cmp(const Coord_t *a,const Coord_t *b)
{
  return a->taskid-b->taskid;
} /* of 'cmp' */

Cpl *cpl_init(const Cpl_coord grid[],     /**< local coordinate list */
              int len,                    /**< length of coordinate list */
              const int xlen[],           /**< x sizes of domain decomposition */
              const int ylen[],           /**< y sizes of domain decomposition */
              const int *taskids,         /**< 2-D task id array, xsize*ysize */
              int xsize,                  /**< length of xlen array */
              int ysize,                  /**< length of ylen array */
              MPI_Comm comm               /**< MPI communicator */
              )                           /** \return allocated data or NULL */
{
  MPI_Datatype type;
  Cpl *cpl;
  int i,j,xindex,yindex,ntask,rank;
  int *xdisp,*ydisp;
  Coord_t *c;
  Cpl_coord *coord;
  MPI_Comm_size(comm,&ntask);
  if(xsize*ysize!=ntask)
  {
    MPI_Comm_rank(comm,&rank);
    if(rank==0)
      fprintf(stderr,"ERROR501: Number of tasks %d is not %d x %d.\n",ntask,xsize,ysize); 
    return NULL;
  }
  cpl=new(Cpl);
  if(cpl==NULL)
  {
    printallocerr("cpl");
    return NULL;
  }
  cpl->comm=comm; 
  xdisp=newvec(int,xsize);
  if(xdisp==NULL)
  {
    printallocerr("xdisp");
    free(cpl);
    return NULL;
  }
  ydisp=newvec(int,ysize);
  if(ydisp==NULL)
  {
    printallocerr("ydisp");
    free(xdisp);
    free(cpl);
    return NULL;
  }
  xdisp[0]=xlen[0];
  ydisp[0]=ylen[0];
  for(i=1;i<xsize;i++) {
    xdisp[i]=xdisp[i-1]+xlen[i];
  }
  for(i=1;i<ysize;i++) {
    ydisp[i]=ydisp[i-1]+ylen[i];
  }
  c=newvec(Coord_t,len);
  if(c==NULL)
  {
    printallocerr("c");
    free(xdisp);
    free(ydisp);
    free(cpl);
    return NULL;
  }
  for(i=0;i<len;i++)
  {
    xindex=0;
    while(xindex<xsize && grid[i].x>=xdisp[xindex])
      xindex++;
    if(xindex==xsize)
    {
      fprintf(stderr,"ERROR502: x coordinate %d out of bounds.\n",grid[i].x);
      free(xdisp);
      free(ydisp);
      free(cpl);
      return NULL;
    }
    yindex=0;
    while(yindex<ysize && grid[i].y>=ydisp[yindex])
      yindex++;
    if(yindex==ysize)
    {
      fprintf(stderr,"ERROR502: y coordinate %d out of bounds.\n",grid[i].y);
      free(xdisp);
      free(ydisp);
      free(cpl);
      return NULL;
    }
    c[i].index=i;
    c[i].x=grid[i].x-xdisp[xindex]+xlen[xindex];
    c[i].y=grid[i].y-ydisp[yindex]+ylen[yindex];
    c[i].taskid=taskids[xindex+xsize*yindex];
  }
  free(xdisp);
  free(ydisp);
  /* sort in ascending order of task id */
  qsort(c,len,sizeof(Coord_t),(int (*)(const void *,const void *))cmp);
  cpl->index=newvec(int,len);
  if(cpl->index==NULL)
  {
    printallocerr("index");
    free(c);
    free(cpl);
    return NULL;
  }
  cpl->src_index=newvec(int,len);
  if(cpl->src_index==NULL)
  {
    printallocerr("src_index");
    free(c);
    free(cpl->index);
    free(cpl);
    return NULL;
  }
  coord=newvec(Cpl_coord,len);
  if(coord==NULL)
  {
    printallocerr("coord");
    free(cpl->index);
    free(cpl->src_index);
    free(c);
    free(cpl);
    return NULL;
  }
  for(i=0;i<len;i++)
  {
    cpl->index[i]=c[i].index;
    coord[i].x=c[i].x;
    coord[i].y=c[i].y;
  }
  for(i=0;i<len;i++)
    cpl->src_index[cpl->index[i]]=i;

  cpl->inlen=newvec(int,xsize*ysize);
  if(cpl->inlen==NULL)
  {
    printallocerr("inlen");
    free(cpl->index);
    free(cpl->src_index);
    free(coord);
    free(c);
    free(cpl);
    return NULL;
  }
  cpl->outlen=newvec(int,xsize*ysize);
  if(cpl->outlen==NULL)
  {
    printallocerr("outlen");
    free(cpl->index);
    free(cpl->src_index);
    free(cpl->inlen);
    free(coord);
    free(c);
    free(cpl);
    return NULL;
  }
  cpl->indisp=newvec(int,xsize*ysize);
  if(cpl->indisp==NULL)
  {
    printallocerr("indisp");
    free(cpl->index);
    free(cpl->src_index);
    free(cpl->inlen);
    free(cpl->outlen);
    free(coord);
    free(c);
    free(cpl);
    return NULL;
  }
  cpl->outdisp=newvec(int,xsize*ysize);
  if(cpl->outdisp==NULL)
  {
    printallocerr("outdisp");
    free(cpl->index);
    free(cpl->src_index);
    free(cpl->inlen);
    free(cpl->outlen);
    free(cpl->indisp);
    free(coord);
    free(c);
    free(cpl);
    return NULL;
  }
  j=0;
  for(i=0;i<xsize*ysize;i++)
  {
    cpl->inlen[i]=0;
    while(j<len && c[j].taskid==i)
    {
      j++;
      cpl->inlen[i]++;
    }
  }
  free(c);
  MPI_Alltoall(cpl->inlen,1,MPI_INT,cpl->outlen,1,MPI_INT,cpl->comm);
  cpl->indisp[0]=cpl->outdisp[0]=0;
  for(i=1;i<xsize*ysize;i++)
  {
    cpl->indisp[i]=cpl->inlen[i-1]+cpl->indisp[i-1];
    cpl->outdisp[i]=cpl->outlen[i-1]+cpl->outdisp[i-1];
  }
  cpl->outsize=cpl->outdisp[xsize*ysize-1]+cpl->outlen[xsize*ysize-1];
  cpl->coord=newvec(Cpl_coord,cpl->outsize);
  if(cpl->coord==NULL)
  {
    printallocerr("coord");
    free(cpl->index);
    free(cpl->src_index);
    free(cpl->inlen);
    free(cpl->outlen);
    free(cpl->indisp);
    free(cpl->outdisp);
    free(coord);
    free(cpl);
    return NULL;
  }
  MPI_Type_contiguous(2,MPI_INT,&type);
  MPI_Type_commit(&type);

  MPI_Alltoallv(coord,cpl->inlen,cpl->indisp,type,cpl->coord,cpl->outlen,
                cpl->outdisp,type,cpl->comm);
  MPI_Type_free(&type);
  free(coord);
  return cpl;
} /* of 'cpl_init' */

#else

Cpl *cpl_init(const Cpl_coord grid[],int len,int xlen,int ylen)
{
  int i;
  Cpl *cpl;
  cpl=new(Cpl);
  if(cpl==NULL)
  {
    printallocerr("cpl");
    return NULL;
  }
  cpl->outsize=len;
  cpl->coord=newvec(Cpl_coord,len);
  if(cpl->coord==NULL)
  {
    printallocerr("coord");
    free(cpl);
    return NULL;
  }
  cpl->index=newvec(int,len);
  if(cpl->index==NULL)
  {
    printallocerr("index");
    free(cpl->coord);
    free(cpl);
    return NULL;
  }
  cpl->src_index=newvec(int,len);
  if(cpl->src_index==NULL)
  {
    printallocerr("index");
    free(cpl->coord);
    free(cpl->index);
    free(cpl);
    return NULL;
  }
  for(i=0;i<len;i++)
  {
    cpl->index[i]=cpl->src_index[i]=i;
    if(grid[i].x>=xlen || grid[i].y>=ylen)
    {
      fprintf(stderr,"ERROR501: Coordinate (%d,%d) out of bounds.\n",grid[i].x,grid[i].y);
      free(cpl->coord); 
      free(cpl->index);
      free(cpl->src_index);
      free(cpl);
      return NULL;
    }
    cpl->coord[i]=grid[i];
  }
  return cpl;
} /* of 'cpl_init' */

#endif
