/**************************************************************************************/
/**                                                                                \n**/
/**                    r  e  g  r  i  d  i  r  r  i  g  .  c                       \n**/
/**                                                                                \n**/
/**     CLM data in 0.5 degree resolution is regridded to 0.25 degree              \n**/
/**     resolution.                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int main(int argc,char **argv)
{
  FILE *file,*data_file;
  Header header,header2;
  Coord *c,*c2;
  Bool swap;
  int *data;
  long long size;
  Coord res,res2;
  Coordfile grid;
  int i,index,index2,data_version,setversion,ngrid,ngrid2;
  float lon,lat;
  String id;
  Filename filename;
  setversion=READ_VERSION;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-longheader"))
        setversion=2;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n",argv[i]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  argc-=i-1;
  argv+=i-1;
  if(argc<5)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            "Usage: %s [-longheader] coord_all.clm coord.clm data_all.clm data.clm\n",
            argv[1-i]);
    return EXIT_FAILURE;
  }
  filename.name=argv[1];
  filename.fmt=(setversion==2) ? CLM2 : CLM;
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
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[1]);
      return EXIT_FAILURE;
    }
    //printf("c:%g %g\n",c[i].lon,c[i].lat);
  }
  closecoord(grid);
  filename.name=argv[2];
  filename.fmt=(setversion==2) ? CLM2 : CLM;
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
    return EXIT_FAILURE;
  ngrid2=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  res2.lon=lon;
  res2.lat=lat;
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
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[2]);
      return EXIT_FAILURE;
    }
    //printf("c2:%g %g\n",c2[i].lon,c2[i].lat);
  }
  closecoord(grid);
  data_file=fopen(argv[3],"rb");
  if(data_file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }
  data_version=setversion;
  if(freadanyheader(data_file,&header,&swap,id,&data_version,TRUE))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[3]);
    return EXIT_FAILURE;
  }
  if(header.nyear<=0)
  {
    fprintf(stderr,"Invalid nyear=%d, set to one.\n",header.nyear);
    header.nyear=1;
  }
  if(header.ncell!=ngrid)
  {
    fprintf(stderr,"Invalid number of cells %d in '%s', not %d.\n",
            header.ncell,argv[3],ngrid);
    return EXIT_FAILURE;
  }
  size=getfilesizep(data_file)-headersize(id,data_version);
  data=newvec(int,header.ncell);
  if(data==NULL)
  {
    printallocerr("idata");
    return EXIT_FAILURE;
  }
  if(size!=(long long)header.ncell*header.nyear*header.nbands*sizeof(int))
  {
    header.nyear=size/(sizeof(int)*header.ncell*header.nbands);
    fprintf(stderr,"File '%s' too short.\n",argv[3]);
    return EXIT_FAILURE;
  }
  fread(data,sizeof(int),header.ncell,data_file);
  fclose(data_file);
  file=fopen(argv[4],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[4],strerror(errno));
    return EXIT_FAILURE;
  }
  header2=header;
  header2.ncell=ngrid2;
  if(fwriteheader(file,&header2,id,data_version))
  {
    fprintf(stderr,"Error writing header in '%s'.\n",argv[4]);
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
      return EXIT_FAILURE;
    }
    index2=findcoord(c+index,c2,&res,ngrid2);
    if(index2==NOT_FOUND)
    {
      fputs("Coordinate ",stderr);
      fprintcoord(stderr,c+index);
      fputs(" for cell ",stderr);
      fprintcoord(stderr,c2+i);
      fprintf(stderr," not found in '%s', set to itself.\n",argv[2]);
      index=i;
    }
    fwrite(&index2,1,sizeof(int),file);
  }
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
