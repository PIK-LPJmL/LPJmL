/**************************************************************************************/
/**                                                                                \n**/
/**                         m  e  r  g  e  c  l  m  .  c                           \n**/
/**                                                                                \n**/
/**     Program merges N CLM files to one CLM file with N bands                    \n**/
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

#define USAGE "Usage: mergeclm [-f] [-longheader] in1.clm|zero [in2.clm ...] out.clm\n"

int main(int argc,char **argv)
{
  FILE **files;
  FILE *out;
  int i,iarg,year,cell,version,version_out,*nbands,setversion,numfiles,nbands_sum,index;
  Byte *bvec;
  short *svec;
  int *ivec;
  float *fvec;
  void *vec;
  struct stat filestat;
  char c;
  int rc;
  Bool force,first;
  Header header,header_out;
  Bool *swap;
  String id,id_out;
  /* set default values */
  force=FALSE;
  setversion=READ_VERSION;
  /* process command options */
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-f"))
        force=TRUE;
      else if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n"
                USAGE,argv[iarg]);
        return EXIT_FAILURE;
      }
    }
    else
      break;

  /* calculate nbands from number of remaining arguments, last one is name of output file */
  numfiles=argc-iarg-1;
  if(numfiles<=0)
  {
    fputs("Input file missing.\n"
            USAGE,stderr);
    return EXIT_FAILURE;
  }
  files=newvec(FILE *,numfiles);
  if(files==NULL)
  {
    printallocerr("files");
    return EXIT_FAILURE;
  }
  swap=newvec(Bool,numfiles);
  if(swap==NULL)
  {
    printallocerr("swap");
    return EXIT_FAILURE;
  }
  nbands=newvec(Bool,numfiles);
  if(nbands==NULL)
  {
    printallocerr("nbands");
    return EXIT_FAILURE;
  }
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
    fprintf(stderr,"Cannot create '%s': %s.\n",argv[argc-1],strerror(errno));
    return EXIT_FAILURE;
  }
  /* open input files and check file size for consistency */
  first=TRUE;
  nbands_sum=0;
  for(i=0;i<numfiles;i++)
  {
    if(!strcmp(argv[iarg+i],"zero"))
    {
      files[i]=NULL;
      nbands[i]=1;
    }
    else
    {
      files[i]=fopen(argv[iarg+i],"rb");
      if(files[i]==NULL)
      {
        fprintf(stderr,"Cannot open '%s': %s.\n",argv[iarg+i],strerror(errno));
        return EXIT_FAILURE;
      }
      version=setversion;
      if(freadanyheader(files[i],&header,swap+i,id,&version))
      {
        fprintf(stderr,"Error reading header in '%s'.\n",argv[i+iarg]);
        return EXIT_FAILURE;
      }
      nbands[i]=header.nbands;
      nbands_sum+=nbands[i];
      if(first)
      {
        header_out=header;
        strcpy(id_out,id);
        version_out=version;
        first=FALSE;
      }
      else
      {
        if(header.ncell!=header_out.ncell)
        {
          fprintf(stderr,"Number of cells=%d in file '%s' differs from %d.\n",
                  header.ncell,argv[i+iarg],header_out.ncell);
          return EXIT_FAILURE;
        }
        else if(header.nyear!=header_out.nyear)
        {
          fprintf(stderr,"Number of years=%d in file '%s' differs from %d.\n",
                  header.nyear,argv[i+iarg],header_out.nyear);
          return EXIT_FAILURE;
        }
        else if(header.firstyear!=header_out.firstyear)
        {
          fprintf(stderr,"First year=%d in file '%s' differs from %d.\n",
                  header.firstyear,argv[i+iarg],header_out.firstyear);
          return EXIT_FAILURE;
        }
        else if(header.datatype!=header_out.datatype)
        {
          fprintf(stderr,"Data type=%s in file '%s' differs from %s.\n",
                  typenames[header.datatype],argv[i+iarg],typenames[header_out.datatype]);
          return EXIT_FAILURE;
        }
        else if(header.cellsize_lon!=header_out.cellsize_lon)
        {
          fprintf(stderr,"Cell size=%g in file '%s' differs from %g.\n",
                  header.cellsize_lon,argv[i+iarg],header_out.cellsize_lon);
          return EXIT_FAILURE;
        }
        else if(header.scalar!=header_out.scalar)
        {
          fprintf(stderr,"Scaling=%g in file '%s' differs from %g.\n",
                  header.scalar,argv[i+iarg],header_out.scalar);
          return EXIT_FAILURE;
        }
      }
      /* check file size */
      fstat(fileno(files[i]),&filestat);
      if(filestat.st_size!=header.ncell*header.nyear*nbands[i]*typesizes[header_out.datatype]+headersize(id,version))
        fprintf(stderr,"Warning: file size of '%s' does not match setting of nyear=%d and ncell=%d.\n",
                argv[iarg+i],header.nyear,header.ncell);
    }
  }
  printf("Number of nbands: %d\n",nbands_sum);
  header_out.nbands=nbands_sum;
  fwriteheader(out,&header_out,id_out,version_out);
  switch(header_out.datatype)
  {
    case LPJ_BYTE:
      vec=bvec=newvec(Byte,nbands_sum);
      break;
    case LPJ_SHORT:
      vec=svec=newvec(short,nbands_sum);
      break;
    case LPJ_INT:
      vec=ivec=newvec(int,nbands_sum);
      break;
    case LPJ_FLOAT:
      vec=fvec=newvec(float,nbands_sum);
      break;
  }
  if(vec==NULL)
  {
    printallocerr("vec");
    return EXIT_FAILURE;
  }
  for(year=0;year<header_out.nyear;year++)
    for(cell=0;cell<header_out.ncell;cell++)
    {
      index=0;
      for(i=0;i<numfiles;i++)
      {
        rc=1;
        switch(header_out.datatype)
        {
          case LPJ_BYTE:
            if(files[i]==NULL)
              bvec[index]=0;
            else
              rc=fread(bvec+index,1,nbands[i],files[i]);
            break;
          case LPJ_SHORT:
            if(files[i]==NULL)
              svec[index]=0;
            else
              rc=freadshort(svec+index,nbands[i],swap[i],files[i]);
            break;
          case LPJ_INT:
            if(files[i]==NULL)
              ivec[index]=0;
            else
              rc=freadint(ivec+index,nbands[i],swap[i],files[i]);
            break;
          case LPJ_FLOAT:
            if(files[i]==NULL)
              fvec[index]=0;
            else
              rc=freadfloat(fvec+index,nbands[i],swap[i],files[i]);
            break;
        }
        if(rc!=nbands[i])
        {
          fprintf(stderr,"Unexpected end of file in '%s' at year %d and cell %d.\n",argv[iarg+i],year+header_out.firstyear,cell);
          return EXIT_FAILURE;
        }
        index+=nbands[i];
      }
      /* write merged data */
      if(fwrite(vec,typesizes[header_out.datatype],nbands_sum,out)!=nbands_sum)
      {
        fprintf(stderr,"Error writing '%s': %s.\n",argv[argc-1],strerror(errno));
        return EXIT_FAILURE;
      }
    }
   /* close all open files */
  for(i=0;i<numfiles;i++)
    if(files[i]!=NULL)
      fclose(files[i]);
  fclose(out);
  return EXIT_SUCCESS;
} /* of 'main' */
