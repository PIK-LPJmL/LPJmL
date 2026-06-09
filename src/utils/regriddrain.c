/**************************************************************************************/
/**                                                                                \n**/
/**                    r  e  g  r  i  d  d  r  a  i  n  .  c                       \n**/
/**                                                                                \n**/
/**     Drainage file is regridded to a new grid file                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-metafile] [-json] [coord_all.clm] coord.clm drainage_all.clm drainage.clm\n"
#define UNDEFINED -99999

int main(int argc,char **argv)
{
  int i,j,ngrid,ngrid2;
  float lon,lat;
  Filename filename,grid_name;
  Coordfile grid;
  Coord *c,c2,res;
  int *index,*index2;
  FILE *file,*data_file;
  Header header;
  Metadata metadata;
  String id,line,line2;
  Bool swap,ismeta=FALSE,isjson=FALSE;
  Routing *r,r2;
  char *arglist,*out_json;
  size_t offset;
  Type grid_type;
  char *path;
  int format,iarg,index_datafile,index_gridfile,data_version;
  initmetadata(&metadata,NULL);
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-metafile"))
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
            USAGE,argv[0]);
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
    data_file=openmetafile(&header,&metadata,&grid_name,NULL,&format,&swap,&offset,argv[index_datafile],TRUE);
    if(data_file==NULL)
      return EXIT_FAILURE;
    if(format==CLM)
    {
      if(freadheaderid(data_file,id,TRUE))
      {
        freemetadata(&metadata);
        fclose(data_file);
        return EXIT_FAILURE;
      }
    }
    if(fseek(data_file,offset,SEEK_SET))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",argv[index_datafile],offset);
      freemetadata(&metadata);
      fclose(data_file);
      return EXIT_FAILURE;
    }
    if(argc==iarg+4)
    {
      filename.name=strdup(argv[iarg]);
      if(filename.name==NULL)
      {
        printallocerr("name");
        freemetadata(&metadata);
        fclose(data_file);
        return EXIT_FAILURE;
      }
      filename.fmt=CLM;
    }
    else
    {
      if(grid_name.name==NULL)
      {
        fprintf(stderr,"Error: grid filename must be specified in '%s' metafile.\n",argv[index_datafile]);
        freemetadata(&metadata);
        fclose(data_file);
        return EXIT_FAILURE;
      }
      path=getpath(argv[index_datafile]);
      filename.name=addpath(grid_name.name,path);
      filename.fmt=grid_name.fmt;
      free(grid_name.name);
      free(path);
      if(filename.name==NULL)
      {
        printallocerr("name");
        freemetadata(&metadata);
        fclose(data_file);
        return EXIT_FAILURE;
      }
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
    data_version=READ_VERSION;
    if(freadanyheader(data_file,&header,&swap,id,&data_version,TRUE))
    {
      fprintf(stderr,"Error reading header in '%s'.\n",argv[iarg+2]);
      fclose(data_file);
      return EXIT_FAILURE;
    }
    if(data_version>CLM_MAX_VERSION)
    {
      fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
              data_version,argv[iarg+2],CLM_MAX_VERSION+1);
      fclose(data_file);
      return EXIT_FAILURE;
    }
    if(data_version<3)
      header.datatype=LPJ_INT;
    filename.name=strdup(argv[iarg]);
    if(filename.name==NULL)
    {
      printallocerr("name");
      fclose(data_file);
      return EXIT_FAILURE;
    }    
    filename.fmt=CLM;
  }
  if(header.datatype!=LPJ_INT)
  {
    fprintf(stderr,"Invalid datatype %s in '%s', must be int.\n",typenames[header.datatype],argv[index_datafile]);
    {
      freemetadata(&metadata);
      free(filename.name);
      fclose(data_file);
      return EXIT_FAILURE;
    }
  }
  if(header.nbands!=2)
  {
    fprintf(stderr,"Invalid number of bands %d in '%s', must be 2.\n",header.nbands,argv[index_datafile]);
    {
      freemetadata(&metadata);
      fclose(data_file);
      return EXIT_FAILURE;
    }
  }
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
  {
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  ngrid=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  res.lon=lon;
  res.lat=lat;
  c=newvec(Coord,ngrid);
  if(c==NULL)
  {
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    closecoord(grid);
    printallocerr("c");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
    if(readcoord(grid,c+i,&res))
    {
      free(c);
      freemetadata(&metadata);
      free(filename.name);
      fclose(data_file);
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,filename.name);
      return EXIT_FAILURE;
    }
  closecoord(grid);
  free(filename.name);
  filename.name=strdup(argv[index_gridfile]);
  if(filename.name==NULL)
  {
    printallocerr("name");
    free(c);
    freemetadata(&metadata);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  filename.fmt=CLM;
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
  {
    free(c);
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  grid_type=getcoordtype(grid);
  ngrid2=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  if(res.lon!=lon || res.lat!=lat)
  {
    fprintf(stderr,"Cell size (%g,%g) in '%s' differs from (%g,%g).\n",
            lon,lat,argv[index_gridfile],res.lon,res.lat);
    free(c);
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    closecoord(grid);
    return EXIT_FAILURE;
  }
  index=newvec(int,ngrid);
  if(index==NULL)
  {
    free(c);
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    closecoord(grid);
    printallocerr("index");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
    index[i]=UNDEFINED;
  index2=newvec(int,ngrid2);
  if(index2==NULL)
  {
    free(c);
    free(index);
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    closecoord(grid);
    printallocerr("index2");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    if(readcoord(grid,&c2,&res))
    {
      free(c);
      free(index);
      free(index2);
      freemetadata(&metadata);
      free(filename.name);
      fclose(data_file);
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[index_gridfile]);
      return EXIT_FAILURE;
    }
    j=findcoord(&c2,c,&res,ngrid);
    if(j==NOT_FOUND)
    {
      fputs("Coordinate ",stderr);
      fprintcoord(stderr,&c2);
      fputs(" not found.\n",stderr);
      free(c);
      free(index);
      free(index2);
      freemetadata(&metadata);
      free(filename.name);
      fclose(data_file);
      closecoord(grid);
      return EXIT_FAILURE;
    }
    index[j]=i;
    index2[i]=j;
  }
  closecoord(grid);
  if(res.lon!=header.cellsize_lon || res.lat!=header.cellsize_lat)
  {
    fprintf(stderr,"Cell size (%g,%g) in '%s' differs from (%g,%g).\n",
            header.cellsize_lon,header.cellsize_lat,argv[index_datafile],res.lon,res.lat);
    free(c);
    free(index);
    free(index2);
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  if(header.ncell!=ngrid)
  {
    fprintf(stderr,"Invalid number of cells=%d in '%s', must be %d.\n",
            header.ncell,argv[index_datafile],ngrid);
    free(c);
    free(index);
    free(index2);
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  r=newvec(Routing,header.ncell);
  if(r==NULL)
  {
    printallocerr("r");
    free(c);
    free(index);
    free(index2);
    freemetadata(&metadata);
    free(filename.name);
    fclose(data_file);
    return EXIT_FAILURE;
  }
  for(i=0;i<header.ncell;i++)
  {
    if(getroute(data_file,r+i,swap))
    {
      fprintf(stderr,"Error reading route %d in '%s'.\n",i,argv[index_datafile]);
      free(r);
      free(c);
      free(index);
      free(index2);
      freemetadata(&metadata);
      free(filename.name);
      fclose(data_file);
      return EXIT_FAILURE;
    }
  }
  fclose(data_file);
  file=fopen(argv[index_datafile+1],"wb");
  if(file==NULL)
  {
    printfcreateerr(argv[index_datafile+1]);
    free(r);
    free(c);
    free(index);
    free(index2);
    freemetadata(&metadata);
    free(filename.name);
    return EXIT_FAILURE;
  }
  header.ncell=ngrid2;
  fwriteheader(file,&header,id,data_version);
  for(i=0;i<ngrid2;i++)
  {
    r2.len=r[index2[i]].len;
    if(r[index2[i]].index<0)
      r2.index=r[index2[i]].index;
    else
    {
      r2.index=index[r[index2[i]].index];
      if(r2.index==UNDEFINED)
      {
        fprintf(stderr,"Index not found for cell %d (%s) in '%s' mapped to %s, set to -1.\n",
                i,sprintcoord(line,c+index2[i]),argv[2],sprintcoord(line2,c+r[index2[i]].index));
        r2.index= -1;
      }
    }
    fwrite(&r2,sizeof(r2),1,file);
  }
  free(c);
  free(index);
  free(index2);
  free(r);
  fclose(file);
  if(ismeta || isjson)
  {
    out_json=malloc(strlen(argv[index_datafile+1])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      freemetadata(&metadata);
      free(filename.name);
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[index_datafile+1]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    if(arglist==NULL)
    {
      printallocerr("arglist");
      free(out_json);
      freemetadata(&metadata);
      free(filename.name);
      return EXIT_FAILURE;
    }
    file=fopen(out_json,"w");
    if(file==NULL)
    {
      free(out_json);
      free(filename.name);
      freemetadata(&metadata);
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    fprintjson(file,argv[index_datafile+1],NULL,arglist,&header,&metadata,&filename,grid_type,format,id,FALSE,data_version);
    free(out_json);
    free(arglist);
    fclose(file);
  }
  free(filename.name);
  freemetadata(&metadata);
  return EXIT_SUCCESS;
} /* of 'main' */
