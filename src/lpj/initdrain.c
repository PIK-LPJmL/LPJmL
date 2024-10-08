/**************************************************************************************/
/**                                                                                \n**/
/**                 i  n  i  t  d  r  a  i  n  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes irrigation neighbours and river routings              \n**/
/**                                                                                \n**/
/**     Mathematical description of the river routing algorithm can be             \n**/
/**     found in:                                                                  \n**/
/**     Rost, S., Gerten, D., Bondeau, A., Lucht, W., Rohwer, J.,                  \n**/
/**     Schaphoff, S.: Agricultural green and blue water consumption and           \n**/
/**     its influence on the global water system. Water Resources                  \n**/
/**     Research (in press).                                                       \n**/
/**                                                                                \n**/
/**     River routing is implemented using the Pnet library.                       \n**/
/**     Pnet  is described in:                                                     \n**/
/**     W. von Bloh, 2008. Sequential and Parallel Implementation of               \n**/
/**     Networks. In P. beim Graben, C. Zhou, M. Thiel, and J. Kurths              \n**/
/**     (eds.), Lectures in Supercomputational Neuroscience, Dynamics in           \n**/
/**     Complex Brain Networks, Springer, 279-318.                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static int *getindex(const Input_netcdf input,const Cell grid[],
                     const Config *config)
{
  int *item=NULL;
#ifdef USE_MPI
  int *vec;
  int *counts;
  int *offsets;
#endif
  int i,cell,n;
  int *index;
  Bool iserr;
  iserr=FALSE;
  if(isroot(*config))
  {
    item=newvec(int,config->nall);
    if(item==NULL)
      iserr=TRUE;
  }
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  if(counts==NULL)
    iserr=TRUE;
  offsets=newvec(int,config->ntask);
  if(offsets==NULL)
    iserr=TRUE;
  vec=newvec(int,config->ngridcell);
  if(vec==NULL)
    iserr=TRUE;
  if(iserror(iserr,config))
  {
    if(isroot(*config))
      free(item);
    free(vec);
    free(counts);
    free(offsets);
    return NULL;
  }
  getcounts(counts,offsets,config->nall,1,config->ntask);
  for(cell=0;cell<config->ngridcell;cell++)
    vec[cell]=(int)getindexinput_netcdf(input,&grid[cell].coord);
  MPI_Gatherv(vec,config->ngridcell,MPI_INT,
              item,counts,offsets,MPI_INT,
              0,config->comm);
  free(vec);
  free(counts);
  free(offsets);
#else
  if(iserr)
    return NULL;
  for(cell=0;cell<config->ngridcell;cell++)
    item[cell]=(int)getindexinput_netcdf(input,&grid[cell].coord);
#endif
  n=getindexsize_netcdf(input);
  index=newvec(int,n);
  if(index==NULL)
  {
    printallocerr("index");
    iserr=TRUE;
  }
  else if(isroot(*config))
  {
    for(i=0;i<n;i++)
      index[i]=-1;
    for(cell=0;cell<config->nall;cell++)
    {
      if(item[cell]>=n)
      {
        fprintf(stderr,"ERROR202: Invalid index %d at cell %d in '%s'.\n",
                item[cell],cell+config->startgrid,
                config->neighb_irrig_filename.name);
        iserr=TRUE;
        break;
      }
      index[item[cell]]=cell;
    } 
    free(item);
  }
  if(iserror(iserr,config))
  {
    free(index);
    return NULL;
  }
#ifdef USE_MPI
  MPI_Bcast(index,n,MPI_INT,0,config->comm);
#endif
  return index;
} /* of 'getindex' */

static Bool initirrig(Cell grid[],    /**< Cell grid             */
                      Config *config  /**< LPJ configuration     */
              )                       /** \return TRUE on error */
{
  Infile irrig_file;
  String line;
  int cell,neighb_irrig,rc,*index=NULL,n=0;
  /* open neighbour irrigation file */
  irrig_file.fmt=config->neighb_irrig_filename.fmt;
  if(openinputdata(&irrig_file,&config->neighb_irrig_filename,"irrigation",NULL,LPJ_INT,1.0,config))
    return TRUE;
  if(config->neighb_irrig_filename.fmt==CDF)
  {
    index=getindex(irrig_file.cdf,grid,config);
    if(index==NULL)
    {
      closeinput(&irrig_file);
      return TRUE;
    }
    n=getindexsize_netcdf(irrig_file.cdf);
  }
  /* initialize pnet structure for irrigation network */
#ifdef USE_MPI
  config->irrig_neighbour=pnet_init(config->comm,
                                    (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
                                    config->nall);
#else
  config->irrig_neighbour=pnet_init(sizeof(Real),config->nall);
#endif
  if(config->irrig_neighbour==NULL)
  {
    fputs("ERROR140: Cannot initialize irrigation network.\n",stderr);
    fflush(stderr);
    free(index);
    closeinput(&irrig_file);
    return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
    /* read connection from file */
    if(readintinputdata(&irrig_file,&neighb_irrig,NULL,&grid[cell].coord,cell+config->startgrid,&config->neighb_irrig_filename))
    {
      free(index);
      closeinput(&irrig_file);
      return TRUE;
    }
    if(config->neighb_irrig_filename.fmt==CDF)
    {
      if(neighb_irrig<0 ||  neighb_irrig>=n)
      {
        fprintf(stderr,"ERROR203: Invalid irrigation neighbour %d of cell %d (%s).\n",
                neighb_irrig,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput(&irrig_file);
        free(index);
        return TRUE;
      }
      neighb_irrig=index[neighb_irrig];
      if(neighb_irrig==-1)
      {
        fprintf(stderr,"ERROR203: Invalid irrigation neighbour %d of cell %d (%s).\n",
                neighb_irrig,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput(&irrig_file);
        free(index);
        return TRUE;
      }
    }
    /* add connection to network */
    rc=pnet_addconnect(config->irrig_neighbour,
                       cell+config->startgrid-config->firstgrid,
                       neighb_irrig-config->firstgrid);
    if(rc)
    {
      fprintf(stderr,"ERROR142: Cannot add irrigation neighbour %d of cell %d: %s.\n",
              neighb_irrig,
              cell+config->startgrid,
              pnet_strerror(rc));
      fflush(stderr);
      closeinput(&irrig_file);
      free(index);
      return TRUE;
    }
  } /* of 'for(cell=...)' */
  closeinput(&irrig_file);
  free(index);
  config->irrig_back=pnet_dup(config->irrig_neighbour); /* copy network */
  return (config->irrig_back==NULL);
} /* of 'initirrig' */

static Bool initriver(Cell grid[],Config *config)
{
  Infile drainage,river;
  int cell,rc;
  Routing r;
  Header header;
  String headername,line;
  int *index=NULL,n=0,version,ncoeff;
  Real len;
  Bool missing;
  size_t offset;
  drainage.fmt=config->drainage_filename.fmt;
  if(config->drainage_filename.fmt==CDF)
  {
    drainage.cdf=openinput_netcdf(&config->drainage_filename,
                                  NULL,0,config);
    if(drainage.cdf==NULL)
      return TRUE;
    river.cdf=openinput_netcdf(&config->river_filename,"m",0,config);
    if(river.cdf==NULL)
    {
      closeinput_netcdf(drainage.cdf);
      return TRUE;
    }
    index=getindex(drainage.cdf,grid,config);
    if(index==NULL)
    {
      closeinput_netcdf(drainage.cdf);
      return TRUE;
    }
    n=getindexsize_netcdf(drainage.cdf);
  }
  else
  {
    river.cdf=NULL;
    if((drainage.file=openinputfile(&header,&drainage.swap,&config->drainage_filename,
                                    headername,NULL,LPJ_INT,&version,&offset,FALSE,config))==NULL)
      return TRUE;
    if(header.datatype!=LPJ_INT)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR217: Datatype %s in drainage file '%s' is not int.\n",typenames[header.datatype],config->drainage_filename.name);
      fclose(drainage.file);
      return TRUE;
    }
    if(config->drainage_filename.fmt!=RAW && header.nbands!=2)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in drainage file '%s' is not 2.\n",
                header.nbands,config->drainage_filename.name);
      fclose(drainage.file);
      return TRUE;
    }
    /* seek startgrid positions in drainage file */
    if(fseek(drainage.file,sizeof(Routing)*(config->startgrid-header.firstcell)+offset,SEEK_CUR))
    {
      fprintf(stderr,"ERROR139: Cannot seek to drainage of cell %d.\n",
              config->startgrid);
      fclose(drainage.file);
      return TRUE;
    }
  }
  /* initialize pnet structure for drainage network */
#ifdef USE_MPI
  config->route=pnet_init(config->comm,
                          (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
                          config->nall);
#else
  config->route=pnet_init(sizeof(Real),config->nall);
#endif

  if(config->route==NULL)
  {
    fputs("ERROR143: Cannot initialize river network.\n",stderr);
    fflush(stderr);
    closeinput(&drainage);
    return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(config->drainage_filename.fmt==CDF)
    {
      if(readintinput_netcdf(drainage.cdf,&r.index,&grid[cell].coord,&missing) || missing)
      {
        fprintf(stderr,"ERROR203: Cannot read drainage of cell %d (%s).\n",
               cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput_netcdf(drainage.cdf);
        closeinput_netcdf(river.cdf);
        free(index);
        return TRUE;
      }
      if(r.index<-1 ||  r.index>=n)
      {
        fprintf(stderr,"ERROR203: Invalid drainage  %d of cell %d (%s).\n",
                r.index,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput_netcdf(drainage.cdf);
        closeinput_netcdf(river.cdf);
        free(index);
        return TRUE;
      }
      if(r.index>0)
      {
        r.index=index[r.index];
        if(r.index==-1)
        {
          fprintf(stderr,"ERROR203: Invalid drainage %d of cell %d (%s).\n",
                  r.index,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
          closeinput_netcdf(drainage.cdf);
          closeinput_netcdf(river.cdf);
          free(index);
          return TRUE;
        }
      }
      if(readinput_netcdf(drainage.cdf,&len,&grid[cell].coord))
      {
        closeinput_netcdf(drainage.cdf);
        closeinput_netcdf(river.cdf);
        free(index);
        return TRUE;
      }
      /* calculate transfer function */
      grid[cell].discharge.tfunct=transfer_function(len,&ncoeff);
    }
    else
    {
      if(getroute(drainage.file,&r,drainage.swap))
      {
        fprintf(stderr,"ERROR144: Cannot read river route for cell %d.\n",
                cell+config->startgrid);
        fflush(stderr);
        fclose(drainage.file);
        return TRUE;
      }
      /* calculate transfer function */
      grid[cell].discharge.tfunct=transfer_function(r.len,&ncoeff);
    }
    grid[cell].discharge.next=r.index;

    if(grid[cell].discharge.tfunct==NULL)
    {
      printallocerr("tfunct");
      closeinput(&drainage);
      if(config->drainage_filename.fmt==CDF)
      {
        closeinput_netcdf(river.cdf);
        free(index);
      }
      return TRUE;
    }
    /* initialize delay queue with the same size */
    if(grid[cell].discharge.queue==NULL) /* has queue been read by freadcell? */
    {
      grid[cell].discharge.queue=newqueue(ncoeff); /* no, allocate it */
      if(grid[cell].discharge.queue==NULL)
      {
        printallocerr("queue");
        closeinput(&drainage);
        if(config->drainage_filename.fmt==CDF)
        {
          closeinput_netcdf(river.cdf);
          free(index);
        }
        return TRUE;
      }
    }
    else if(queuesize(grid[cell].discharge.queue)!=ncoeff) /* check whether queue size in restart file has the same length as ncoeff */
    {
      /* The queue size is different, so resize the queue and set it to zero */
      fprintf(stderr,"ERROR256: Size of discharge queue=%d of cell %d in restart file differs from %d, queue is resized and set to zero.\n",
             queuesize(grid[cell].discharge.queue),cell+config->startgrid,ncoeff);
      freequeue(grid[cell].discharge.queue);
      grid[cell].discharge.queue=newqueue(ncoeff);
      if(grid[cell].discharge.queue==NULL)
      {
        printallocerr("queue");
        closeinput(&drainage);
        if(config->drainage_filename.fmt==CDF)
        {
          closeinput_netcdf(river.cdf);
          free(index);
        }
        return TRUE;
      }
    }

    if(r.index>=0)
    {
      /* add connection */
      rc=pnet_addconnect(config->route,
                         cell+config->startgrid-config->firstgrid,
                         r.index-config->firstgrid);
      if(rc)
      {
        fprintf(stderr,"ERROR146: Cannot add river route from cell %d to cell %d in '%s': %s.\n",
                r.index,cell+config->startgrid,config->drainage_filename.name,pnet_strerror(rc));
        fflush(stderr);
        closeinput(&drainage);
        if(config->drainage_filename.fmt==CDF)
        {
          closeinput_netcdf(river.cdf);
          free(index);
        }
        return TRUE;
      }

/*    printf("c:%d next:%d size:%d d:%d first.id:%d\n",cell,
             grid[cell].discharge.next,grid[cell].discharge.queue.size,r.len,
             grid[cell].discharge.queue.first); */

    }

  } /* of for '(cell=0;..)' */

/*   printf("outsize:%d\n",config->route->outsize); */
/*   printf("lo:%d hi:%d\n",config->route->lo,config->route->hi); */

  closeinput(&drainage);
  if(config->drainage_filename.fmt==CDF)
  {
    closeinput_netcdf(river.cdf);
    free(index);
  }
  return FALSE;
} /* of 'initriver' */

Bool initdrain(Cell grid[],    /**< Cell grid             */
               Config *config  /**< LPJ configuration     */
              )                /** \return TRUE on error  */
{
  if(config->withlanduse!=NO_LANDUSE)
  {
    if(iserror(initirrig(grid,config),config))
      return TRUE; 
    pnet_reverse(config->irrig_neighbour);
    pnet_setup(config->irrig_neighbour);
    pnet_setup(config->irrig_back);
  }
  if(iserror(initriver(grid,config),config))
    return TRUE;
  pnet_reverse(config->route);
  pnet_setup(config->route);
  return FALSE;
} /* of 'initdrain' */

/*
case1: l==0, i.e. c==0 && d[1]==-9 no value
case2: l>0 && v>l, i.e. d[c+1]==-1 ocean or inland sink, whole water of considered cell drains to the ocean or inland sink
case3: l>0 && v<=l, i.e. d[c+1]>=0 || d[c+1]==-1 distribution of the water between two cells, or one cell and the ocean/inland sink

*/
