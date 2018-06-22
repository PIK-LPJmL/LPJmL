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
  FILE *file;
  Header header,header2;
  Intcoord *c,*c2;
  Bool swap;
  int version,count;
  Coord coord;
  int i,j,base_version,setversion;
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
  if(argc<4)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            "Usage: %s [-longheader] grid.clm basegrid.clm newgrid.clm\n",
            argv[1-i]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[1],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }
  version=setversion;
  if(freadheader(file,&header,&swap,LPJGRID_HEADER,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(version==1)
  {
    header.scalar=0.01;
    header.cellsize_lon=header.cellsize_lat=0.5;
  }
  c=newvec(Intcoord,header.ncell);
  if(c==NULL)
  {
    fclose(file);
    printallocerr("c");
    return EXIT_FAILURE;
  }
  for(i=0;i<header.ncell;i++)
    if(readintcoord(file,c+i,swap))
    {
      fclose(file);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[1]);
      return EXIT_FAILURE;
    }
  fclose(file);
  file=fopen(argv[2],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[2],strerror(errno));
    return EXIT_FAILURE;
  }
  base_version=setversion;
  if(freadheader(file,&header2,&swap,LPJGRID_HEADER,&base_version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[2]);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(base_version==1)
    header2.scalar=0.01;
  c2=newvec(Intcoord,header2.ncell);
  if(c2==NULL)
  {
    fclose(file);
    printallocerr("c2");
    return EXIT_FAILURE;
  }
  for(i=0;i<header2.ncell;i++)
    if(readintcoord(file,c2+i,swap))
    {
      fclose(file);
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[2]);
      return EXIT_FAILURE;
    }
  fclose(file);
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
  for(i=0;i<header.ncell;i++)
  {
    coord.lat=c[i].lat*header.scalar;
    coord.lon=c[i].lon*header.scalar;
    for(j=0;j<header2.ncell;j++)
    {
      if(coord.lat==c2[j].lat*header2.scalar && coord.lon==c2[j].lon*header2.scalar)
      {
        count++;
        fwrite(c+i,sizeof(Intcoord),1,file);
        break;
      }
    }
  }
  printf("Number of cells in file %d, %d skipped.\n",
         count,header.ncell-count);
  header.ncell=count;
  rewind(file);
  fwriteheader(file,&header,LPJGRID_HEADER,version);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
