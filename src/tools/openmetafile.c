/**************************************************************************************/
/**                                                                                \n**/
/**               o  p  e  n  m  e  t  a  f  i  l  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions opens input desription file and opens specified binary           \n**/
/**     file. Description file has the following format:                           \n**/
/**                                                                                \n**/
/**     remark "metafile for LPJmL input"                                          \n**/
/**     file inputfile.bin                                                         \n**/
/**     firstyear 1901                                                             \n**/
/**     nyear 109                                                                  \n**/
/**     nbands 12                                                                  \n**/
/**     endian little                                                              \n**/
/**     firstcell 0                                                                \n**/
/**     ncell 67420                                                                \n**/
/**     scalar 0.1                                                                 \n**/
/**     cellsize 0.5 0.5                                                           \n**/
/**     offset 0                                                                   \n**/
/**     datatype short                                                             \n**/
/**                                                                                \n**/
/**     JSON file format is also supported:                                        \n**/
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

#ifdef USE_JSON

char *parse_json_metafile(LPJfile *lpjfile,   /**< pointer to JSON file */
                          char *s,            /**< first string of JSON file */
                          Header *header,     /**< pointer to file header */
                          Map **map,         /**< map from json file or NULL */
                          const char *map_name, /**< name of map or NULL */
                          Attr **attrs,       /**< pointer to array of attributes */
                          int *n_attr,        /**< size of array attribute */
                          String unit,        /**< unit of variable or NULL */
                          String descr,       /**< description of variable or NULL */
                          size_t *offset,     /**< offset in binary file */
                          Bool *swap,         /**< byte order has to be changed (TRUE/FALSE) */
                          Verbosity verbosity /**< verbosity level */
                         )                    /** \return filename of binary file or NULL */
{
  FILE *file;
  String filename;
  Bool endian;
  file=lpjfile->file.file;
  if(parse_json(file,lpjfile,s,verbosity))
  {
    closeconfig(lpjfile);
    lpjfile->file.file=file;
    return NULL;
  }
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
        lpjfile->file.file=file;
        return NULL;
      }
    }
    else
    {
     *attrs=NULL;
     *n_attr=0;
    }
  }
  if(unit!=NULL)
  {
    if(iskeydefined(lpjfile,"unit"))
    {
      if(fscanstring(lpjfile,unit,"unit",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    else
      unit[0]='\0';
  }
  if(descr!=NULL)
  {
    if(iskeydefined(lpjfile,"descr"))
    {
      if(fscanstring(lpjfile,descr,"descr",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    else
      descr[0]='\0';
  }
  if(header!=NULL)
  {
    if(iskeydefined(lpjfile,"firstcell"))
    {
      if(fscanint(lpjfile,&header->firstcell,"firstcell",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"ncell"))
    {
      if(fscanint(lpjfile,&header->ncell,"ncell",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"firstyear"))
    {
      if(fscanint(lpjfile,&header->firstyear,"firstyear",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"lastyear"))
    {
      if(fscanint(lpjfile,&header->nyear,"lastyear",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
      header->nyear-=header->firstyear-1;
    }
    if(iskeydefined(lpjfile,"nyear"))
    {
      if(fscanint(lpjfile,&header->nyear,"nyear",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"nstep"))
    {
      if(fscanint(lpjfile,&header->nstep,"nstep",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"timestep"))
    {
      if(fscanint(lpjfile,&header->timestep,"timestep",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
      if(header->timestep<1)
      {
        if(verbosity)
          fprintf(stderr,"ERROR221: Invalid time step %d, must be >0.\n",
                  header->timestep);
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"nbands"))
    {
      if(fscanint(lpjfile,&header->nbands,"nbands",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"order"))
    {
      if(fscankeywords(lpjfile,&header->order,"order",ordernames,4,FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
      header->order++;
    }
    if(iskeydefined(lpjfile,"scalar"))
    {
      if(fscanfloat(lpjfile,&header->scalar,"scalar",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"datatype"))
    {
      if(fscankeywords(lpjfile,(int *)&header->datatype,"datatype",typenames,5,FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"cellsize_lon"))
    {
      if(fscanfloat(lpjfile,&header->cellsize_lon,"cellsize_lon",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
    if(iskeydefined(lpjfile,"cellsize_lat"))
    {
      if(fscanfloat(lpjfile,&header->cellsize_lat,"cellsize_lat",FALSE,verbosity))
      {
        closeconfig(lpjfile);
        lpjfile->file.file=file;
        return NULL;
      }
    }
  } /* of if(header!=NULL) */
  if(iskeydefined(lpjfile,"offset"))
  {
    if(fscansize(lpjfile,offset,"offset",FALSE,verbosity))
    {
      closeconfig(lpjfile);
      lpjfile->file.file=file;
      return NULL;
    }
  }
  if(iskeydefined(lpjfile,"bigendian"))
  {
    if(fscanbool(lpjfile,&endian,"bigendian",FALSE,verbosity))
    {
      closeconfig(lpjfile);
      lpjfile->file.file=file;
      return NULL;
    }
    *swap=(endian) ? !bigendian() : bigendian();
  }
  if(fscanstring(lpjfile,filename,"filename",FALSE,verbosity))
  {
    closeconfig(lpjfile);
    lpjfile->file.file=file;
    return NULL;
  }
  closeconfig(lpjfile);
  lpjfile->file.file=file;
  return strdup(filename);
} /* of 'parse_json_metafile' */

#endif

FILE *openmetafile(Header *header,       /**< pointer to file header */
                   Map **map,            /**< map from json file or NULL */
                   const char *map_name, /**< name of map or NULL */
                   Attr **attrs,         /**< pointer to array of attributes */
                   int *n_attr,          /**< size of array attribute */
                   String unit,          /**< unit of variable or NULL */
                   String descr,         /**< description of variable or NULL */
                   Bool *swap,           /**< byte order has to be changed (TRUE/FALSE) */
                   size_t *offset,       /**< offset in binary file */
                   const char *filename, /**< file name */
                   Bool isout            /**< error output (TRUE/FALSE) */
                  )                      /** \return file pointer to open file or NULL */
{
  LPJfile file;
  FILE *data;
  String key,value;
  char *name,*path,*fullname;
  int index;
  file.isjson=FALSE;
  /* open description file */
  if((file.file.file=fopen(filename,"r"))==NULL)
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
  if(unit!=NULL)
    unit[0]='\0';
  if(descr!=NULL)
    descr[0]='\0';
  while(!fscantoken(file.file.file,key))
    if(key[0]=='{')
    {
#ifdef USE_JSON
      name=parse_json_metafile(&file,key,header,map,map_name,attrs,n_attr,unit,descr,offset,swap,isout ? ERR : NO_ERR);
      break;
#else
      if(isout)
        fprintf(stderr,"ERROR229: JSON format not supported for metafile '%s' in this version of LPJmL.\n",
                filename);
      free(name);
      fclose(file.file.file);
      return NULL;
#endif
    }
    else if(!strcmp(key,"firstcell"))
    {
      if(fscanint(&file,&header->firstcell,"firstcell",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"ncell"))
    {
      if(fscanint(&file,&header->ncell,"ncell",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"firstyear"))
    {
      if(fscanint(&file,&header->firstyear,"firstyear",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"lastyear"))
    {
      if(fscanint(&file,&header->nyear,"lastyear",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      header->nyear-=header->firstyear-1;
    }
    else if(!strcmp(key,"nyear"))
    {
      if(fscanint(&file,&header->nyear,"nyear",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"nbands"))
    {
      if(fscanint(&file,&header->nbands,"nbands",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"nstep"))
    {
      if(fscanint(&file,&header->nstep,"nstep",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"timestep"))
    {
      if(fscanint(&file,&header->timestep,"timestep",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      if(header->timestep<1)
      {
        if(isout)
          fprintf(stderr,"ERROR221: Invalid time step %d in '%s', must be >0.\n",
                  header->timestep,filename);
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"order"))
    {
      if(fscanstring(&file,value,"order",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("order");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      index=findstr(value,ordernames,4);
      if(index==NOT_FOUND)
      {
        if(isout)
          fprintf(stderr,"ERROR221: Invalid order '%s' in '%s'.\n",value,filename);
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      header->order=index+1;
    }
    else if(!strcmp(key,"offset"))
    {
      if(fscansize(&file,offset,"offset",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"scalar"))
    {
      if(fscanfloat(&file,&header->scalar,"scalar",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"cellsize"))
    {
      if(fscanfloat(&file,&header->cellsize_lon,"cellsize_lon",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      if(fscanfloat(&file,&header->cellsize_lat,"cellsize_lat",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"datatype"))
    {
      if(fscanstring(&file,value,"datatype",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("datatype");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      index=findstr(value,typenames,5);
      if(index==NOT_FOUND)
      {
        if(isout)
          fprintf(stderr,"ERROR221: Invalid datatype '%s' in '%s'.\n",value,filename);
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      header->datatype=(Type)index;
    }
    else if(!strcmp(key,"endian"))
    {
      if(fscanstring(&file,value,"endian",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("endian");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      if(!strcmp(value,"little"))
        *swap=bigendian();
      else if(!strcmp(value,"big"))
        *swap=!bigendian();
      else
      {
        if(isout)
          fprintf(stderr,"ERROR221: Invalid endianness '%s' in line %d of '%s'.\n",value,getlinecount(),filename);
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"file"))
    {
      if(fscanstring(&file,value,"file",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("file");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      name=strdup(value);
      if(name==NULL)
      {
        printallocerr("name");
        return NULL;
      }
    }
    else if(!strcmp(key,"remark"))
    {
      if(fscanstring(&file,value,"remark",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("remark");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(unit!=NULL && !strcmp(key,"unit"))
    {
      if(fscanstring(&file,unit,"unit",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("unit");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(descr!=NULL && !strcmp(key,"descr"))
    {
      if(fscanstring(&file,descr,"descr",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("descr");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else
    {
      if(isout)
      {
        fputs("ERROR222: Invalid key word '",stderr);
        fputprintable(stderr,key);
        fprintf(stderr,"' in line %d of '%s'.\n",getlinecount(),filename);
      }
      free(name);
      fclose(file.file.file);
      return NULL;
    }
  fclose(file.file.file);
  if(name==NULL)
  {
    if(isout)
      fprintf(stderr,"ERROR223: No filename specified in '%s'.\n",filename);
    return NULL;
  }
  if(name[0]=='^')
  {
    /* if filename starts with a '^' then path of description file is added to filename */
    path=getpath(filename);
    if(path==NULL)
    {
      printallocerr("path");
      free(name);
      return NULL;
    }
    fullname=malloc(strlen(path)+strlen(name)+1);
    if(fullname==NULL)
    {
      printallocerr("name");
      free(path);
      free(name);
      return NULL;
    }
    strcpy(fullname,path);
    strcat(fullname,"/");
    strcat(fullname,name+1);
    free(name);
    free(path);
    name=fullname;
  }
  else
  {
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
  }
  /* open data file */
  if((data=fopen(name,"rb"))==NULL  && isout)
    printfopenerr(name);
  /* check file size of binary file */
  if(isout && data!=NULL)
  {
    if((header->order==CELLINDEX  && getfilesizep(data)!=sizeof(int)*header->ncell+typesizes[header->datatype]*header->ncell*header->nbands*header->nstep*header->nyear+*offset) || (header->order!=CELLINDEX && getfilesizep(data)!=typesizes[header->datatype]*header->ncell*header->nbands*header->nyear*header->nstep+*offset))
      fprintf(stderr,"WARNING032: File size of '%s' does not match settings in description file '%s'.\n",name,filename);
  }
  free(name);
  return data;
} /* of 'openmetafile' */
