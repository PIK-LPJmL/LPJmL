/**************************************************************************************/
/**                                                                                \n**/
/**               o  p  e  n  m  e  t  a  f  i  l  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions opens JSON desription file and returns filename of specified     \n**/
/**     binary file. Description file has the following format:                    \n**/
/**                                                                                \n**/
/**     {                                                                          \n**/
/**       "filename" : "inputfile.bin",                                            \n**/
/**       "firstyear" : 1901,                                                      \n**/
/**       "nyear" :  109,                                                          \n**/
/**       "nbands" :  12,                                                          \n**/
/**       "bigendian" :  false,                                                    \n**/
/**       "firstcell" :  0,                                                        \n**/
/**       "ncell" :  67420,                                                        \n**/
/**       "scalar" :  0.1,                                                         \n**/
/**       "cellsize_lon" :  0.5,                                                   \n**/
/**       "cellsize_lat" :  0.5,                                                   \n**/
/**       "offset" :  0,                                                           \n**/
/**       "datatype" :  "short"                                                    \n**/
/**     }                                                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

char *ordernames[]={"cellyear","yearcell","cellindex","cellseq"};

void fprintmap(FILE *file,const Map *map)
{
  int i;
  fputc('[',file);
  foreachlistitem(i,map->list)
  {
    if(getmapitem(map,i)==NULL)
      fputs("null",file);
    else if(map->isfloat)
      fprintf(file,"%g",*((double *)getmapitem(map,i)));
    else
      fprintf(file,"\"%s\"",(char *)getmapitem(map,i));
    if(i<getmapsize(map)-1)
      fputc(',',file);
  }
  fputc(']',file);
} /* of 'fprintmap' */

Map *newmap(Bool isfloat,int size)
{
  Map *map;
  map=new(Map);
  if(map==NULL)
    return NULL;
  map->isfloat=isfloat;
  map->list=newlist(size);
  if(map->list==NULL)
  {
    free(map);
    return NULL;
  }
  return map;
} /* of 'newmap' */

void freemap(Map *map)
{
  int i;
  if(map!=NULL)
  {
    foreachlistitem(i,map->list)
      free(getmapitem(map,i));
    freelist(map->list);
  }
  free(map);
} /* of 'freemap' */

char *parse_json_metafile(FILE *file,         /**< pointer to JSON file */
                          Header *header,     /**< pointer to file header */
                          Metadata *metadata, /**< metadata information or NULL */
                          Filename *gridfile, /**< name of grid file or NULL */
                          Type *grid_type,    /**< datatype of grid or NULL */
                          int *filefmt,       /**< file format or NULL */
                          size_t *offset,     /**< offset in binary file */
                          Bool *swap,         /**< byte order has to be changed (TRUE/FALSE) */
                          Verbosity verbosity /**< verbosity level */
                         )                    /** \return filename of binary file or NULL */
{
  LPJfile *lpjfile,*item;
  const char *filename;
  char *ret;
  Bool endian;
  int format;
  lpjfile=parse_json(file,verbosity);
  if(lpjfile==NULL)
    return NULL;
  if(metadata!=NULL)
  {
    if(fscanmetadata(lpjfile,metadata,verbosity))
    {
      closeconfig(lpjfile);
      return NULL;
    }
  }
  if(iskeydefined(lpjfile,"format"))
  {
    if(fscankeywords(lpjfile,&format,"format",fmt,N_FMT,FALSE,verbosity))
    {
      closeconfig(lpjfile);
      return NULL;
    }
    if(format!=RAW && format!=CLM && format!=CLM2)
    {
      if(verbosity)
        fprintf(stderr,"ERROR229: Invalid format %s for input file, must be raw, clm or clm2.\n",
                fmt[format]);
      closeconfig(lpjfile);
      return NULL;
    }
    if(filefmt!=NULL)
      *filefmt=format;
  }
  if(gridfile!=NULL)
  {
    if(iskeydefined(lpjfile,"grid"))
    {
      item=fscanstruct(lpjfile,"grid",verbosity);
      if(item==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      filename=fscanstring(item,NULL,"filename",verbosity);
      if(filename==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      gridfile->name=strdup(filename);
      if(fscankeywords(item,&gridfile->fmt,"format",fmt,N_FMT,FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
      if(grid_type!=NULL)
      {
        if(iskeydefined(item,"datatype"))
        {
          if(fscankeywords(item,(int *)grid_type,"datatype",typenames,N_TYPES,FALSE,verbosity))
          {
            closeconfig(lpjfile);
            return NULL;
          }
          if(*grid_type==LPJ_BYTE || *grid_type==LPJ_INT)
          {
            if(verbosity)
              fprintf(stderr,"ERROR229: Invalid datatype %s for grid, must be short, float or double.\n",
                    typenames[*grid_type]);
            closeconfig(lpjfile);
            return NULL;
          }
        }
      }
    }
    else
      gridfile->name=NULL;
  }
  if(header!=NULL)
  {
    if(iskeydefined(lpjfile,"firstcell"))
    {
      if(fscanint(lpjfile,&header->firstcell,"firstcell",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"ncell"))
    {
      if(fscanint(lpjfile,&header->ncell,"ncell",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"firstyear"))
    {
      if(fscanint(lpjfile,&header->firstyear,"firstyear",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"lastyear"))
    {
      if(fscanint(lpjfile,&header->nyear,"lastyear",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
      header->nyear-=header->firstyear-1;
    }
    if(iskeydefined(lpjfile,"nyear"))
    {
      if(fscanint(lpjfile,&header->nyear,"nyear",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"nstep"))
    {
      if(fscanint(lpjfile,&header->nstep,"nstep",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"timestep"))
    {
      if(fscanint(lpjfile,&header->timestep,"timestep",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
      if(header->timestep<1)
      {
        if(verbosity)
          fprintf(stderr,"ERROR221: Invalid time step %d, must be >0.\n",
                  header->timestep);
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"nbands"))
    {
      if(fscanint(lpjfile,&header->nbands,"nbands",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"order"))
    {
      if(fscankeywords(lpjfile,&header->order,"order",ordernames,4,FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
      header->order++;
    }
    if(iskeydefined(lpjfile,"scalar"))
    {
      if(fscanfloat(lpjfile,&header->scalar,"scalar",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"datatype"))
    {
      if(fscankeywords(lpjfile,(int *)&header->datatype,"datatype",typenames,N_TYPES,FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"cellsize_lon"))
    {
      if(fscanfloat(lpjfile,&header->cellsize_lon,"cellsize_lon",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"cellsize_lat"))
    {
      if(fscanfloat(lpjfile,&header->cellsize_lat,"cellsize_lat",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
  } /* of if(header!=NULL) */
  if(iskeydefined(lpjfile,"offset"))
  {
    if(fscansize(lpjfile,offset,"offset",FALSE,verbosity))
    {
      closeconfig(lpjfile);
      return NULL;
    }
  }
  if(iskeydefined(lpjfile,"bigendian"))
  {
    if(fscanbool(lpjfile,&endian,"bigendian",FALSE,verbosity))
    {
      closeconfig(lpjfile);
      return NULL;
    }
    *swap=(endian) ? !bigendian() : bigendian();
  }
  filename=fscanstring(lpjfile,NULL,"filename",verbosity);
  if(filename==NULL)
  {
    closeconfig(lpjfile);
    return NULL;
  }
  ret=strdup(filename);
  closeconfig(lpjfile);
  return ret;
} /* of 'parse_json_metafile' */


FILE *openmetafile(Header *header,       /**< pointer to file header */
                   Metadata *metadata,   /**< metadata information or NULL */
                   Filename *gridfile,   /**< name of grid file or NULL */
                   Type *grid_type,      /**< datatype of grid or NULL */
                   int *filefmt,         /**< file format or NULL */
                   Bool *swap,           /**< byte order has to be changed (TRUE/FALSE) */
                   size_t *offset,       /**< offset in binary file */
                   const char *filename, /**< file name */
                   Bool isout            /**< error output (TRUE/FALSE) */
                  )                      /** \return file pointer to open file or NULL */
{
  FILE *file;
  char *name,*path,*fullname;
  /* open description file */
  if((file=fopen(filename,"r"))==NULL)
  {
    if(isout)
      printfopenerr(filename);
    return NULL;
  }
  initscan(filename);
  /* set default values for swap and offset */
  *swap=FALSE;
  *offset=0;
  name=NULL;
  name=parse_json_metafile(file,header,metadata,gridfile,grid_type,filefmt,offset,swap,isout ? ERR : NO_ERR);
  fclose(file);
  if(name==NULL)
  {
    if(isout)
      fprintf(stderr,"ERROR223: Cannot parse JSON file '%s'.\n",filename);
    freemetadata(metadata);
    return NULL;
  }
  path=getpath(filename);
  fullname=addpath(name,path);
  if(fullname==NULL)
  {
    printallocerr("name");
    free(path);
    free(name);
    freemetadata(metadata);
    return NULL;
  }
  free(name);
  name=fullname;
  if(gridfile!=NULL && gridfile->name!=NULL)
  {
    fullname=addpath(gridfile->name,path);
    if(fullname==NULL)
    {
      printallocerr("name");
      free(path);
      free(name);
      freemetadata(metadata);
      return NULL;
    }
    free(gridfile->name);
    gridfile->name=fullname;
  }
  free(path);
  /* open data file */
  file=fopen(name,"rb");
  /* check file size of binary file */
  if(file==NULL)
  {
    if(isout)
      printfopenerr(name);
    freemetadata(metadata);
  }
  else
  {
    if((header->order==CELLINDEX  && getfilesizep(file)!=sizeof(int)*header->ncell+typesizes[header->datatype]*header->ncell*header->nbands*header->nstep*header->nyear+*offset) || (header->order!=CELLINDEX && getfilesizep(file)!=typesizes[header->datatype]*header->ncell*header->nbands*header->nyear*header->nstep+*offset))
    {
      if(getfilesizep(file)==0)
      {
        freemetadata(metadata);
        fclose(file);
        file=NULL;
        if(isout)
          fprintf(stderr,"ERROR242: File '%s' is empty.\n",name);
      }
      else if(isout)
        fprintf(stderr,"WARNING032: File size of '%s' does not match settings in JSON metafile '%s'.\n",name,filename);
    }
  }
  free(name);
  return file;
} /* of 'openmetafile' */
