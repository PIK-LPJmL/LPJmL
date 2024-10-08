/**************************************************************************************/
/**                                                                                \n**/
/**                 i  n  i  t  r  e  s  e  r  v  o  i  r  .  c                    \n**/
/**                                                                                \n**/
/**     Function initializes irrigation network for dam reservoirs                 \n**/
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
  int elevation;
  int next;
} Item;

static Bool initreservoir2(Cell grid[],   /**< LPJ grid */
                           Config *config /**< LPJ configuration */
                          )               /** \return TRUE on error */
{
  int cell;
  Real data;
  Bool swap;
  Header header;
  String headername;
  int version;
  FILE *file;
  char *name;
  size_t filesize;
  Infile input;
  Reservoir reservoir;
  size_t offset;
  if(openinputdata(&input,&config->elevation_filename,"elevation","m",LPJ_SHORT,1.0,config))
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readinputdata(&input,&data,&grid[cell].coord,cell+config->startgrid,&config->elevation_filename))
    {
      closeinput(&input);
      return TRUE;
    }
    grid[cell].elevation=(int)data;
  }
  closeinput(&input);
  if((file=openinputfile(&header,&swap,&config->reservoir_filename,
                         headername,NULL,LPJ_FLOAT,
                         &version,&offset,TRUE,config))==NULL)
    return TRUE;
  if(header.nbands!=10)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Number of bands=%d in reservoir data file '%s' is not 10.\n",
              header.nbands,config->reservoir_filename.name);
    fclose(file);
    return TRUE;
  }
  if(header.nstep!=1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Number of steps=%d in reservoir data file '%s' is not 1.\n",
              header.nstep,config->reservoir_filename.name);
    fclose(file);
    return TRUE;
  }
  if(header.timestep!=1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Time step=%d in reservoir data file '%s' is not 1.\n",
              header.timestep,config->reservoir_filename.name);
    fclose(file);
    return TRUE;
  }
  if(isroot(*config) && config->reservoir_filename.fmt!=META)
  {
    filesize=getfilesizep(file)-headersize(headername,version)-offset;
    if(filesize!=sizeof(int)*header.nyear*header.nbands*header.ncell)
      fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",
              config->reservoir_filename.name);
  }
  if(fseek(file,sizeof(Reservoir)*(config->startgrid-header.firstcell)+offset,SEEK_CUR))
  {
    name=getrealfilename(&config->reservoir_filename);
    fprintf(stderr,"ERROR150: Cannot seek reservoir data file '%s' to position %d.\n",
            name,config->startgrid);
    free(name);
    fclose(file);
    return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readreservoir(&reservoir,swap,file))
    {
      name=getrealfilename(&config->reservoir_filename);
      fprintf(stderr,"ERROR151: Cannot read reservoir data file '%s'.\n",
              name);
      free(name);
      fclose(file);
      return TRUE;
    }

    /*if constant landuse, all dams as in landuse year (but 'built' before)*/
    if(config->withlanduse==CONST_LANDUSE)
    {
      if(reservoir.year>0 && reservoir.year<=config->landuse_year_const)
        reservoir.year=1;
    }

    if(reservoir.year>0 && reservoir.year<=config->lastyear) /* Is there a dam? */
    {
      if(!grid[cell].ml.dam)
      {
        grid[cell].ml.resdata=new(Resdata);
        if(grid[cell].ml.resdata==NULL)
        {
          printallocerr("resdata");
          fclose(file);
          return TRUE;
        }
        grid[cell].ml.resdata->demand_fraction=0;
      }
      grid[cell].ml.resdata->reservoir=reservoir;

    }
    else /* no */
      grid[cell].ml.resdata=NULL;
  }
  fclose(file);
  return FALSE;
} /* of 'initreservoir2' */

Bool initreservoir(Cell grid[],   /**< LPJ grid */
                   Config *config /**< LPJ configuration */
                  )               /** \return TRUE on error */
{
  Bool iserr,*visit;
  int cell,i,j,count,index;
  Intlist *back,list,new;
  Item *recv;
#ifdef USE_MPI
  int *counts;
  int *offsets;
  Item *send;
#endif
  iserr=initreservoir2(grid,config);
  if(iserror(iserr,config))
    return TRUE;
#ifdef USE_MPI
  send=newvec(Item,config->ngridcell);
  check(send);
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  getcounts(counts,offsets,config->nall,2,config->ntask);
#endif
  recv=newvec(Item,config->nall);
  check(recv);
  for(cell=0;cell<config->ngridcell;cell++)
  {
#ifdef USE_MPI
    send[cell].next=grid[cell].discharge.next;
    send[cell].elevation=grid[cell].elevation;
#else
    recv[cell].next=grid[cell].discharge.next;
    recv[cell].elevation=grid[cell].elevation;
#endif
  }
#ifdef USE_MPI
  MPI_Allgatherv(send,counts[config->rank],MPI_INT,recv,counts,
                 offsets,MPI_INT,config->comm);
#endif
  back=newvec(Intlist,config->nall);
  check(back);
  for(cell=0;cell<config->nall;cell++)
    initintlist(back+cell);
  for(cell=0;cell<config->nall;cell++)
    if(recv[cell].next>=0)
      addintlistitem(back+recv[cell].next-config->firstgrid,cell);
  initintlist(&list);
#ifdef USE_MPI
  config->irrig_res=pnet_init(config->comm,
                                   (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
                                   config->nall);
#else
  config->irrig_res=pnet_init(sizeof(Real),config->nall);
#endif
  visit=newvec(Bool,config->nall);
  check(visit);
  for(cell=0;cell<config->ngridcell;cell++)
    if(grid[cell].ml.resdata!=NULL)
    {
      for(i=0;i<config->nall;i++)
        visit[i]=FALSE;
      index=cell+config->startgrid-config->firstgrid;
      addintlistitem(&list,index);
      visit[index]=TRUE;
      while(recv[index].next>=0 )/*&& k<20) */
      {
        index=recv[index].next-config->firstgrid;
        if(visit[index])
        {
          fprintf(stderr,
                  "WARNING017: Loop detected in river routing at cell=%d\n",
                  index);
          break;
        }
        visit[index]=TRUE;
        addintlistitem(&list,index);
      }
      for(count=0;count<5;count++) /*Amount of cells upstream in reach of reservoir */
      {
        initintlist(&new);
        for(i=0;i<list.n;i++)
        {
          pnet_addconnect(config->irrig_res,
                          cell+config->startgrid-config->firstgrid,
                          list.index[i]);
          if(count<4)
            for(j=0;j<back[list.index[i]].n;j++)
            {
              index=back[list.index[i]].index[j];
              if(!visit[index] &&
                 recv[index].elevation<recv[cell+config->startgrid-config->firstgrid].elevation)
              {
                visit[index]=TRUE;
                addintlistitem(&new,index);
              }
            }
        } /* of for(i=0;...) */
        emptyintlist(&list);
        list=new;
      }
      grid[cell].ml.resdata->fraction=newvec(Real,pnet_inlen(config->irrig_res,
                                          cell+config->startgrid-config->firstgrid));
      check(grid[cell].ml.resdata->fraction);
    }
  config->irrig_res_back=pnet_dup(config->irrig_res);
  pnet_reverse(config->irrig_res_back);
  pnet_setup(config->irrig_res);
  pnet_setup(config->irrig_res_back);
  for(cell=0;cell<config->ngridcell;cell++)
    if(pnet_inlen(config->irrig_res_back,cell+config->startgrid-config->firstgrid))
    {
      grid[cell].ml.fraction=newvec(Real,pnet_inlen(config->irrig_res_back,
                                    cell+config->startgrid-config->firstgrid));
      check(grid[cell].ml.fraction);
    }
    else
      grid[cell].ml.fraction=NULL;
  for(cell=0;cell<config->nall;cell++)
    emptyintlist(back+cell);
  free(back);
  free(recv);
  free(visit);
#ifdef USE_MPI
  free(send);
  free(counts);
  free(offsets);
#endif
  return FALSE;
} /* of 'initreservoir' */
