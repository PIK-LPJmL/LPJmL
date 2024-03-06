/**************************************************************************************/
/**                                                                                \n**/
/**                      c  a  t  c  l  m  .  c                                    \n**/
/**                                                                                \n**/
/**     Concatenates LPJ climate data files                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include <sys/stat.h>
#define USAGE "Usage: catclm [-f] [-v] [-longheader] [-size4] infile1.clm [infile2.clm ...] outfile.clm\n"

int main(int argc,char **argv)
{
  Header header,oldheader;
  String id;
  int i,j,k,firstyear,version,n,setversion,iarg,firstversion;
  FILE *in,*out;
  short *values;
  int *ivals;
  int *index,*index2;
  Byte *bvals;
  long long *lvals;
  struct stat filestat;
  Bool swap,verbose,force;
  size_t size,filesize;
  char c;
  size=2;
  setversion=READ_VERSION;
  verbose=FALSE;
  force=FALSE;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[iarg],"-f"))
        force=TRUE;
      else if(!strcmp(argv[iarg],"-v"))
        verbose=TRUE;
      else if(!strcmp(argv[iarg],"-size4"))
        size=4;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+2)
  {
    fprintf(stderr,"Error: Argument(s) missing.\n"
                   USAGE);
    return EXIT_FAILURE;
  }
  n=argc-iarg-1;
  if(!force)
  {
    if(!stat(argv[argc-1],&filestat))
    {
      fprintf(stderr,"File '%s' already exists, overwrite (y/n)?\n",argv[argc-1]);
      scanf("%c",&c);
      if(c!='y')
        return EXIT_FAILURE;
    }
  }
  out=fopen(argv[argc-1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[argc-1],strerror(errno));
    return EXIT_FAILURE;
  }
  for(i=0;i<n;i++)
  {
    in=fopen(argv[iarg+i],"rb");
    if(in==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[iarg+i],strerror(errno));
      return EXIT_FAILURE;
    }
    if(i)
    {
      version=setversion;
      if(freadheader(in,&header,&swap,id,&version,TRUE))
      {
        fprintf(stderr,"Error reading header in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(version>CLM_MAX_VERSION)
      {
        fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
                version,argv[i+iarg],CLM_MAX_VERSION+1);
        return EXIT_FAILURE;
      }
      filesize=getfilesizep(in)-headersize(id,version);
      if((header.order==CELLINDEX && filesize!=sizeof(int)*header.ncell+((version>=3) ? typesizes[header.datatype] : size)*header.ncell*header.nbands*header.nstep*header.nyear) ||
         (header.order!=CELLINDEX && filesize!=((version>=3) ? typesizes[header.datatype] : size)*header.ncell*header.nbands*header.nstep*header.nyear))
      {
        fprintf(stderr,"Error: file length of '%s' does not match header.\n",argv[iarg+i]);
        return EXIT_FAILURE;
      }
      if(version>=3 && header.datatype!=oldheader.datatype)
      {
        fprintf(stderr,"Error: Different datatype in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(header.order!=oldheader.order)
      {
        fprintf(stderr,"Error: Different order in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(header.ncell!=oldheader.ncell)
      {
        fprintf(stderr,"Error: Different number of cells in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(header.firstcell!=oldheader.firstcell)
      {
        fprintf(stderr,"Error: Different index of first cell in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(header.nbands!=oldheader.nbands)
      {
        fprintf(stderr,"Error: Different number of bands in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(header.nstep!=oldheader.nstep)
      {
        fprintf(stderr,"Error: Different number of steps in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(header.firstyear!=oldheader.firstyear+oldheader.nyear)
      {
        fprintf(stderr,"Error: First year=%d in '%s' is different from %d.\n",header.firstyear,argv[i+iarg],oldheader.firstyear+oldheader.nyear);
        return EXIT_FAILURE;
      }
      if(header.order==CELLINDEX)
      {
        if(freadint(index2,header.ncell,swap,in)!=header.ncell)
        {
          fprintf(stderr,"Error reading cell index in '%s'.\n",argv[i+iarg]);
          return EXIT_FAILURE;
        }
        for(j=0;j<header.ncell;j++)
          if(index[j]!=index2[j])
          {
            fprintf(stderr,"Cell index different in '%s'.\n",argv[i+iarg]);
            return EXIT_FAILURE;
          }
      }
    }
    else
    {
      version=setversion;
      if(freadanyheader(in,&header,&swap,id,&version,TRUE))
      {
        fprintf(stderr,"Error reading header in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      if(version>CLM_MAX_VERSION)
      {
        fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
                version,argv[i+iarg],CLM_MAX_VERSION+1);
        return EXIT_FAILURE;
      }
      filesize=getfilesizep(in)-headersize(id,version);
      if((header.order==CELLINDEX && filesize!=sizeof(int)*header.ncell+((version>=3) ? typesizes[header.datatype] : size)*header.ncell*header.nbands*header.nstep*header.nyear) ||
         (header.order!=CELLINDEX && filesize!=((version>=3) ? typesizes[header.datatype] : size)*header.ncell*header.nbands*header.nstep*header.nyear))
      {
        fprintf(stderr,"Error: file length of '%s' does not match header.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      fseek(out,headersize(id,version),SEEK_SET);
      if(header.order==CELLINDEX)
      {
        index=newvec(int,header.ncell);
        if(index==NULL)
        {
          printallocerr("index");
          return EXIT_FAILURE;
        }
        if(freadint(index,header.ncell,swap,in)!=header.ncell)
        {
          fprintf(stderr,"Error reading cell index in '%s'.\n",argv[i+iarg]);
          return EXIT_FAILURE;
        }
        if(fwrite(index,sizeof(int),header.ncell,out)!=header.ncell)
        {
          fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
          return EXIT_FAILURE;
        }
        index2=newvec(int,header.ncell);
        if(index2==NULL)
        {
          printallocerr("index");
          return EXIT_FAILURE;
        }
      }
      firstyear=header.firstyear;
      firstversion=version;
    }
    if(verbose)
      printf("Filename: %s, %d-%d\n",argv[i+iarg],header.firstyear,header.firstyear+header.nyear-1);
    if(version>=3)
      size=typesizes[header.datatype];
    switch(size)
    {
      case 1:
        bvals=newvec(Byte,(long long)header.nbands*header.nstep*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(bvals,1,(long long)header.nbands*header.nstep*header.ncell,in)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+iarg]);
            return EXIT_FAILURE;
          }
          if(fwrite(bvals,1,(long long)header.nbands*header.nstep*header.ncell,out)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
            return EXIT_FAILURE;
          }
        }
        free(bvals);
        break;
      case 2:
        values=newvec(short,(long long)header.nbands*header.nstep*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(values,sizeof(short),(long long)header.nbands*header.nstep*header.ncell,in)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+iarg]);
            return EXIT_FAILURE;
          }
          if(swap)
            for(k=0;k<(long long)header.nbands*header.nstep*(long long)header.ncell;k++)
              values[k]=swapshort(values[k]);
          if(fwrite(values,sizeof(short),(long long)header.nbands*header.nstep*header.ncell,out)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
            return EXIT_FAILURE;
          }
        }
        free(values);
        break;
      case 4:
        ivals=newvec(int,(long long)header.nbands*header.nstep*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(ivals,sizeof(int),(long long)header.nbands*header.nstep*header.ncell,in)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+iarg]);
            return EXIT_FAILURE;
          }
          if(swap)
            for(k=0;k<(long long)header.nbands*header.nstep*header.ncell;k++)
              ivals[k]=swapint(ivals[k]);
          if(fwrite(ivals,sizeof(int),(long long)header.nbands*header.nstep*header.ncell,out)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
            return EXIT_FAILURE;
          }
        }
        free(ivals);
        break;
      case 8:
        lvals=newvec(long long,(long long)header.nbands*header.nstep*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(lvals,sizeof(long long),(long long)header.nbands*header.nstep*header.ncell,in)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+iarg]);
            return EXIT_FAILURE;
          }
          if(swap)
            for(k=0;k<(long long)header.nbands*header.nstep*header.ncell;k++)
              lvals[k]=swaplong(lvals[k]);
          if(fwrite(lvals,sizeof(long long),(long long)header.nbands*header.ncell,out)!=(long long)header.nbands*header.nstep*header.ncell)
          {
            fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
            return EXIT_FAILURE;
          }
        }
        free(lvals);
        break;

    } /* of 'switch' */
    fclose(in);
    oldheader=header;
  }
  header.nyear=header.firstyear+header.nyear-firstyear;
  header.firstyear=firstyear;
  rewind(out);
  fwriteheader(out,&header,id,firstversion);
  fclose(out);
  return EXIT_SUCCESS;
} /* of 'main' */
