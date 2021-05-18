/**************************************************************************************/
/**                                                                                \n**/
/**                    r  e  g  r  i  d  c  l  m  .  c                             \n**/
/**                                                                                \n**/
/**     CLM data is regridded to new grid file                                     \n**/
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

#define USAGE "Usage: %s [-size4] [-search] [-zero] [-longheader] coord_old.clm coord_new.clm data_old.clm data_new.clm\n"

int main(int argc,char **argv)
{
  FILE *file,*data_file;
  Header header,header2;
  Coord *c,*c2;
  Real dist_min;
  Bool swap,isint,issearch,iszero;
  short *data;
  int *zero;
  int *idata;
  long long size;
  Coord res,res2;
  Coordfile grid;
  int i,j,*index,data_version,setversion,ngrid,ngrid2;
  float lon,lat,*fzero;
  String id;
  Filename filename;
  isint=issearch=iszero=FALSE;
  setversion=READ_VERSION;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-size4"))
        isint=TRUE;
      else if(!strcmp(argv[i],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[i],"-search"))
        issearch=TRUE;
      else if(!strcmp(argv[i],"-zero"))
        iszero=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[i],argv[0]);
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
            USAGE,argv[1-i]);
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
  if(freadanyheader(data_file,&header,&swap,id,&data_version))
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
  size=getfilesize(argv[3])-headersize(id,data_version);
  if(data_version==3)
    isint=header.datatype==LPJ_INT || header.datatype==LPJ_FLOAT;
  if(iszero)
  {
    zero=newvec(int,header.nbands);
    if(header.datatype==LPJ_FLOAT)
    {
      fzero=(float *)zero;
      for(i=0;i<header.nbands;i++)
        fzero[i]=0;
    }
    else
      for(i=0;i<header.nbands;i++)
        zero[i]=0;
  }
  if(isint)
  {
    idata=newvec(int,(long long)header.ncell*header.nbands);
    if(idata==NULL)
    {
      printallocerr("idata");
      return EXIT_FAILURE;
    }
    if(size!=(long long)header.ncell*header.nyear*header.nbands*sizeof(int))
    {
      if((long long)header.ncell*header.nyear*header.nbands*sizeof(short)==size)
      {
        fprintf(stderr,"File size of '%s' does not match header, set datatype to 2 byte size.\n",argv[3]);  
        isint=FALSE;
        free(idata);
        data=newvec(short,(long long)header.ncell*header.nbands);
        if(data==NULL)
        {
          printallocerr("data");
          return EXIT_FAILURE;
        }
      }
      else
      {
        header.nyear=size/(sizeof(int)*header.ncell*header.nbands);
        fprintf(stderr,"File size of '%s' does not match header, number of years set to %d.\n",argv[3],header.nyear);
      }
    }
  }
  else
  {
    data=newvec(short,(long long)header.ncell*header.nbands);
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
    if(size!=(long long)header.ncell*header.nyear*header.nbands*sizeof(short))
    {
      if((long long)header.ncell*header.nyear*header.nbands*sizeof(int)==size)
      {
        fprintf(stderr,"File size of '%s' does not match header, set datatype to 4 byte size.\n",argv[3]);  
        isint=TRUE;
        free(data);
        idata=newvec(int,(long long)header.ncell*header.nbands);
        if(idata==NULL)
        {
          printallocerr("idata");
          return EXIT_FAILURE;
        }
      }
      else
      {
        header.nyear=size/(sizeof(short)*header.ncell*header.nbands);
        fprintf(stderr,"File size of '%s'does not match header, number of years set to %d.\n",argv[3],header.nyear);
      }
    }
  }
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
  index=newvec(int,ngrid2);
  if(index==NULL)
  {
    printallocerr("index");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid2;i++)
  {
    j=findcoord(c2+i,c,ngrid);
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
        fputs(" not found.\n",stderr);
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
      if(freadint(idata,(long long)header.ncell*header.nbands,swap,data_file)!=(long long)header.ncell*header.nbands)
      {
        fprintf(stderr,"Error reading '%s' in year %d.\n",argv[3],header2.firstyear+i);
        return EXIT_FAILURE;
      }
      for(j=0;j<header2.ncell;j++)
      {
        if(index[j]==-1)
          fwrite(zero,sizeof(int),header.nbands,file);
        else
        {
          if(fwrite(idata+index[j]*header.nbands,sizeof(int),header.nbands,file)!=header.nbands)
            fprintf(stderr,"Error writing file '%s: %s.\n",argv[4],strerror(errno));
        }
      }
    }
  else
    for(i=0;i<header2.nyear;i++)
    {
      if(freadshort(data,(long long)header.ncell*header.nbands,swap,data_file)!=(long long)header.ncell*header.nbands)
      {
        fprintf(stderr,"Error reading '%s' in year %d.\n",argv[3],header2.firstyear+i);
        return EXIT_FAILURE;
      }
      for(j=0;j<header2.ncell;j++)
      {
        if(index[j]==-1)
          fwrite(zero,sizeof(short),header.nbands,file);
        else
        {
          if(fwrite(data+index[j]*header.nbands,sizeof(short),header.nbands,file)!=header.nbands)
            fprintf(stderr,"Error writing file '%s: %s.\n",argv[4],strerror(errno));
        }
      }
    }
  fclose(file);
  fclose(data_file);
  return EXIT_SUCCESS;
} /* of 'main' */
