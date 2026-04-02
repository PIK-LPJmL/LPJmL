/**************************************************************************************/
/**                                                                                \n**/
/**                    r  e  g  r  i  d  i  r  r  i  g  .  c                       \n**/
/**                                                                                \n**/
/**     Irrigation file is regridded to a new grid file                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-longheader] [-metafile] [-json] [coord_old.clm] coord_new.clm data_old.clm data_new.clm\n"

int main(int argc,char **argv)
{
  FILE *file,*data_file;
  Header header,header2;
  Coord *c,*c2;
  Bool swap,isjson=FALSE,ismeta=FALSE;
  int *data;
  long long size;
  Coord res,res2;
  Coordfile grid;
  int i,index,index2,data_version,setversion,ngrid,ngrid2;
  float lon,lat;
  String id;
  Filename filename,grid_name;
  char *arglist,*out_json;
  size_t offset;
  Type grid_type;
  Map *map=NULL;
  Attr *global_attrs=NULL;
  char *var_name=NULL,*var_units=NULL,*var_long_name=NULL,*var_standard_name=NULL;
  char *source=NULL,*history=NULL;
  char *path;
  char *map_name;
  map_name=MAP_NAME;
  int n_global=0,format,iarg,index_datafile,index_gridfile;
  setversion=READ_VERSION;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<(ismeta ? 3 : 4)+iarg)
  {
    fprintf(stderr,"Missing argument(s)\n"
            USAGE,
            argv[0]);
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
    header.nstep=1;
    header.datatype=LPJ_INT;
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
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",argv[index_datafile],offset);      fclose(data_file);
      return EXIT_FAILURE;
    }
    if(argc==iarg+4)
    {
      filename.name=strdup(argv[iarg]);
      check(filename.name);
      filename.fmt=CLM;
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
    if(data_version<3)
      header.datatype=LPJ_INT;
    filename.name=strdup(argv[iarg]);
    check(filename.name);
    filename.fmt=CLM;
  }
  if(header.datatype!=LPJ_INT)
  {
    fprintf(stderr,"Invalid datatype %s in '%s', must be int.\n",typenames[header.datatype],argv[index_datafile]);
    {
      fclose(data_file);
      return EXIT_FAILURE;
    }
  }
  if(header.nbands!=1)
  {
    fprintf(stderr,"Invalid number of bands %d in '%s', must be 1.\n",header.nbands,argv[index_datafile]);
    {
      fclose(data_file);
      return EXIT_FAILURE;
    }
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
  {
    free(c);
    return EXIT_FAILURE;
  }
  grid_type=getcoordtype(grid);
  ngrid2=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  res2.lon=lon;
  res2.lat=lat;
  c2=newvec(Coord,ngrid2);
  if(c2==NULL)
  {
    free(c);
    closecoord(grid);
    printallocerr("c2");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    if(readcoord(grid,c2+i,&res2))
    {
      free(c);
      free(c2);
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[index_gridfile]);
      return EXIT_FAILURE;
    }
    //printf("c2:%g %g\n",c2[i].lon,c2[i].lat);
  }
  closecoord(grid);
  if(header.nyear<=0)
  {
    fprintf(stderr,"Invalid nyear=%d, set to one.\n",header.nyear);
    header.nyear=1;
  }
  if(header.ncell!=ngrid)
  {
    fprintf(stderr,"Invalid number of cells %d in '%s', not %d.\n",
            header.ncell,argv[index_datafile],ngrid);
    free(c);
    free(c2);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  size=getfilesizep(data_file)-headersize(id,data_version);
  data=newvec(int,header.ncell);
  if(data==NULL)
  {
    printallocerr("idata");
    free(c);
    free(c2);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  if(size!=(long long)header.ncell*header.nyear*header.nbands*sizeof(int))
  {
    header.nyear=size/(sizeof(int)*header.ncell*header.nbands);
    fprintf(stderr,"File '%s' too short.\n",argv[3]);
    free(c);
    free(c2);
    free(data);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  freadint(data,header.ncell,swap,data_file);
  fclose(data_file);
  file=fopen(argv[index_datafile+1],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[index_datafile+1],strerror(errno));
    free(c);
    free(c2);
    free(data);
    return EXIT_FAILURE;
  }
  header2=header;
  header2.ncell=ngrid2;
  if(fwriteheader(file,&header2,id,data_version))
  {
    fprintf(stderr,"Error writing header in '%s'.\n",argv[index_datafile+1]);
    free(c);
    free(c2);
    free(data);
    fclose(file);
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    index=findcoord(c2+i,c,&res,ngrid);
    if(index==NOT_FOUND)
    {
      fputs("Coordinate ",stderr);
      fprintcoord(stderr,c2+i);
      fputs(" not found.\n",stderr);
      return EXIT_FAILURE;
    }
    index=data[index];
    if(index<0 || index>=ngrid)
    {
      fprintf(stderr,"Invalid index %d for cell %d (",index,i);
      fprintcoord(stderr,c2+i);
      fprintf(stderr,") found, must be in [0,%d].\n",ngrid-1);
      free(c);
      free(c2);
      free(data);
      fclose(file);
      return EXIT_FAILURE;
    }
    index2=findcoord(c+index,c2,&res,ngrid2);
    if(index2==NOT_FOUND)
    {
      fputs("Coordinate ",stderr);
      fprintcoord(stderr,c+index);
      fputs(" for cell ",stderr);
      fprintcoord(stderr,c2+i);
      fprintf(stderr," not found in '%s', set to itself.\n",argv[index_gridfile]);
      index2=i;
    }
    if(fwrite(&index2,sizeof(int),1,file)!=1)
    {
      fprintf(stderr,"Error writing '%s': %s.\n",argv[index_datafile+1],strerror(errno));
      free(c);
      free(c2);
      free(data);
      fclose(file);
      return EXIT_FAILURE;
    }
  }
  fclose(file);
  free(c);
  free(c2);
  free(data);
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
