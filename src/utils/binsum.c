/**************************************************************************************/
/**                                                                                \n**/
/**                  b  i  n  s  u  m   .  c                                       \n**/
/**                                                                                \n**/
/**     Aggregates binary output files                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-clm] [-nitem n] [-nsum n] [-month|day] [-swap] [-mean] [gridfile] binfile sumfile\n"

int main(int argc,char **argv)
{
  FILE *file,*out;
  float *data;
  float *data_sum;
  int i,j,ngrid,iarg,nitem,nsum,nyear,version;
  char *endptr;
  Header header;
  Bool swap,mean,isclm;
  swap=mean=isclm=FALSE;
  nitem=1;
  nsum=NMONTH;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-swap"))
        swap=TRUE;
      else if(!strcmp(argv[iarg],"-clm"))
        isclm=TRUE;
      else if(!strcmp(argv[iarg],"-mean"))
        mean=TRUE;
      else if(!strcmp(argv[iarg],"-nitem"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-nitem'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        nitem=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-nitem'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(nitem<=0)
        {
          fputs("Error: Number of items must be greater than zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-nsum"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-nsum'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        nsum=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-nsum'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(nsum<=0)
        {
          fputs("Error: Number of sums must be greater than zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-day"))
        nsum=NDAYYEAR;
      else if(!strcmp(argv[iarg],"-month"))
        nsum=NMONTH;
      else
      {
        fprintf(stderr,"Error: Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+((isclm) ? 2 : 3))
  {
    fprintf(stderr,"Error: Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(!isclm)
  {
    file=fopen(argv[iarg],"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening grid file '%s': %s.\n",argv[iarg],
              strerror(errno));
      return EXIT_FAILURE;
    }
    ngrid=getfilesizep(file)/sizeof(short)/2;
    if(ngrid==0)
    {
       fprintf(stderr,"Error: Number of grid cells in '%s' is zero.\n",argv[iarg]);
       return EXIT_FAILURE;
    }
    fclose(file);
    iarg++;
  }
  file=fopen(argv[iarg],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg],strerror(errno));
    return EXIT_FAILURE;
  }
  out=fopen(argv[iarg+1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[iarg+1],strerror(errno));
    return EXIT_FAILURE;
  }
  if(isclm)
  {
    version=READ_VERSION;
    if(freadheader(file,&header,&swap,LPJOUTPUT_HEADER,&version,TRUE))
    {
      return EXIT_FAILURE;
    }
    ngrid=header.ncell;
    nsum=header.nstep;
    nyear=header.nyear*nsum;
    nitem=header.nbands;
    if(getfilesizep(file)!=headersize(LPJOUTPUT_HEADER,version)+typesizes[header.datatype]*header.nyear*header.nstep*header.nbands*header.ncell)
      fprintf(stderr,"Warning: file size of '%s' does not match header.\n",argv[iarg]);
    header.nstep=1;
    fwriteheader(out,&header,LPJOUTPUT_HEADER,version);
  }
  else
  {
    nyear=getfilesizep(file)/sizeof(float)/ngrid/nitem;
    if(getfilesizep(file) % (sizeof(float)*ngrid*nitem))
      fprintf(stderr,"Warning: file size of '%s' is not multiple of bands %d and number of cells %d.\n",argv[iarg],nitem,ngrid);
  }
  data=newvec(float,ngrid*nitem);
  if(data==NULL)
  {
    printallocerr("data");
    return EXIT_FAILURE;
  }
  data_sum=newvec(float,ngrid*nitem);
  if(data_sum==NULL)
  {
    printallocerr("data");
    return EXIT_FAILURE;
  }
  for(i=0;i<nyear;i++)
  {
    if(i % nsum==0)
      for(j=0;j<ngrid*nitem;j++)
        data_sum[j]=0;
    if(freadfloat(data,ngrid*nitem,swap,file)!=ngrid*nitem)
    {
      fprintf(stderr,"Error reading data in year %d.\n",i);
      return EXIT_FAILURE;
    }
    if(mean && nsum==NMONTH)
      for(j=0;j<ngrid*nitem;j++)
        data_sum[j]+=data[j]*ndaymonth[i % NMONTH];
    else
      for(j=0;j<ngrid*nitem;j++)
        data_sum[j]+=data[j];
    if((i+1) % nsum==0)
    {
      if(mean)
      {
        if(nsum==NMONTH)
          for(j=0;j<ngrid*nitem;j++)
            data_sum[j]/=NDAYYEAR;
        else
          for(j=0;j<ngrid*nitem;j++)
            data_sum[j]/=nsum;
      }
      if(fwrite(data_sum,sizeof(float),ngrid*nitem,out)!=ngrid*nitem)
      {
        fprintf(stderr,"Error writing data in year %d.\n",i/nsum);
        return EXIT_FAILURE;
      }
    }
  }
  fclose(file);
  fclose(out);
  return EXIT_SUCCESS;
} /* of 'main' */
