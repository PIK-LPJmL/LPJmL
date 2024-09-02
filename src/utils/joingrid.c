/**************************************************************************************/
/**                                                                                \n**/
/**                    j  o  i  n  g  r  i  d  .  c                                \n**/
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
  Coordfile grid;
  Filename filename;
  FILE *file;
  Header header;
  Bool swap;
  int version,count;
  Coord res,*c,*c2;
  float lon,lat;
  int i,setversion,ngrid,ngrid2;
  setversion=READ_VERSION;
  struct
  {
   float lon,lat;
  } fcoord;
  struct
  {
   double lon,lat;
  } dcoord;
  Intcoord scoord;
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
  if(argc<4)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            "Usage: %s [-longheader] grid.clm basegrid.clm newgrid.clm\n",
            argv[1-i]);
    return EXIT_FAILURE;
  }
  filename.name=argv[1];
  filename.fmt=(setversion==2) ? CLM2 : CLM;
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
    return EXIT_FAILURE;
  version=setversion;
  file=fopen(argv[1],"rb");
  if(freadheader(file,&header,&swap,LPJGRID_HEADER,&version,TRUE))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  fclose(file);
  if(version==1)
  {
    header.scalar=0.01;
    header.cellsize_lon=header.cellsize_lat=0.5;
  }
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
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
    return EXIT_FAILURE;
  ngrid2=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  if(lon!=res.lon || lat!=res.lat)
  {
    fprintf(stderr,"Error resolution (%g,%g) in '%s' differs from (%g,%g) in '%s'.\n",
            lat,lon,argv[2],res.lat,res.lon,argv[1]);
    return EXIT_FAILURE;
  }
  res.lon=lon;
  res.lat=lat;
  c2=newvec(Coord,ngrid2);
  if(c==NULL)
  {
    closecoord(grid);
    printallocerr("c");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    if(readcoord(grid,c2+i,&res))
    {
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[2]);
      return EXIT_FAILURE;
    }
    //printf("c:%g %g\n",c[i].lon,c[i].lat);
  }
  closecoord(grid);
  file=fopen(argv[3],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }
  if(fwriteheader(file,&header,LPJGRID_HEADER,version))
  {
    fprintf(stderr,"Error writing header in '%s'.\n",argv[3]);
    return EXIT_FAILURE;
  }
  count=0;
  printf("[     ");
  for(i=0;i<ngrid;i++)
  {
    if(ngrid>10 && (i %(ngrid/10)==0))
    {
      printf("\b\b\b\b\b%3d%%]",(i/(ngrid/10))*10);
      fflush(stdout);
    }
    if(findcoord(c+i,c2,&res,ngrid2)!=NOT_FOUND)
    {
      switch(header.datatype)
      {
        case LPJ_SHORT:
          scoord.lon=(short)(c[i].lon*0.01);
          scoord.lat=(short)(c[i].lat*0.01);
          fwrite(&scoord,sizeof(Intcoord),1,file);
          break;
        case LPJ_FLOAT:
          fcoord.lon=(float)c[i].lon;
          fcoord.lat=(float)c[i].lat;
          fwrite(&fcoord,sizeof(fcoord),1,file);
          break;
        case LPJ_DOUBLE:
          dcoord.lon=c[i].lon;
          dcoord.lat=c[i].lat;
          fwrite(&dcoord,sizeof(dcoord),1,file);
          break;
        default:
          break;
      }
      count++;
    }
  }
  printf("\nNumber of cells in file %d, %d skipped.\n",
         count,ngrid-count);
  header.ncell=count;
  rewind(file);
  fwriteheader(file,&header,LPJGRID_HEADER,version);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
