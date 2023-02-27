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

#define UNDEFINED -99999

int main(int argc,char **argv)
{
  int i,j,ngrid,ngrid2,version;
  float lon,lat;
  Filename filename;
  Coordfile grid;
  Coord *c,c2,res;
  int *index,*index2;
  FILE *file;
  Header header;
  String headername,line,line2;
  Bool swap;
  Routing *r,r2;
  if(argc<5)
  {
    fprintf(stderr,"Missing argument(s)\n"
            "Usage: %s coord_all.clm coord.clm drainage_all.clm drainage.clm\n",
            argv[0]);
    return EXIT_FAILURE;
  }
  filename.name=argv[1];
  filename.fmt=CLM;
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
    if(readcoord(grid,c+i,&res))
    {
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[1]);
      return EXIT_FAILURE;
    }
  closecoord(grid);
  filename.name=argv[2];
  filename.fmt=CLM;
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
    return EXIT_FAILURE;
  ngrid2=numcoord(grid);
  getcellsizecoord(&lon,&lat,grid);
  if(res.lon!=lon || res.lat!=lat)
  {
    fprintf(stderr,"Cell size (%g,%g) in '%s' differs from (%g,%g) in '%s'.\n",
            lon,lat,argv[2],res.lon,res.lat,argv[1]);
    return EXIT_FAILURE;
  }
  index=newvec(int,ngrid);
  if(index==NULL)
  {
    closecoord(grid);
    printallocerr("index");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
    index[i]=UNDEFINED;
  index2=newvec(int,ngrid2);
  if(index2==NULL)
  {
    closecoord(grid);
    printallocerr("index2");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    if(readcoord(grid,&c2,&res))
    {
      closecoord(grid);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[2]);
      return EXIT_FAILURE;
    }
    j=findcoord(&c2,c,&res,ngrid);
    if(j==NOT_FOUND)
    {
      fputs("Coordinate ",stderr);
      fprintcoord(stderr,&c2);
      fputs(" not found.\n",stderr);
      return EXIT_FAILURE;
    }
    index[j]=i;
    index2[i]=j;
  }
  closecoord(grid);
  file=fopen(argv[3],"rb");
  if(file==NULL)
  {
    printfopenerr(argv[3]);
    return EXIT_FAILURE;
  }
  version=READ_VERSION;
  if(freadanyheader(file,&header,&swap,headername,&version,TRUE))
  {
    fprintf(stderr,"Invalid header in '%s'.\n",argv[3]);
    return EXIT_FAILURE;
  }
  if(res.lon!=header.cellsize_lon || res.lat!=header.cellsize_lat)
  {
    fprintf(stderr,"Cell size (%g,%g) in '%s' differs from (%g,%g) in '%s'.\n",
            header.cellsize_lon,header.cellsize_lat,argv[4],res.lon,res.lat,argv[1]);
    return EXIT_FAILURE;
  }
  if(header.ncell!=ngrid)
  {
    fprintf(stderr,"Number of cells=%d in '%s' differs from %d in '%s'.\n",
            header.ncell,argv[3],ngrid,argv[1]);
    return EXIT_FAILURE;
  }
  r=newvec(Routing,header.ncell);
  if(r==NULL)
  {
    printallocerr("r");
    return EXIT_FAILURE;
  }
  for(i=0;i<header.ncell;i++)
  {
    if(getroute(file,r+i,swap))
    {
      fprintf(stderr,"Error reading route %d in '%s'.\n",i,argv[3]);
      return EXIT_FAILURE;
    }
  }
  fclose(file);
  file=fopen(argv[4],"wb");
  if(file==NULL)
  {
    printfcreateerr(argv[4]);
    return EXIT_FAILURE;
  }
  header.ncell=ngrid2;
  fwriteheader(file,&header,headername,version);
  for(i=0;i<ngrid2;i++)
  {
    r2.len=r[index2[i]].len;
    if(r[index2[i]].index<0)
      r2.len=r[index2[i]].index;
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
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
