/**************************************************************************************/
/**                                                                                \n**/
/**                    r  e  g  r  i  d  s  o  i  l  .  c                          \n**/
/**                                                                                \n**/
/**     soil data in 0.5 degree resolution is regridded to 0.25 degree             \n**/
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
  FILE *file;
  Coord *c,*c2;
  Bool same;
  Byte *soil,zero=0;
  Coordfile grid;
  float lon,lat;
  Coord res,res2;
  int i,j,setversion,ngrid,ngrid2,count;
  Filename filename;
  setversion=READ_VERSION;
  same=FALSE;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[i],"-same"))
        same=TRUE;
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
           "Usage: %s [-same] [-longheader] coord0.5.clm coord0.25.clm soil0.5.bin soil0.25.bin\n",
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
  same=(res.lon==res2.lon);
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
  for(i=0;i<ngrid2;i++)
  {
    if(ngrid2>10 && (i %(ngrid2/10)==0))
    {
      printf("\b\b\b\b\b%3d%%]",(i/(ngrid2/10))*10);
      fflush(stdout);
    }
#ifdef DEBUG
    printf("%d ",i);
    printcoord(c2[i]); 
    fputs(":",stdout);
#endif
    for(j=0;j<ngrid;j++)
    {
      if((same && c2[i].lat==c[j].lat && c2[i].lon==c[j].lon)||
         (!same && c2[i].lat-c[j].lat<=0.25 && c2[i].lon-c[j].lon<=0.25))
      {
#ifdef DEBUG
        printf("%d  ",j);
        printf("lat %.2f  lon %.2f",c[j].lat,c[j].lon);
        fflush(stdout);
#endif
        break;
      }
    }
    if(j==ngrid)
    {
      fwrite(&zero,1,1,file);
      count++;
    }
    else
      fwrite(soil+j,1,1,file);
  }
  putchar('\n');
  if(count)
    fprintf(stderr,"Warning: %d cells not found, soilcode set to zero.\n",count);
  fclose(file);
  return EXIT_SUCCESS;
}  /* of 'main' */
