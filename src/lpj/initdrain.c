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
    iserr=TRUE;
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

static Bool getroute(FILE *file, /**< pointer to binary file */
                     Routing *r, /**< River route read */
                     Bool swap   /**< Byte order has to be changed */
                    )            /** \return TRUE on error */
{
  /* Function reads river route from file */
  if(fread(r,sizeof(Routing),1,file)!=1)
    return TRUE;
  if(swap)
  {
    /* byte order has to be changed */
    r->index=swapint(r->index);
    r->len=swapint(r->len);
  }
  return FALSE;
} /* of 'getroute' */

static Bool initirrig(Cell grid[],    /* Cell grid             */
                      Config *config  /* LPJ configuration     */
              )                /* returns TRUE on error */
{
  Infile irrig_file;
  Header header;
  String headername,line;
  int cell,neighb_irrig,rc,*index,n,version;
  Bool swap;
  size_t offset;
  /* open neighbour irrigation file */
  if(config->neighb_irrig_filename.fmt==CDF)
  {
    irrig_file.cdf=openinput_netcdf(config->neighb_irrig_filename.name,
                                    config->neighb_irrig_filename.var,NULL,0,
                                    config);
    if(irrig_file.cdf==NULL)
      return TRUE;
    index=getindex(irrig_file.cdf,grid,config);
    if(index==NULL)
    {
      closeinput_netcdf(irrig_file.cdf);
      return TRUE;
    }
    n=getindexsize_netcdf(irrig_file.cdf);
  }
  else
  {
    if((irrig_file.file=openinputfile(&header,&swap,&config->neighb_irrig_filename,
                                      headername,
                                      &version,&offset,config))==NULL)
      return TRUE;
    if(version>=3 && header.datatype!=LPJ_INT)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR217: Datatype %s in irrigation neighbour file '%s' is not int.\n",typenames[header.datatype],config->neighb_irrig_filename.name);
      fclose(irrig_file.file);
      return TRUE;
    }
    if(config->neighb_irrig_filename.fmt!=RAW && header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in irrigation neighbour file '%s' is not 1.\n",
                header.nbands,config->neighb_irrig_filename.name);
      fclose(irrig_file.file);
      return TRUE;
    }
    if(fseek(irrig_file.file,sizeof(neighb_irrig)*(config->startgrid-header.firstcell)+offset,SEEK_CUR))
    {
      fprintf(stderr,"ERROR139: Cannot seek to irrigation neighbour of %d.\n",
              config->startgrid);
      fflush(stderr);
      fclose(irrig_file.file);
      return TRUE;
    }
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
    closeinput(irrig_file,config->neighb_irrig_filename.fmt);
    return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
    /* read connection from file */
    if(config->neighb_irrig_filename.fmt==CDF)
    {
      if(readintinput_netcdf(irrig_file.cdf,&neighb_irrig,&grid[cell].coord))
      {
        closeinput_netcdf(irrig_file.cdf);
        free(index);
        return TRUE;
      }
      if(neighb_irrig<0 ||  neighb_irrig>=n)
      {
        fprintf(stderr,"ERROR203: Invalid irrigation neighbour %d of cell %d (%s).\n",
                neighb_irrig,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput_netcdf(irrig_file.cdf);
        free(index);
        return TRUE;
      }
      neighb_irrig=index[neighb_irrig];
      if(neighb_irrig==-1)
      {
        fprintf(stderr,"ERROR203: Invalid irrigation neighbour %d of cell %d (%s).\n",
                neighb_irrig,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput_netcdf(irrig_file.cdf);
        free(index);
        return TRUE;
      }
    }
    else
      if(fread(&neighb_irrig,sizeof(neighb_irrig),1,irrig_file.file)!=1)
      {
        fprintf(stderr,"ERROR141: Cannot read irrigation neighbour of cell %d.\n",
                cell+config->startgrid);
        fflush(stderr);
        fclose(irrig_file.file);
        return TRUE;
      }
    /* add connection to network */
    rc=pnet_addconnect(config->irrig_neighbour,
                       cell+config->startgrid-config->firstgrid,
                       ((swap) ? swapint(neighb_irrig) : neighb_irrig)-config->firstgrid);
    if(rc)
    {
      fprintf(stderr,"ERROR142: Cannot add irrigation neighbour %d of cell %d: %s.\n",
              ((swap) ? swapint(neighb_irrig) : neighb_irrig),
              cell+config->startgrid,
              pnet_strerror(rc));
      fflush(stderr);
      closeinput(irrig_file,config->neighb_irrig_filename.fmt);
      if(config->neighb_irrig_filename.fmt==CDF)
        free(index);
      return TRUE;
    }
  } /* of 'for(cell=...)' */
  closeinput(irrig_file,config->neighb_irrig_filename.fmt);
  if(config->neighb_irrig_filename.fmt==CDF)
    free(index);
  config->irrig_back=pnet_dup(config->irrig_neighbour); /* copy network */
  return (config->irrig_back==NULL);
} /* of 'initirrig' */

static Bool initriver(Cell grid[],Config *config)
{
  Infile drainage,river;
  int cell,rc;
  Routing r;
  Bool swap;
  Header header;
  String headername,line;
  int *index,n,version,ncoeff;
  Real len;
  size_t offset;
  if(config->drainage_filename.fmt==CDF)
  {
    drainage.cdf=openinput_netcdf(config->drainage_filename.name,
                                  config->drainage_filename.var,NULL,0,
                                  config);
    if(drainage.cdf==NULL)
      return TRUE;
    river.cdf=openinput_netcdf(config->river_filename.name,
                               config->river_filename.var,"m",0,
                               config);
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

    if((drainage.file=openinputfile(&header,&swap,&config->drainage_filename,
                                    headername,&version,&offset,config))==NULL)
      return TRUE;
    if(version>=3 && header.datatype!=LPJ_INT)
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
      return TRUE;
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
    fclose(drainage.file);
    return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(config->drainage_filename.fmt==CDF)
    {
      if(readintinput_netcdf(drainage.cdf,&r.index,&grid[cell].coord))
      {
        closeinput_netcdf(drainage.cdf);
        closeinput_netcdf(river.cdf);
        free(index);
        return TRUE;
      }
      if(r.index<0 ||  r.index>=n)
      {
        fprintf(stderr,"ERROR203: Invalid irrigation neighbour %d of cell %d (%s).\n",
                r.index,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput_netcdf(drainage.cdf);
        closeinput_netcdf(river.cdf);
        free(index);
        return TRUE;
      }
      r.index=index[r.index];
      if(r.index==-1)
      {
        fprintf(stderr,"ERROR203: Invalid irrigation neighbour %d of cell %d (%s).\n",
                r.index,cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
        closeinput_netcdf(drainage.cdf);
        closeinput_netcdf(river.cdf);
        free(index);
        return TRUE;
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
      if(getroute(drainage.file,&r,swap))
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
      closeinput(drainage,config->drainage_filename.fmt);
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
        closeinput(drainage,config->drainage_filename.fmt);
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
        fprintf(stderr,"ERROR146: Cannot add river route from cell %d to cell %d: %s.\n",
                r.index,cell+config->startgrid,pnet_strerror(rc));
        fflush(stderr);
        closeinput(drainage,config->drainage_filename.fmt);
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

  closeinput(drainage,config->drainage_filename.fmt);
  if(config->drainage_filename.fmt==CDF)
  {
    closeinput_netcdf(river.cdf);
    free(index);
  }
  return FALSE;
} /* of 'initriver' */

Bool initdrain(Cell grid[],    /* Cell grid             */
               Config *config  /* LPJ configuration     */
              )                /* returns TRUE on error */
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
