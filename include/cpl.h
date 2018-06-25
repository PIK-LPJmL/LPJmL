/****************************************************************************************/
/**                                                                                  \n**/
/**                           c  p  l  .  h                                          \n**/
/**                                                                                  \n**/
/**     Header file for parallel coupling coordinate list with domain-               \n**/
/**     decomposed arrays (2-D rectangular decompostion)                             \n**/
/**                                                                                  \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef CPL_H
#define CPL_H

/* Definition of datatypes */

typedef struct
{
  int x,y;    // indices of the cell position in the global grid, 0..n-1, 0..m-1
} Cpl_coord;

typedef struct
{
#ifdef USE_MPI
  MPI_Comm comm;
  int *inlen;
  int *outlen;
  int *indisp;
  int *outdisp;
#endif
  int outsize;
  int *index;
  int *src_index;
  Cpl_coord *coord;
} Cpl;

/* Declaration of functions */

#ifdef USE_MPI
/** cpl_init() returns allocated data or NULL */
extern Cpl *cpl_init(const Cpl_coord grid[],     /*! local coordinate list */
                     int len,                    /*! length of coordinate list */ 
                     const int xlen[],           /*! x sizes of domain decomposition */
                     const int ylen[],           /*! y sizes of domain decomposition */
                     const int *taskids,         /*! 2-D array [xsize][ysize] of MPI task ids in the domain decomposition */
                     int xsize,                  /*! length of xlen array, i.e. nr of tasks in x-direction */
                     int ysize,                  /*! length of ylen array, i.e. nr of tasks in y-direction */
                     MPI_Comm comm);
#else
extern Cpl *cpl_init(const Cpl_coord [],int,int,int);
#endif
extern void cpl_free(Cpl *);

/* Definition of macros */

#ifdef USE_MPI
#define cpl_toarr(cpl,src,dst,type) MPI_Alltoallv(src,cpl->inlen,cpl->indisp,type,dst,cpl->outlen,cpl->outdisp,type,cpl->comm)
#define cpl_togrid(cpl,src,dst,type) MPI_Alltoallv(src,cpl->outlen,cpl->outdisp,type,dst,cpl->inlen,cpl->indisp,type,cpl->comm)
#else
#define cpl_toarr(cpl,src,dst,size) memcpy(src,dst,cpl->outsize*(size))
#define cpl_togrid(cpl,src,dst,size) memcpy(src,dst,cpl->outsize*(size))
#endif
#define cpl_index(cpl,i) cpl->index[i]
#define cpl_src_index(cpl,i) cpl->src_index[i]
#define cpl_xcoord(cpl,i) cpl->coord[i].x
#define cpl_ycoord(cpl,i) cpl->coord[i].y
#define cpl_size(cpl) cpl->outsize

#endif /* CPL_H */
