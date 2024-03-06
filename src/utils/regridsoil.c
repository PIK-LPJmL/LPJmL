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

#define USAGE "Usage: %s [-longheader] [-search] [-zero] [-json] coord_old.clm coord_new.clm soil_old.bin soil_new.bin\n"

int main(int argc,char **argv)
{
  FILE *file;
  Coord *c,*c2;
  Bool iszero,issearch,isjson;
  Byte *soil,zero=0;
  char *arglist,*out_json;
  Coordfile grid;
  float lon,lat;
  Coord res,res2;
  Real dist_min;
  int i,j,setversion,ngrid,ngrid2,count,iarg;
  Filename filename,grid_name;
  Type grid_type;
  Header header;
  setversion=READ_VERSION;
  iszero=issearch=isjson=FALSE;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-longheader"))
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
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;

  if(argc<4+iarg)
  {
    fprintf(stderr,"Missing arguments.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  filename.name=argv[iarg];
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
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[iarg]);
      return EXIT_FAILURE;
    }
  closecoord(grid);
  filename.name=argv[iarg+1];
  filename.fmt=(setversion==2) ? CLM2 : CLM;
  grid=opencoord(&filename,TRUE);
  if(grid==NULL)
    return EXIT_FAILURE;
  ngrid2=numcoord(grid);
  grid_type=getcoordtype(grid);
  getcellsizecoord(&lon,&lat,grid);
  res2.lon=lon;
  res2.lat=lat;
  if(res.lon!=res2.lon)
    fprintf(stderr,"Warning: longitudinal resolution %g in '%s' differs from %g in '%s.\n",res2.lon,argv[iarg+1],res.lon,argv[iarg]);
  if(res.lat!=res2.lat)
    fprintf(stderr,"Warning: latitudinal resolution %g in '%s' differs from %g in '%s.\n",res2.lat,argv[iarg+1],res.lat,argv[iarg]);
  c2=newvec(Coord,ngrid2);
  if(c2==NULL)
  {
    printallocerr("c2");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
    if(readcoord(grid,c2+i,&res2))
    {
      fprintf(stderr,"Error reading cell %d in '%s'.\n",i,argv[iarg+1]);
      return EXIT_FAILURE;
    }
  closecoord(grid);
  file=fopen(argv[iarg+2],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
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
    fprintf(stderr,"Error reading '%s': %s.\n",argv[iarg+2],strerror(errno));
    return EXIT_FAILURE;
  }
  fclose(file);
  file=fopen(argv[iarg+3],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[iarg+3],strerror(errno));
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
  if(isjson)
  {
    out_json=malloc(strlen(argv[iarg+3])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[iarg+3]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    file=fopen(out_json,"w");
    if(file==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    grid_name.name=argv[iarg+1];
    grid_name.fmt=CLM;
    header.datatype=LPJ_BYTE;
    header.firstcell=0;
    header.ncell=ngrid2;
    header.timestep=header.nstep=header.nbands=1;
    header.firstyear=1901;
    header.nyear=1;
    header.scalar=1;
    header.order=CELLYEAR;
    header.cellsize_lon=lon;
    header.cellsize_lat=lat;
    fprintjson(file,argv[iarg+3],NULL,NULL,arglist,&header,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL,&grid_name,grid_type,RAW,NULL,FALSE,0);
    fclose(file);
  }
  return EXIT_SUCCESS;
}  /* of 'main' */
