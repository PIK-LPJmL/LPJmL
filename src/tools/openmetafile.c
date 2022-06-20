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

void fprintmap(FILE *file,List *map)
{
  int i;
  fputc('[',file);
  foreachlistitem(i,map)
  {
    if(getlistitem(map,i)==NULL)
      fputs("null",file);
    else
      fprintf(file,"\"%s\"",(char *)getlistitem(map,i));
    if(i<getlistlen(map)-1)
      fputc(',',file);
  }
  fputc(']',file);
} /* of 'fprintmap' */

void freemap(List *map)
{
  int i;
  foreachlistitem(i,map)
    free(getlistitem(map,i));
  freelist(map);
} /* of 'freemap' */

char *parse_json_metafile(FILE *file,         /**< pointer to JSON file */
                          Header *header,     /**< pointer to file header */
                          List **map,         /**< map from json file or NULL */
                          const char *map_name, /**< name of map or NULL */
                          size_t *offset,     /**< offset in binary file */
                          Bool *swap,         /**< byte order has to be changed (TRUE/FALSE) */
                          Verbosity verbosity /**< verbosity level */
                         )                    /** \return filename of binary file or NULL */
{
  LPJfile *lpjfile;
  const char *filename;
  char *ret;
  Bool endian;
  lpjfile=parse_json(file,verbosity);
  if(lpjfile==NULL)
    return NULL;
  if(map!=NULL)
  {
    if(iskeydefined(lpjfile,(map_name==NULL) ? MAP_NAME : map_name))
      *map=fscanstringarray(lpjfile,(map_name==NULL) ? MAP_NAME : map_name,verbosity);
    else
      *map=NULL;
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
                   List **map,           /**< map from json file or NULL */
                   const char *map_name, /**< name of map or NULL */
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
  name=parse_json_metafile(file,header,map,map_name,offset,swap,isout ? ERR : NO_ERR);
  fclose(file);
  if(name==NULL)
  {
    if(isout)
      fprintf(stderr,"ERROR223: No filename specified in '%s'.\n",filename);
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
