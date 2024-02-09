/**************************************************************************************/
/**                                                                                \n**/
/**                    r  e  g  r  i  d  s  o  i  l  .  c                          \n**/
/**                                                                                \n**/
/**     soil data is regridded to new grid file                                    \n**/
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
  FILE *file;
  Coord *c,*c2;
  Bool iszero,issearch;
  Byte *soil,zero=0;
  Coordfile grid;
  float lon,lat;
  Coord res,res2;
  Real dist_min;
  int i,j,setversion,ngrid,ngrid2,count;
  Filename filename;
  setversion=READ_VERSION;
  iszero=issearch=FALSE;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[i],"-search"))
        issearch=TRUE;
      else if(!strcmp(argv[i],"-zero"))
        iszero=TRUE;
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
    fprintf(stderr,"Missing arguments.\n"
           "Usage: %s [-longheader] [-search] [-zero] coord_old.clm coord_new.clm soil_old.bin soil_new.bin\n",
            argv[0]);
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
    printallocerr("c");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
    if(readcoord(grid,c+i,&res))
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[1]);
      return EXIT_FAILURE;
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
  if(res.lon!=res2.lon)
    fprintf(stderr,"Warning: longitudinal resolution %g in '%s' differs from %g in '%s.\n",res2.lon,argv[2],res.lon,argv[1]);
  if(res.lat!=res2.lat)
    fprintf(stderr,"Warning: latitudinal resolution %g in '%s' differs from %g in '%s.\n",res2.lat,argv[2],res.lat,argv[1]);
  c2=newvec(Coord,ngrid2);
  if(c2==NULL)
  {
    printallocerr("c2");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
    if(readcoord(grid,c2+i,&res2))
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[2]);
      return EXIT_FAILURE;
    }
  closecoord(grid);
  file=fopen(argv[3],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }
  soil=malloc(ngrid);
  if(soil==NULL)
  {
    printallocerr("soil");
    return EXIT_FAILURE;
  }
  if(fread(soil,1,ngrid,file)!=ngrid)
  {
    fprintf(stderr,"Error reading '%s': %s.\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }
  fclose(file);
  file=fopen(argv[4],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[4],strerror(errno));
    return EXIT_FAILURE;
  }
  count=0;
  printf("[     ");
  for(i=0;i<ngrid2;i++)
  {
    if(ngrid2>10 && (i %(ngrid2/10)==0))
    {
      printf("\b\b\b\b\b%3d%%]",(i/(ngrid2/10))*10);
      fflush(stdout);
    }
#ifdef DEBUG
    printf("%d ",i);
    printcoord(c2+i);
    fputs(":",stdout);
#endif
    j=findcoord(c2+i,c,&res,ngrid);
    if(j==NOT_FOUND)
    {
      count++;
      if(issearch)
      {
        fputs("Coordinate ",stderr);
        fprintcoord(stderr,c2+i);
        fputs(" not found, replaced by ",stderr);
        j=findnextcoord(&dist_min,c2+i,c,ngrid);
        fprintcoord(stderr,c+j);
        fprintf(stderr,", distance=%g\n",dist_min);
      }
      else
      {
        fputs("Coordinate ",stderr);
        fprintcoord(stderr,c2+i);
        fprintf(stderr," at index %d not found.\n",i);
        j=ngrid;
        if(!iszero)
          return EXIT_FAILURE;
      }
    }
    if(j==ngrid)
      fwrite(&zero,1,1,file);
    else
      fwrite(soil+j,1,1,file);
  }
  putchar('\n');
  if(count)
    fprintf(stderr,"Warning: %d cells not found.\n",count);
  fclose(file);
  return EXIT_SUCCESS;
}  /* of 'main' */
