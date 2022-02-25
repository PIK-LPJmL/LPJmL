/**************************************************************************************/
/**                                                                                \n**/
/**                 e  x  t  f  l  o  w  .  c                                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJ, derived from the Fortran/C++ version              \n**/
/**                                                                                \n**/
/**     Functions for reading external flux data for river routing                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct extflow
{
  FILE *file;
  int *index;
  Bool swap;
  int firstyear;
  int nyear;
  int ncell;
  float scalar;
  Type datatype;
  long long size,offset;
};               /* definition of opaque datatype extflow */

Extflow initextflow(const Config *config /**< LPJmL configuration */
                   )                     /** \return pointer to extflow data or NULL on errir */
{
  Extflow extflow;
  Header header;
  int version,*index,start,i;
  size_t offset;
  char *name;
  extflow=new(struct extflow);
  if(extflow==NULL)
    return NULL;
  if(config->extflow_filename.fmt==META)
  {
    version=CLM_MAX_VERSION+1;
    /* set default values for header */
    header.order=CELLINDEX;
    header.firstyear=config->firstyear;
    header.nyear=config->lastyear-config->firstyear+1;
    header.firstcell=0;
    header.ncell=0;
    header.nbands=NDAYYEAR;
    header.nstep=1;
    header.scalar=1;
    header.datatype=LPJ_FLOAT;
    header.cellsize_lon=(float)config->resolution.lon;
    header.cellsize_lat=(float)config->resolution.lat;
    /* open description file */
    extflow->file=openmetafile(&header,&extflow->swap,&offset,config->extflow_filename.name,isroot(*config));
    if(extflow->file==NULL)
    {
      free(extflow);
      return NULL;
    }
  }
  else
  {
    extflow->file=fopen(config->extflow_filename.name,"rb");
    if(extflow->file==NULL)
    {
      if(isroot(*config))
        printfopenerr(config->extflow_filename.name);
      free(extflow);
      return NULL;
    }
    offset=0;
    version=(config->extflow_filename.fmt==CLM) ? READ_VERSION : 2;
    if(freadheader(extflow->file,&header,&extflow->swap,LPJEXTFLOW_HEADER,&version,isroot(*config)))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR154: Invalid header in '%s'.\n",config->extflow_filename.name);
      fclose(extflow->file);
      free(extflow);
      return NULL;
    }
  }
  if(header.order!=CELLINDEX)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Invalid order in '%s', must be CELLINDEX.\n",config->extflow_filename.name);
    fclose(extflow->file);
    free(extflow);
    return NULL;
  }
  if(version<3)
    extflow->datatype=LPJ_FLOAT;
  else
    extflow->datatype=header.datatype;
  if(version>1 && header.cellsize_lon!=config->resolution.lon)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Cell size %g different from %g in '%s'.\n",
              header.cellsize_lon,config->resolution.lon,config->extflow_filename.name);
    fclose(extflow->file);
    free(extflow);
    return NULL;
  }
  if(version>1 && header.cellsize_lat!=config->resolution.lat)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Cell size %g different from %g in '%s'.\n",
              header.cellsize_lat,config->resolution.lat,config->extflow_filename.name);
    fclose(extflow->file);
    free(extflow);
    return NULL;
  }
  if(version==4)
  {
    if(header.nbands>1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR154: Number of bands=%d in '%s' is not 1.\n",
                header.nbands,config->extflow_filename.name);
      fclose(extflow->file);
      free(extflow);
      return NULL;
    }
    header.nbands=header.nstep;
  }
  if(isroot(*config) && config->extflow_filename.fmt!=META && getfilesizep(extflow->file)!=sizeof(int)*header.ncell+headersize(LPJEXTFLOW_HEADER,version)+typesizes[extflow->datatype]*header.ncell*header.nbands*header.nyear)
    fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->extflow_filename.name);
  if(header.ncell==0)
  {
    extflow->ncell=0;
    fclose(extflow->file);
    return extflow;
  }
  if(header.nbands!=NDAYYEAR)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Invalid number of bands=%d in '%s', must be 365.\n",
              header.nbands,config->extflow_filename.name);
    fclose(extflow->file);
    free(extflow);
    return NULL;
  }
  extflow->firstyear=header.firstyear;
  index=newvec(int,header.ncell);
  if(index==NULL)
  {
    printallocerr("index");
    fclose(extflow->file);
    free(extflow);
    return NULL;
  }
  if(config->extflow_filename.fmt==META)
    fseek(extflow->file,offset,SEEK_SET);
  if(freadint(index,header.ncell,extflow->swap,extflow->file)!=header.ncell)
  {
    name=getrealfilename(&config->extflow_filename);
    fprintf(stderr,"ERROR131: Cannot read cell index array from '%s'.\n",name);
    free(name);
    free(index);
    fclose(extflow->file);
    free(extflow);
    return NULL;
  }
  /* check for correct order and boundaries in index vector */
  for(start=0;start<header.ncell;start++)
  {
    if(start<header.ncell-1 && index[start]>=index[start+1])
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->extflow_filename);
        fprintf(stderr,"ERROR141: Cell index array in '%s' is not in ascending order at index=%d: %d>%d.\n",
                name,start,index[start],index[start+1]);
        free(name);
      }
      free(index);
      fclose(extflow->file);
      free(extflow);
      return NULL;
    }
    else if(index[start]<0 || index[start]>=config->nall)
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->extflow_filename);
        fprintf(stderr,"ERROR141: Cell index array in '%s' is out of bounds at index=%d: %d.\n",
                name,start,index[start]);
        free(name);
      }
      free(index);
      fclose(extflow->file);
      free(extflow);
      return NULL;
    }
  }
  for(start=0;start<header.ncell;start++)
    if(index[start]>=config->startgrid)
      break;
  if(start==header.ncell)
  {
    fclose(extflow->file);
    extflow->ncell=0;
  }
  else
  {
    for(i=start;i<header.ncell;i++)
      if(index[i]>config->startgrid+config->ngridcell-1)
        break;
    if(i==start)
      extflow->ncell=0;
    else
    {
      extflow->offset=headersize(LPJEXTFLOW_HEADER,version)+offset+sizeof(int)*header.ncell+typesizes[extflow->datatype]*start;
      extflow->ncell=i-start;
      extflow->size=typesizes[extflow->datatype]*header.ncell;
      extflow->scalar=header.scalar;
      extflow->index=newvec(int,extflow->ncell);
      if(extflow->index==NULL)
      {
        printallocerr("index");
        free(index);
        fclose(extflow->file);
        free(extflow);
        return NULL;
      }
      for(i=0;i<extflow->ncell;i++)
      {
        extflow->index[i]=index[i+start]-config->startgrid;
#ifdef DEBUG
        printf("inflow cell %d\n",extflow->index[i]);
#endif
      }
    }
    extflow->nyear=header.nyear;
  }
  free(index);
  return extflow;
} /* of 'initextflow' */

Bool getextflow(Extflow extflow, /**< Pointer to extflow data */
                Cell grid[],     /**< cell grid */
                int day,         /**< day of year (0..364) */
                int year         /**< year of extflow data (AD) */
               )                 /** \return TRUE on error */
{
  int i;
  Real *vec;
  if(extflow->ncell>0)
  {
    year-=extflow->firstyear;
    if(year<0)
      year=0;
    if(year>=extflow->nyear)
      year=extflow->nyear-1;
    if(fseek(extflow->file,extflow->offset+(NDAYYEAR*year+day)*extflow->size,SEEK_SET))
    {
      fprintf(stderr,"ERROR150: Cannot seek file to year %d in getextflow().\n",year);
      return TRUE;
    }
    vec=newvec(Real,extflow->ncell);
    if(vec==NULL)
    {
      printallocerr("vec");
      return TRUE;
    }
    if(readrealvec(extflow->file,vec,0,extflow->scalar,extflow->ncell,extflow->swap,extflow->datatype))
    {
      fprintf(stderr,"ERROR151: Cannot read extflow for year %d.\n",year+extflow->firstyear);
      free(vec);
      return TRUE;
    }
    for(i=0;i<extflow->ncell;i++)
      grid[extflow->index[i]].discharge.fin_ext=vec[i];
    free(vec);
  }
  return FALSE;
} /* of 'getextflow' */

void freeextflow(Extflow extflow /**< pointer to extflow data */
                )
{
  if(extflow!=NULL)
  {
    if(extflow->ncell>0)
    {
      fclose(extflow->file);
      free(extflow->index);
    }
    free(extflow);
  }
} /* of 'freeextflow' */
