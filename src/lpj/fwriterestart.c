/**************************************************************************************/
/**                                                                                \n**/
/**                 f  w  r  i  t  e  r  e  s  t  a  r  t  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions writes restart/checkpoint file.                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define MSGTAG 101 /* MPI message tag */

Bool fwriterestart(const Cell grid[],   /**< cell array               */
                   int npft,            /**< number of natural PFTs   */
                   int ncft,            /**< number of crop PFTs      */
                   int year,            /**< year                     */
                   const char *filename,/**< filename of restart file */
                   Bool ischeckpoint,   /**< file is checkpoint file  */
                   const Config *config /**< LPJ configuration        */
                  )                     /** \return TRUE on error     */
{
#ifdef USE_MPI
  MPI_Status status;
  Bool iserror;
#endif
  Bstruct file;
  char *s;
  long long *index;  /* index vector storing file position of each LPJ cell */
  long long filepos; /* position of first element of index vector in restart file */
  time_t t;
  int p;
  if(isroot(*config))
    /* create restart file */
    file=bstruct_create(filename);
  else
  {
#ifdef USE_MPI
    /* wait until previous task has finished work */
    MPI_Recv(&iserror,1,MPI_INT,config->rank-1,MSGTAG,config->comm,&status);
    if(iserror)
    {
      if(config->rank<config->ntask-1)
        MPI_Send(&iserror,1,MPI_INT,config->rank+1,MSGTAG,config->comm);
      return TRUE;
    }
    /* get file position of index vector */
    MPI_Recv(&filepos,1,MPI_LONG,config->rank-1,MSGTAG,config->comm,&status);
#endif
    /* append file */
    file=bstruct_append(filename,TRUE);
  }
  if(file==NULL)
  {
    printfcreateerr(filename);
#ifdef USE_MPI
    iserror=TRUE;
    if(config->rank<config->ntask-1)
      MPI_Send(&iserror,1,MPI_INT,config->rank+1,MSGTAG,config->comm);
#endif
    return TRUE;
  }
  if(isroot(*config))
  {
    /* write header in restart file */
    bstruct_writestruct(file,"header");
    bstruct_writestring(file,"version",getversion());
    bstruct_writestring(file,"sim_name",config->sim_name);
    time(&t);
    s=getsprintf("%s: %s",strdate(&t),config->arglist);
    bstruct_writestring(file,"history",s);
    free(s);
    bstruct_writestruct(file,"global_attrs");
    bstruct_writestring(file,"GIT_repo",getrepo());
    bstruct_writestring(file,"GIT_hash",gethash());
    for(p=0;p<config->n_global;p++)
    {
      bstruct_writestring(file,config->global_attrs[p].name,config->global_attrs[p].value);
    }
    bstruct_writeendstruct(file);
    bstruct_writeint(file,"year",year);
    bstruct_writeint(file,"firstcell",config->startgrid);
    bstruct_writeint(file,"npft",npft);
    bstruct_writeint(file,"ncft",ncft);
    bstruct_writereal(file,"cellsize_lat",config->resolution.lat);
    bstruct_writereal(file,"cellsize_lon",config->resolution.lon);
    bstruct_writeint(file,"datatype",(sizeof(Real)==sizeof(float)) ? LPJ_FLOAT : LPJ_DOUBLE);
    bstruct_writebool(file,"landuse",(config->withlanduse!=NO_LANDUSE));
    bstruct_writeint(file,"sdate_option",config->sdate_option);
    bstruct_writebool(file,"crop_phu_option",config->crop_phu_option>=PRESCRIBED_CROP_PHU);
    bstruct_writebool(file,"river_routing",config->river_routing);
    bstruct_writebool(file,"separate_harvests",config->separate_harvests);
    bstruct_writearray(file,"pfts",npft+ncft);
    for(p=0;p<npft+ncft;p++)
      bstruct_writestring(file,NULL,config->pftpar[p].name);
    bstruct_writeendarray(file);
    fwriteseed(file,"seed",config->seed);
    bstruct_writeendstruct(file);
    /* define array with index vector and get position of first element of index vector */
    bstruct_writeindexarray(file,"grid",&filepos,config->nall);
  }
  index=newvec(long long,config->ngridcell);
  check(index);
  /* write cell data and get index vector */
  if(fwritecell(file,index,grid,config->ngridcell,ncft,npft,ischeckpoint,config)!=config->ngridcell)
  {
    fprintf(stderr,"ERROR153: Cannot write data in restart file '%s': %s\n",
            filename,strerror(errno));
    free(index);
    bstruct_close(file);
#ifdef USE_MPI
    iserror=TRUE;
    if(config->rank<config->ntask-1)
      MPI_Send(&iserror,1,MPI_INT,config->rank+1,MSGTAG,config->comm);
#endif
    return TRUE;
  }
  if(config->rank==config->ntask-1)
    bstruct_writeendarray(file);
  /* write index vector */
  bstruct_writearrayindex(file,filepos,index,config->startgrid-config->firstgrid,config->ngridcell);
  if(config->ntask>1)
    /* wait until all data are completely written */
    bstruct_sync(file);
  bstruct_close(file);
  free(index);
#ifdef USE_MPI
  iserror=FALSE;
  if(config->rank<config->ntask-1)
  {
    /* send message to next task to write further data */
    MPI_Send(&iserror,1,MPI_INT,config->rank+1,MSGTAG,config->comm);
    /* send file positiom ogf index vector */
    MPI_Send(&filepos,1,MPI_LONG,config->rank+1,MSGTAG,config->comm);
  }
#endif
  return FALSE;
} /* of 'fwriterestart' */
