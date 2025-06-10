/**************************************************************************************/
/**                                                                                \n**/
/**                    r  e  g  r  i  d  c  l  m  .  c                             \n**/
/**                                                                                \n**/
/**     CLM data is regridded to new grid file                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-v] [-metafile] [-size4] [-byte] [-search] [-zero] [-longheader] [-json] [grid_old.clm] grid_new.clm data_old.clm data_new.clm\n"

int main(int argc,char **argv)
{
  FILE *file,*data_file;
  Header header,header2;
  Coord *c,*c2;
  Real dist_min;
  Bool swap,isint,issearch,iszero,isjson,ismeta,isbyte;
  short *data=NULL;
  int *zero=NULL;
  int *idata=NULL;
  Byte *bdata=NULL;
  long long size;
  Coord res,res2;
  Coordfile grid;
  int i,j,*index,data_version,setversion,ngrid,ngrid2,iarg,format;
  int index_datafile,index_gridfile;
  float lon,lat,*fzero;
  char *arglist,*out_json;
  const char *progname;
  size_t offset;
  String id;
  Filename filename,grid_name;
  Type grid_type;
  Map *map=NULL;
  Attr *global_attrs=NULL;
  char *var_name=NULL,*var_units=NULL,*var_long_name=NULL,*var_standard_name=NULL;
  char *source=NULL,*history=NULL;
  char *path;
  char *map_name;
  map_name=MAP_NAME;
  int n_global=0;
  isint=issearch=iszero=isjson=ismeta=isbyte=FALSE;
  setversion=READ_VERSION;
  progname=strippath(argv[0]);
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version"))
      {
        puts(getversion());
        return EXIT_SUCCESS;
      }
      if(!strcmp(argv[iarg],"-size4"))
        isint=TRUE;
      else if(!strcmp(argv[iarg],"-byte"))
        isbyte=TRUE;
      else if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
      else if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[iarg],"-search"))
        issearch=TRUE;
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-zero"))
        iszero=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<(ismeta ? 3 : 4)+iarg)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            USAGE,progname);
    return EXIT_FAILURE;
  }
  format=CLM;
  if(ismeta)
  {
    index_datafile=(argc==iarg+3) ? iarg+1 : iarg+2;
    index_gridfile=(argc==iarg+3) ? iarg : iarg+1;
    header.cellsize_lon=header.cellsize_lat=0.5;
    header.firstyear=1901;
    header.firstcell=0;
    header.nyear=1;
    header.nbands=1;
    header.nstep=12;
    header.datatype=LPJ_SHORT;
    header.order=CELLYEAR;
    data_version=CLM_MAX_VERSION;
    data_file=openmetafile(&header,&map,map_name,&global_attrs,&n_global,&source,&history,&var_name,&var_units,&var_standard_name,&var_long_name,&grid_name,NULL,&format,&swap,&offset,argv[index_datafile],TRUE);
    if(data_file==NULL)
      return EXIT_FAILURE;
    if(format==CLM)
    {
      if(freadheaderid(data_file,id,TRUE))
      {
        fclose(data_file);
        return EXIT_FAILURE;
      }
    }
    if(fseek(data_file,offset,SEEK_SET))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",argv[index_datafile],offset);
      fclose(data_file);
      return EXIT_FAILURE;
    }
    if(argc==iarg+4)
    {
      filename.name=strdup(argv[iarg]);
      check(filename.name);
      filename.fmt=(setversion==2) ? CLM2 : CLM;
    }
    else
    {
      if(grid_name.name==NULL)
      {
        fprintf(stderr,"Error: grid filename must be specified in '%s' metafile.\n",argv[index_datafile]);
        return EXIT_FAILURE;
      }
      path=getpath(argv[index_datafile]);
      filename.name=addpath(grid_name.name,path);
      filename.fmt=grid_name.fmt;
      if(filename.name==NULL)
      {
        printallocerr("name");
        return EXIT_FAILURE;
      }
      free(grid_name.name);
      free(path);
    }
  }
  else
  {
    index_datafile=iarg+2;
    index_gridfile=iarg+1;
    data_file=fopen(argv[iarg+2],"rb");
    if(data_file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
      return EXIT_FAILURE;
    }
    data_version=setversion;
    if(freadanyheader(data_file,&header,&swap,id,&data_version,TRUE))
    {
      fprintf(stderr,"Error reading header in '%s'.\n",argv[iarg+2]);
      return EXIT_FAILURE;
    }
    if(data_version>CLM_MAX_VERSION)
    {
      fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
              data_version,argv[iarg+2],CLM_MAX_VERSION+1);
        return EXIT_FAILURE;
    }
    filename.name=strdup(argv[iarg]);
    check(filename.name);
    filename.fmt=(setversion==2) ? CLM2 : CLM;
  }
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
    return EXIT_FAILURE;
  ngrid=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  res.lon=lon;
  res.lat=lat;
  c=newvec(Coord,ngrid);
  if(c==NULL)
  {
    closecoord(grid);
    printallocerr("c");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
  {
    if(readcoord(grid,c+i,&res))
    {
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,filename.name);
      return EXIT_FAILURE;
    }
    //printf("c:%g %g\n",c[i].lon,c[i].lat);
  }
  closecoord(grid);
  free(filename.name);
  filename.name=strdup(argv[index_gridfile]);
  check(filename.name);
  filename.fmt=(setversion==2) ? CLM2 : CLM;
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
    return EXIT_FAILURE;
  grid_type=getcoordtype(grid);
  ngrid2=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  res2.lon=lon;
  res2.lat=lat;
  if(res.lon!=res2.lon)
    fprintf(stderr,"Warning: longitudinal resolution %g in '%s' differs from %g in '%s.\n",res2.lon,argv[index_gridfile],res.lon,argv[index_datafile]);
  if(res.lat!=res2.lat)
    fprintf(stderr,"Warning: latitudinal resolution %g in '%s' differs from %g in '%s.\n",res2.lat,argv[index_gridfile],res.lat,argv[index_datafile]);
  c2=newvec(Coord,ngrid2);
  if(c2==NULL)
  {
    closecoord(grid);
    printallocerr("c2");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    if(readcoord(grid,c2+i,&res2))
    {
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[index_gridfile]);
      return EXIT_FAILURE;
    }
    //printf("c2:%g %g\n",c2[i].lon,c2[i].lat);
  }
  closecoord(grid);
  if(res.lon!=header.cellsize_lon)
    fprintf(stderr,"Warning: longitudinal resolution %g in '%s' differs from %g in '%s.\n",header.cellsize_lon,argv[index_datafile],res.lon,filename.name);
  if(res.lat!=header.cellsize_lat)
    fprintf(stderr,"Warning: latitudinal resolution %g in '%s' differs from %g in '%s.\n",header.cellsize_lat,argv[index_datafile],res.lat,filename.name);
  if(header.nyear<=0)
  {
    fprintf(stderr,"Invalid nyear=%d, set to one.\n",header.nyear);
    header.nyear=1;
  }
  if(header.ncell!=ngrid)
  {
    fprintf(stderr,"Invalid number of cells %d in '%s', not %d.\n",
            header.ncell,argv[index_datafile],ngrid);
    return EXIT_FAILURE;
  }
  if(ismeta)
    size=getfilesizep(data_file)-offset;
  else
    size=getfilesizep(data_file)-headersize(id,data_version);
  if(data_version>=3)
  {
    isint=header.datatype==LPJ_INT || header.datatype==LPJ_FLOAT;
    isbyte=header.datatype==LPJ_BYTE;
  }
  if(iszero)
  {
    zero=newvec(int,header.nbands*header.nstep);
    if(header.datatype==LPJ_FLOAT)
    {
      fzero=(float *)zero;
      for(i=0;i<header.nbands*header.nstep;i++)
        fzero[i]=0;
    }
    else
      for(i=0;i<header.nbands*header.nstep;i++)
        zero[i]=0;
  }
  if(isint)
  {
    idata=newvec(int,(long long)header.ncell*header.nbands*header.nstep);
    if(idata==NULL)
    {
      printallocerr("idata");
      return EXIT_FAILURE;
    }
    if(size!=(long long)header.ncell*header.nyear*header.nbands*header.nstep*sizeof(int))
    {
      if((long long)header.ncell*header.nyear*header.nbands*header.nstep*sizeof(short)==size)
      {
        fprintf(stderr,"File size of '%s' does not match header, set datatype to 2 byte size.\n",argv[index_datafile]);
        isint=FALSE;
        free(idata);
        data=newvec(short,(long long)header.ncell*header.nbands*header.nstep);
        if(data==NULL)
        {
          printallocerr("data");
          return EXIT_FAILURE;
        }
      }
      else
      {
        header.nyear=size/(sizeof(int)*header.ncell*header.nbands*header.nstep);
        if(size % (sizeof(int)*header.ncell*header.nbands*header.nstep)==0)
          fprintf(stderr,"File size of '%s' does not match header, number of years set to %d.\n",argv[index_datafile],header.nyear);
        else
        {
          fprintf(stderr,"File size of '%s' is not multiple of ncell and nbands.\n",argv[index_datafile]);
          return EXIT_FAILURE;
        }
      }
    }
  }
  else if(isbyte)
  {
    bdata=newvec(Byte,(long long)header.ncell*header.nbands*header.nstep);
    if(bdata==NULL)
    {
      printallocerr("bdata");
      return EXIT_FAILURE;
    }
    if(size!=(long long)header.ncell*header.nyear*header.nbands*header.nstep)
    {
      header.nyear=size/(header.ncell*header.nbands*header.nstep);
      if(size % (header.ncell*header.nbands*header.nstep)==0)
        fprintf(stderr,"File size of '%s' does not match header, number of years set to %d.\n",argv[index_datafile],header.nyear);
      else
      {
        fprintf(stderr,"File size of '%s' is not multiple of ncell and nbands.\n",argv[index_datafile]);
        return EXIT_FAILURE;
      }
    }
  }
  else
  {
    data=newvec(short,(long long)header.ncell*header.nbands*header.nstep);
    if(data==NULL)
    {
      printallocerr("data");
      return EXIT_FAILURE;
    }
    if(data==NULL)
    {
      printallocerr("data");
      return EXIT_FAILURE;
    }
    if(size!=(long long)header.ncell*header.nyear*header.nbands*header.nstep*sizeof(short))
    {
      if((long long)header.ncell*header.nyear*header.nbands*header.nstep*sizeof(int)==size)
      {
        fprintf(stderr,"File size of '%s' does not match header, set datatype to 4 byte size.\n",argv[index_datafile]);
        isint=TRUE;
        free(data);
        idata=newvec(int,(long long)header.ncell*header.nbands*header.nstep);
        if(idata==NULL)
        {
          printallocerr("idata");
          return EXIT_FAILURE;
        }
      }
      else
      {
        header.nyear=size/(sizeof(short)*header.ncell*header.nbands*header.nstep);
        if(size % (sizeof(short)*header.ncell*header.nbands*header.nstep)==0)
          fprintf(stderr,"File size of '%s' does not match header, number of years set to %d.\n",argv[index_datafile],header.nyear);
        else
        {
          fprintf(stderr,"File size of '%s' is not multiple of ncell and nbands.\n",argv[index_datafile]);
          return EXIT_FAILURE;
        }
      }
    }
  }
  file=fopen(argv[index_datafile+1],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[index_datafile+1],strerror(errno));
    return EXIT_FAILURE;
  }
  header2=header;
  header2.cellsize_lon=(float)res2.lon;
  header2.cellsize_lat=(float)res2.lat;
  header2.ncell=ngrid2;
  if(format==CLM)
  {
    if(fwriteheader(file,&header2,id,data_version))
    {
      fprintf(stderr,"Error writing header in '%s'.\n",argv[index_datafile+1]);
      return EXIT_FAILURE;
    }
  }
  index=newvec(int,ngrid2);
  if(index==NULL)
  {
    printallocerr("index");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    j=findcoord(c2+i,c,&res,ngrid);
    if(j==NOT_FOUND) /* coordinate not found? */
    {
      if(issearch)
      {
        fputs("Coordinate ",stderr);
        fprintcoord(stderr,c2+i);
        fputs(" not found, replaced by ",stderr);
        index[i]=findnextcoord(&dist_min,c2+i,c,ngrid);
        fprintcoord(stderr,c+index[i]);
        fprintf(stderr,", distance=%g\n",dist_min);
      }
      else
      {
        fputs("Coordinate ",stderr);
        fprintcoord(stderr,c2+i);
        fprintf(stderr," at index %d not found.\n",i);
        index[i]=-1;
        if(!iszero)
          return EXIT_FAILURE;
      }
    }
    else
      index[i]=j;
  }
  if(isint)
    for(i=0;i<header2.nyear;i++)
    {
      if(freadint(idata,(long long)header.ncell*header.nbands*header.nstep,swap,data_file)!=(long long)header.ncell*header.nbands*header.nstep)
      {
        fprintf(stderr,"Error reading '%s' in year %d.\n",argv[index_datafile],header2.firstyear+i);
        return EXIT_FAILURE;
      }
      for(j=0;j<header2.ncell;j++)
      {
        if(index[j]==-1)
          fwrite(zero,sizeof(int),header.nbands*header.nstep,file);
        else
        {
          if(fwrite(idata+index[j]*header.nbands*header.nstep,sizeof(int),header.nbands*header.nstep,file)!=header.nbands*header.nstep)
            fprintf(stderr,"Error writing file '%s: %s.\n",argv[index_datafile+1],strerror(errno));
        }
      }
    }
  else if(isbyte)
    for(i=0;i<header2.nyear;i++)
    {
      if(fread(bdata,1,(long long)header.ncell*header.nbands*header.nstep,data_file)!=(long long)header.ncell*header.nbands*header.nstep)
      {
        fprintf(stderr,"Error reading '%s' in year %d.\n",argv[index_datafile],header2.firstyear+i);
        return EXIT_FAILURE;
      }
      for(j=0;j<header2.ncell;j++)
      {
        if(index[j]==-1)
          fwrite(zero,1,header.nbands*header.nstep,file);
        else
        {
          if(fwrite(bdata+index[j]*header.nbands*header.nstep,1,header.nbands*header.nstep,file)!=header.nbands*header.nstep)
            fprintf(stderr,"Error writing file '%s: %s.\n",argv[index_datafile+1],strerror(errno));
        }
      }
    }
  else
    for(i=0;i<header2.nyear;i++)
    {
      if(freadshort(data,(long long)header.ncell*header.nbands*header.nstep,swap,data_file)!=(long long)header.ncell*header.nbands*header.nstep)
      {
        fprintf(stderr,"Error reading '%s' in year %d.\n",argv[index_datafile],header2.firstyear+i);
        return EXIT_FAILURE;
      }
      for(j=0;j<header2.ncell;j++)
      {
        if(index[j]==-1)
          fwrite(zero,sizeof(short),header.nbands*header.nstep,file);
        else
        {
          if(fwrite(data+index[j]*header.nbands*header.nstep,sizeof(short),header.nbands*header.nstep,file)!=header.nbands*header.nstep)
            fprintf(stderr,"Error writing file '%s: %s.\n",argv[index_datafile+1],strerror(errno));
        }
      }
    }
  free(index);
  free(bdata);
  free(idata);
  free(zero);
  free(data);
  free(c);
  free(c2);
  fclose(file);
  fclose(data_file);
  if(ismeta || isjson)
  {
    out_json=malloc(strlen(argv[index_datafile+1])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[index_datafile+1]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    file=fopen(out_json,"w");
    if(file==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    if(data_version<4)
      header2.nbands/=header2.nstep;
    fprintjson(file,argv[index_datafile+1],NULL,source,history,arglist,&header2,map,map_name,global_attrs,n_global,var_name,var_units,var_standard_name,var_long_name,&filename,grid_type,format,id,FALSE,data_version);
    free(out_json);
    free(arglist);
    fclose(file);
  }
  free(filename.name);
  freemap(map);
  freeattrs(global_attrs,n_global);
  free(var_name);
  free(var_units);
  free(var_long_name);
  free(var_standard_name);
  free(source);
  free(history);
  return EXIT_SUCCESS;
} /* of 'main' */
