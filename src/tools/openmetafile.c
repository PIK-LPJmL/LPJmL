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

const char *ordernames[]={"cellyear","yearcell","cellindex","cellseq"};

void fprintmap(FILE *file,Map *map)
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
                          Map **map,         /**< map from json file or NULL */
                          const char *map_name, /**< name of map or NULL */
                          Attr **attrs,       /**< pointer to array of attributes */
                          int *n_attr,        /**< size of array attribute */
                          char **source,
                          char **history,
                          char **variable,    /**< name of variable or NULL */
                          char **unit,        /**< unit of variable or NULL */
                          char **standard_name, /**< standard name of variable or NULL */
                          char **long_name,   /**< long name of variable or NULL */
                          Filename *gridfile, /**< name of grid file or NULL */
                          Type *grid_type,    /**< datatype of grid or NULL */
                          size_t *offset,     /**< offset in binary file */
                          Bool *swap,         /**< byte order has to be changed (TRUE/FALSE) */
                          Verbosity verbosity /**< verbosity level */
                         )                    /** \return filename of binary file or NULL */
{
  LPJfile *lpjfile,*item;
  const char *filename,*val;
  char *ret;
  Bool endian;
  int format;
  lpjfile=parse_json(file,verbosity);
  if(lpjfile==NULL)
    return NULL;
  if(map!=NULL)
  {
    if(iskeydefined(lpjfile,(map_name==NULL) ? MAP_NAME : map_name))
      *map=fscanmap(lpjfile,(map_name==NULL) ? MAP_NAME : map_name,verbosity);
    else
      *map=NULL;
  }
  if(attrs!=NULL)
  {
    if(iskeydefined(lpjfile,"global_attrs"))
    {
      if(fscanattrs(lpjfile,attrs,n_attr,"global_attrs",verbosity))
      {
        closeconfig(lpjfile);
        return NULL;
      }
    }
    else
    {
     *attrs=NULL;
     *n_attr=0;
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
                format[fmt]);
      closeconfig(lpjfile);
      return NULL;
    }
  }
  if(variable!=NULL)
  {
    if(iskeydefined(lpjfile,"variable"))
    {
      val=fscanstring(lpjfile,NULL,"variable",verbosity);
      if(val==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      *variable=strdup(val);
    }
    else
      *variable=NULL;
  }
  if(source!=NULL)
  {
    if(iskeydefined(lpjfile,"source"))
    {
      val=fscanstring(lpjfile,NULL,"source",verbosity);
      if(val==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      *source=strdup(val);
    }
    else
      *source=NULL;
  }
  if(history!=NULL)
  {
    if(iskeydefined(lpjfile,"history"))
    {
      val=fscanstring(lpjfile,NULL,"history",verbosity);
      if(val==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      *history=strdup(val);
    }
    else
      *history=NULL;
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
          if(fscankeywords(item,(int *)grid_type,"datatype",typenames,5,FALSE,verbosity))
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
  if(unit!=NULL)
  {
    if(iskeydefined(lpjfile,"unit"))
    {
      val=fscanstring(lpjfile,NULL,"unit",verbosity);
      if(val==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      *unit=strdup(val);
    }
    else
      *unit=NULL;
  }
  if(standard_name!=NULL)
  {
    if(iskeydefined(lpjfile,"standard_name"))
    {
      val=fscanstring(lpjfile,NULL,"standard_name",verbosity);
      if(val==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      *standard_name=strdup(val);
    }
    else
      *standard_name=NULL;
  }
  if(long_name!=NULL)
  {
    if(iskeydefined(lpjfile,"descr"))
    {
      val=fscanstring(lpjfile,NULL,"descr",verbosity);
      if(val==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      *long_name=strdup(val);
    }
    else if(iskeydefined(lpjfile,"long_name"))
    {
      val=fscanstring(lpjfile,NULL,"long_name",verbosity);
      if(val==NULL)
      {
        closeconfig(lpjfile);
        return NULL;
      }
      *long_name=strdup(val);
    }
    else
      *long_name=NULL;
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
      if(fscankeywords(lpjfile,(int *)&header->datatype,"datatype",typenames,5,FALSE,verbosity))
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
                   Map **map,            /**< map from json file or NULL */
                   const char *map_name, /**< name of map or NULL */
                   Attr **attrs,         /**< pointer to array of attributes */
                   int *n_attr,          /**< size of array attribute */
                   char **source,
                   char **history,
                   char **variable,      /**< name of variable or NULL */
                   char **unit,          /**< unit of variable or NULL */
                   char **standard_name, /**< standard name of variable or NULL */
                   char **long_name,     /**< long name of variable or NULL */
                   Filename *gridfile,   /**< name of grid file or NULL */
                   Type *grid_type,      /**< datatype of grid or NULL */
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
  if(map!=NULL)
    *map=NULL;
  name=parse_json_metafile(file,header,map,map_name,attrs,n_attr,source,history,variable,unit,standard_name,long_name,gridfile,grid_type,offset,swap,isout ? ERR : NO_ERR);
  fclose(file);
  if(name==NULL)
  {
    if(isout)
      fprintf(stderr,"ERROR223: Cannot parse JSON file '%s'.\n",filename);
    return NULL;
  }
  path=getpath(filename);
  fullname=addpath(name,path);
  if(fullname==NULL)
  {
    printallocerr("name");
    free(path);
    free(name);
    return NULL;
  }
  free(name);
  free(path);
  name=fullname;
  /* open data file */
  if((file=fopen(name,"rb"))==NULL  && isout)
    printfopenerr(name);
  /* check file size of binary file */
  if(isout && file!=NULL)
  {
    if((header->order==CELLINDEX  && getfilesizep(file)!=sizeof(int)*header->ncell+typesizes[header->datatype]*header->ncell*header->nbands*header->nstep*header->nyear+*offset) || (header->order!=CELLINDEX && getfilesizep(file)!=typesizes[header->datatype]*header->ncell*header->nbands*header->nyear*header->nstep+*offset))
      fprintf(stderr,"WARNING032: File size of '%s' does not match settings in description file '%s'.\n",name,filename);
  }
  free(name);
  return file;
} /* of 'openmetafile' */
