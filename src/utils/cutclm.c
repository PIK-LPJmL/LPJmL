/**************************************************************************************/
/**                                                                                \n**/
/**                 c  u  t  c  l  m  .  c                                         \n**/
/**                                                                                \n**/
/**     Program cuts input from CLM file with new start/end year                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include <sys/stat.h>

#define USAGE "Usage: cutclm [-longheader] [-metafile] [-json] [-end] year src.clm dst.clm\n"

int main(int argc,char **argv)
{
  FILE *file,*out;
  char *endptr,*arglist,*out_json;
  Header header;
  Metadata metadata;;
  Bool swap,istail,ismeta=FALSE,isjson=FALSE;
  int version,year,index,idata,i,format;
  long long size,ldata,filesize;
  short sdata;
  Byte bdata;
  String id;
  Filename grid_name;
  Type grid_type;
  size_t offset;
  istail=FALSE;
  version=READ_VERSION;
  grid_name.name=NULL;
  for(index=1;index<argc;index++)
  {
    if(argv[index][0]=='-')
    {
      if(!strcmp(argv[index],"-end"))
        istail=TRUE;
      else if(!strcmp(argv[index],"-longheader"))
        version=2;
      else if(!strcmp(argv[index],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[index],"-metafile"))
        ismeta=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[index]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  }
  if(argc<index+3)
  {
    fprintf(stderr,"Argument(s) missing.\n"
            USAGE);
    return EXIT_FAILURE;
  }
  year=strtol(argv[index],&endptr,10);
  if(*endptr!='\0')
  {
    fprintf(stderr,"Invalid number '%s' for year.\n",argv[index]);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[index+1],argv[index+2]))
  {
    fputs("Error: source and destination filename are the same.\n",stderr);
    return EXIT_FAILURE;
  }
  format=CLM;
  initmetadata(&metadata,NULL);
  if(ismeta)
  {
    /* set default values */
    header.datatype=LPJ_SHORT;
    header.timestep=1;
    header.nbands=1;
    header.nstep=1;
    header.order=CELLYEAR;
    header.firstcell=0;
    header.firstyear=1901;
    header.cellsize_lon=header.cellsize_lat=0.5;
    header.ncell=1;
    header.nyear=1;
    version=CLM_MAX_VERSION;
    grid_type=LPJ_SHORT;
    file=openmetafile(&header,&metadata,&grid_name,&grid_type,&format,&swap,&offset,argv[index+1],TRUE);
    if(file==NULL)
      return EXIT_FAILURE;
    if(format==CLM)
    {
      if(freadheaderid(file,id,TRUE))
      {
        fclose(file);
        freemetadata(&metadata);
        free(grid_name.name);
        return EXIT_FAILURE;
      }
    }
    fseek(file,offset,SEEK_SET);
    size=typesizes[header.datatype];
  }
  else
  {
    file=fopen(argv[index+1],"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[index+1],strerror(errno));
      return EXIT_FAILURE;
    }
    if(freadanyheader(file,&header,&swap,id,&version,TRUE))
    {
      fprintf(stderr,"Error reading header in '%s'.\n",
              argv[index+1]);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version>CLM_MAX_VERSION)
    {
      fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
              version,argv[index+1],CLM_MAX_VERSION+1);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version>=3)
      size=typesizes[header.datatype];
    else
    {
      filesize=getfilesizep(file);
      size=(filesize-headersize(id,version))/header.ncell/header.nbands/header.nyear/header.nstep;
      printf("Size of data: %lld bytes\n",size);
      if(size!=1 && size!=2 && size!=4 && size!=8)
      {
        fprintf(stderr,"Invalid size of data=%lld.\n",size);
        fclose(file);
        return EXIT_FAILURE;
      }
      if((filesize-headersize(id,version)) % ((long long)header.ncell*header.nbands*header.nyear*header.nstep)!=0)
        fprintf(stderr,"Warning: file size of '%s' is not multiple of ncell*nbands*nyear.\n",argv[index+1]);
    }
  }
  if(year<header.firstyear || year>=header.firstyear+header.nyear)
  {
    fprintf(stderr,"Invalid year %d, must be in [%d,%d].\n",year,header.firstyear,header.firstyear+header.nyear-1);
    fclose(file);
    freemetadata(&metadata);
    free(grid_name.name);
    return EXIT_FAILURE;
  }
  out=fopen(argv[index+2],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[index+2],strerror(errno));
    fclose(file);
    freemetadata(&metadata);
    free(grid_name.name);
    return EXIT_FAILURE;
  }
  if(istail)
    header.nyear=year-header.firstyear+1;
  else
  {
    if(fseek(file,size*(year-header.firstyear)*header.nbands*header.nstep*header.ncell,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to year %d.\n",argv[index+1],year);
      fclose(file);
      fclose(out);
      freemetadata(&metadata);
      free(grid_name.name);
      return EXIT_FAILURE;
    }
    header.nyear-=year-header.firstyear;
    header.firstyear=year;
  }
  if(format==CLM)
    fwriteheader(out,&header,id,version);
  switch(size)
  {
    case 1:
      for(i=0;i<header.nyear*header.nbands*header.nstep*header.ncell;i++)
      {
        if(fread(&bdata,1,1,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          fclose(file);
          fclose(out);
          freemetadata(&metadata);
          free(grid_name.name);
          return EXIT_FAILURE;
        }
        fwrite(&bdata,1,1,out);
      }
      break;
    case 2:
      for(i=0;i<header.nyear*header.nbands*header.nstep*header.ncell;i++)
      {
        if(freadshort(&sdata,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          fclose(file);
          fclose(out);
          freemetadata(&metadata);
          free(grid_name.name);
          return EXIT_FAILURE;
        }
        fwrite(&sdata,sizeof(short),1,out);
      }
      break;
    case 4:
      for(i=0;i<header.nyear*header.nbands*header.nstep*header.ncell;i++)
      {
        if(freadint(&idata,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          fclose(file);
          fclose(out);
          freemetadata(&metadata);
          free(grid_name.name);
          return EXIT_FAILURE;
        }
        fwrite(&idata,sizeof(int),1,out);
      }
      break;
    case 8:
      for(i=0;i<header.nyear*header.nbands*header.nstep*header.ncell;i++)
      {
        if(freadlong(&ldata,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          fclose(file);
          fclose(out);
          freemetadata(&metadata);
          free(grid_name.name);
          return EXIT_FAILURE;
        }
        fwrite(&ldata,sizeof(long long),1,out);
      }
      break;
  } /* of switch */
  fclose(file);
  fclose(out);
  if(ismeta || isjson)
  {
    out_json=malloc(strlen(argv[index+2])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      freemetadata(&metadata);
      free(grid_name.name);
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[index+2]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    if(arglist==NULL)
    {
      printallocerr("arglist");
      free(out_json);
      freemetadata(&metadata);
      free(grid_name.name);
      return EXIT_FAILURE;
    }
    out=fopen(out_json,"w");
    if(out==NULL)
    {
      printfcreateerr(out_json);
      free(out_json);
      free(arglist);
      freemetadata(&metadata);
      free(grid_name.name);
      return EXIT_FAILURE;
    }
    fprintjson(out,argv[index+2],NULL,arglist,&header,&metadata,(grid_name.name==NULL) ? NULL : &grid_name,grid_type,format,id,FALSE,version);
    free(out_json);
    free(arglist);
    fclose(out);
  }
  freemetadata(&metadata);
  free(grid_name.name);
  return EXIT_SUCCESS;
} /* of 'main' */
