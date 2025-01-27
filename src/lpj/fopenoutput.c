/**************************************************************************************/
/**                                                                                \n**/
/**                  f  o  p  e  n  o  u  t  p  u  t  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function creates files for output                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static Bool create(Netcdf *cdf,const char *filename,int index,
                   Coord_array *array,const Config *config)
{
  int size;
  size=outputsize(config->outputvars[index].id,
                  config->npft[GRASS]+config->npft[TREE],
                  config->npft[CROP],config);
  if(size==1)
    return create_netcdf(cdf,filename,
                         (config->outputvars[index].id==GRID) ? "cellid" :
                         config->outnames[config->outputvars[index].id].var,
                         (config->outputvars[index].id==GRID) ? "cell id" :
                         config->outnames[config->outputvars[index].id].standard_name,
                         config->outnames[config->outputvars[index].id].long_name,
                         (config->outputvars[index].id==GRID) ? "" :
                         config->outnames[config->outputvars[index].id].unit,
                         (config->outputvars[index].id==GRID) ? LPJ_INT: getoutputtype(config->outputvars[index].id,config->grid_type),
                         getnyear(config->outnames,config->outputvars[index].id),
                         (config->outnames[config->outputvars[index].id].timestep==ANNUAL) ? 1 : config->outnames[config->outputvars[index].id].timestep,
                         0,FALSE,array,config);
  else
    return create_pft_netcdf(cdf,filename,
                             config->outputvars[index].id,
                             config->npft[GRASS]+config->npft[TREE],
                             config->npft[CROP],
                             config->outnames[config->outputvars[index].id].var,
                             config->outnames[config->outputvars[index].id].standard_name,
                             config->outnames[config->outputvars[index].id].long_name,
                             config->outnames[config->outputvars[index].id].unit,
                             getoutputtype(config->outputvars[index].id,config->grid_type),
                             getnyear(config->outnames,config->outputvars[index].id),
                             (config->outnames[config->outputvars[index].id].timestep==ANNUAL) ? 1 : config->outnames[config->outputvars[index].id].timestep,0,FALSE,array,config);
} /* of 'create' */

static void openfile(Outputfile *output,const Cell grid[],
                     const char *filename,int i,
                     const Config *config)
{
  Header header;
  if(config->outputvars[i].filename.fmt==CDF)
  {
    switch(config->outputvars[i].id)
    {
      case ADISCHARGE:
        if(output->index_all==NULL)
        {
          output->index_all=createcoord_all(grid,config);
          if(output->index_all==NULL)
          {
            output->files[config->outputvars[i].id].isopen=FALSE;
            return;
          }
        }
        break;
      default:
        if(output->index==NULL)
        {
          output->index=createcoord(output,grid,config);
          if(output->index==NULL)
          {
            output->files[config->outputvars[i].id].isopen=FALSE;
            return;
          }
        }
        break;
    } /* of switch */
    if(isroot(*config))
    {
      if(!config->outputvars[i].oneyear)
      {
        if(!config->ischeckpoint && config->outputvars[i].filename.meta)
          fprintoutputjson(i,0,config);
        if(!create(&output->files[config->outputvars[i].id].fp.cdf,filename,i,
                   (config->outputvars[i].id==ADISCHARGE) ? output->index_all : output->index,config))
          output->files[config->outputvars[i].id].isopen=TRUE;
      }
    }
  }
  else if(isroot(*config) && !config->outputvars[i].oneyear)
  {
    if(!config->ischeckpoint && config->outputvars[i].filename.meta)
      fprintoutputjson(i,0,config);
    switch(config->outputvars[i].filename.fmt)
    {
       case CLM:
        if(config->ischeckpoint && getnyear(config->outnames,config->outputvars[i].id)!=0)
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"r+b"))==NULL)
            printfopenerr(config->outputvars[i].filename.name);
          else
          {
            output->files[config->outputvars[i].id].isopen=TRUE;
            if(config->checkpointyear>=config->outputyear)
            {
              fseek(output->files[config->outputvars[i].id].fp.file,
                    headersize(LPJOUTPUT_HEADER,config->outputvars[i].filename.version)+
                    getsize(i,config)*(config->checkpointyear-config->outputyear+1),SEEK_SET);
            }
            else
              fseek(output->files[config->outputvars[i].id].fp.file,
                    headersize(LPJOUTPUT_HEADER,config->outputvars[i].filename.version),SEEK_SET);
          }
        }
        else
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
            printfcreateerr(config->outputvars[i].filename.name);
          else
          {
            output->files[config->outputvars[i].id].isopen=TRUE;
            header.firstyear=config->outputyear;
            if(config->outputvars[i].id==ADISCHARGE)
              header.ncell=config->nall;
            else
              header.ncell=config->total;
            header.firstcell=config->firstgrid;
            header.cellsize_lon=(float)config->resolution.lon;
            header.cellsize_lat=(float)config->resolution.lat;
            header.scalar=1;
            if(config->outputvars[i].id==GRID)
            {
              header.datatype=config->grid_type;
              if(header.datatype==LPJ_SHORT)
                header.scalar=0.01;
              header.nbands=2;
              header.nstep=1;
              header.timestep=1;
              header.nyear=1;
              header.order=CELLYEAR;
              fwriteheader(output->files[config->outputvars[i].id].fp.file,
                           &header,LPJGRID_HEADER,config->outputvars[i].filename.version);
            }
            else
            {
              header.order=CELLSEQ;
              if(getnyear(config->outnames,config->outputvars[i].id)==0)
              {
                header.nstep=1;
                header.timestep=1;
                header.nyear=1;
              }
              else
              {
                header.nstep=getnyear(config->outnames,config->outputvars[i].id);
                header.timestep=max(1,config->outnames[config->outputvars[i].id].timestep);
                header.nyear=(config->lastyear-config->outputyear+1)/header.timestep;
                header.firstyear=config->outputyear+header.timestep-1;
              }
              header.nbands=outputsize(config->outputvars[i].id,
                                       config->npft[GRASS]+config->npft[TREE],
                                       config->npft[CROP],config);
              header.datatype=getoutputtype(config->outputvars[i].id,config->grid_type);
              fwriteheader(output->files[config->outputvars[i].id].fp.file,
                           &header,LPJOUTPUT_HEADER,config->outputvars[i].filename.version);
            }
          }
        }
        break;
      case RAW:
        if(config->ischeckpoint && getnyear(config->outnames,config->outputvars[i].id)!=0)
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"r+b"))==NULL)
            printfopenerr(config->outputvars[i].filename.name);
          else
          {
            output->files[config->outputvars[i].id].isopen=TRUE;
            if(config->checkpointyear>=config->outputyear)
            {
              fseek(output->files[config->outputvars[i].id].fp.file,
                    getsize(i,config)*(config->checkpointyear-config->outputyear+1),SEEK_SET);
            }
          }
        }
        else
        {
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
            printfcreateerr(config->outputvars[i].filename.name);
          else
            output->files[config->outputvars[i].id].isopen=TRUE;
        }
        break;
      case TXT:
        if((output->files[config->outputvars[i].id].fp.file=fopen(filename,(config->ischeckpoint && getnyear(config->outnames,config->outputvars[i].id)!=0) ? "a" : "w"))==NULL)
          printfcreateerr(config->outputvars[i].filename.name);
        else
          output->files[config->outputvars[i].id].isopen=TRUE;
        break;
    } /* of switch */
  }
  output->files[config->outputvars[i].id].oneyear=config->outputvars[i].oneyear;
} /* of 'openfile' */

Outputfile *fopenoutput(const Cell grid[],   /**< LPJ grid */
                        int n,               /**< size of output file array */
                        const Config *config /**< LPJmL configuration */
                       )                     /** \return output file data or NULL */
{
  int i,size;
#ifdef USE_MPI
  int count;
#endif
  int ncell;
  char *filename;
  Outputfile *output;
  output=new(Outputfile);
  check(output);
  output->files=newvec(File,n);
  check(output->files);
  output->n=n;
  output->index=output->index_all=NULL; 
  for(i=0;i<n;i++)
    output->files[i].isopen=output->files[i].issocket=output->files[i].oneyear=FALSE;
#ifdef USE_MPI
  output->counts=newvec(int,config->ntask);
  check(output->counts);
  output->offsets=newvec(int,config->ntask);
  check(output->offsets);
  count=config->count;
  MPI_Allgather(&count,1,MPI_INT,output->counts,1,MPI_INT,
                config->comm);
  /* calculate array offsets */
  output->offsets[0]=0;
  for(i=1;i<config->ntask;i++)
    output->offsets[i]=output->offsets[i-1]+output->counts[i-1];
#endif
  outputnames(output,config);
  for(i=0;i<config->n_out;i++)
  {
    if(hassuffix(config->outputvars[i].filename.name,config->compress_suffix))
    { 
      output->files[config->outputvars[i].id].compress=TRUE;
      filename=stripsuffix(config->outputvars[i].filename.name);
      check(filename);
    }
    else
    {
      output->files[config->outputvars[i].id].compress=FALSE;
      filename=config->outputvars[i].filename.name;
    }
    output->files[config->outputvars[i].id].filename=config->outputvars[i].filename.name;
    output->files[config->outputvars[i].id].fmt=config->outputvars[i].filename.fmt;
    if(iscoupled(*config) && config->outputvars[i].filename.issocket)
    {
      output->files[config->outputvars[i].id].issocket=TRUE;
      output->files[config->outputvars[i].id].id=config->outputvars[i].filename.id;
      if(isroot(*config))
      {
        if(config->outputvars[i].id==GLOBALFLUX)
          ncell=0;
        else
          ncell=(config->outputvars[i].id==ADISCHARGE) ? config->nall : config->total;
        if(config->outputvars[i].id==GLOBALFLUX)
          size=sizeof(Flux)/sizeof(Real);
        else if(config->outputvars[i].id==GRID)
          size=2;
        else
          size=outputsize(config->outputvars[i].id,
                          config->npft[GRASS]+config->npft[TREE],
                          config->npft[CROP],config);
        if(openoutput_coupler(config->outputvars[i].filename.id,ncell,getnyear(config->outnames,config->outputvars[i].id),size,getoutputtype(config->outputvars[i].id,config->grid_type),config))
        {
          output->files[config->outputvars[i].id].issocket=FALSE;
          fprintf(stderr,"ERROR100: Cannot open socket stream for output '%s'.\n",
                  config->outnames[config->outputvars[i].id].name);
        }
      }
#ifdef USE_MPI
      MPI_Bcast(&output->files[config->outputvars[i].id].issocket,1,MPI_INT,
                0,config->comm);
#endif
    }
    if(config->outputvars[i].filename.fmt!=SOCK)
      openfile(output,grid,filename,i,config);
#ifdef USE_MPI
    MPI_Bcast(&output->files[config->outputvars[i].id].isopen,1,MPI_INT,
              0,config->comm);
#endif
    if(config->pedantic && config->outputvars[i].filename.fmt!=SOCK && !output->files[config->outputvars[i].id].isopen)
      return NULL;
    if(output->files[config->outputvars[i].id].compress)
      free(filename);
  }
  return output;
} /* of 'fopenoutput' */

void openoutput_yearly(Outputfile *output,int year,const Config *config)
{
  char *filename;
  Header header;
  int i,size;
  for(i=0;i<config->n_out;i++)
    if(config->outputvars[i].oneyear)
    {
      if(isroot(*config))
      {
        filename=getsprintf(config->outputvars[i].filename.name,year);
        check(filename);
        if(config->outputvars[i].filename.meta)
          fprintoutputjson(i,year,config);
        switch(config->outputvars[i].filename.fmt)
        {
          case CLM:
            if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
            {
              printfcreateerr(filename);
              output->files[config->outputvars[i].id].isopen=FALSE;
            }
            else
            {
              output->files[config->outputvars[i].id].isopen=TRUE;
              header.firstyear=year;
              header.nyear=1;
              header.ncell=config->total;
              header.firstcell=config->firstgrid;
              header.cellsize_lon=(float)config->resolution.lon;
              header.cellsize_lat=(float)config->resolution.lat;
              header.scalar=1;
              header.order=CELLSEQ;
              header.timestep=1;
              header.nstep=getnyear(config->outnames,config->outputvars[i].id);
              header.nbands=outputsize(config->outputvars[i].id,
                                       config->npft[GRASS]+config->npft[TREE],
                                       config->npft[CROP],config);
              if(config->outputvars[i].id==SDATE || config->outputvars[i].id==HDATE || config->outputvars[i].id==SEASONALITY)
                header.datatype=LPJ_SHORT;
              else
                header.datatype=LPJ_FLOAT;
              fwriteheader(output->files[config->outputvars[i].id].fp.file,
                           &header,LPJOUTPUT_HEADER,config->outputvars[i].filename.version);

            }
            break;
        case RAW:
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"wb"))==NULL)
          {
            printfcreateerr(filename);
            output->files[config->outputvars[i].id].isopen=FALSE;
          }
          else
            output->files[config->outputvars[i].id].isopen=TRUE;
          break;
        case TXT:
          if((output->files[config->outputvars[i].id].fp.file=fopen(filename,"w"))==NULL)
          {
            output->files[config->outputvars[i].id].isopen=FALSE;
            printfcreateerr(filename);
          }
          else
            output->files[config->outputvars[i].id].isopen=TRUE;
          break;
        case CDF:
          size=outputsize(config->outputvars[i].id,
                          config->npft[GRASS]+config->npft[TREE],
                          config->npft[CROP],config);
         if(size==1)
           output->files[config->outputvars[i].id].isopen=!create_netcdf(&output->files[config->outputvars[i].id].fp.cdf,filename,
                         config->outnames[config->outputvars[i].id].var,
                         config->outnames[config->outputvars[i].id].standard_name,
                         config->outnames[config->outputvars[i].id].long_name,
                         config->outnames[config->outputvars[i].id].unit,
                         getoutputtype(config->outputvars[i].id,config->grid_type),
                         getnyear(config->outnames,config->outputvars[i].id),1,year,TRUE,
                         (config->outputvars[i].id==ADISCHARGE) ? output->index_all : output->index,config);
         else
           output->files[config->outputvars[i].id].isopen=!create_pft_netcdf(&output->files[config->outputvars[i].id].fp.cdf,filename,
                         config->outputvars[i].id,
                         config->npft[GRASS]+config->npft[TREE],
                         config->npft[CROP],
                         config->outnames[config->outputvars[i].id].var,
                         config->outnames[config->outputvars[i].id].standard_name,
                         config->outnames[config->outputvars[i].id].long_name,
                         config->outnames[config->outputvars[i].id].unit,
                         getoutputtype(config->outputvars[i].id,config->grid_type),
                         getnyear(config->outnames,config->outputvars[i].id),1,year,TRUE,
                             output->index,config);

        } /* of switch */
        free(filename);
      } /* of(isroot(*config)) */
#ifdef USE_MPI
      MPI_Bcast(&output->files[config->outputvars[i].id].isopen,1,MPI_INT,0,config->comm);
#endif
    }
} /* of 'openoutput_yearly */
