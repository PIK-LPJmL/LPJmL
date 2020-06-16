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

#define USAGE "Usage: catclm [-longheader] [-size4] infile1.clm [infile2.clm ...] outfile.clm\n"

int main(int argc,char **argv)
{
  Header header,oldheader;
  String id;
  int i,j,k,firstyear,version,n,setversion,index,firstversion;
  FILE *in,*out;
  short *values;
  int *ivals;
  Byte *bvals;
  long long *lvals;
  Bool swap;
  size_t size,filesize;
  size=2;
  setversion=READ_VERSION;
  for(index=1;index<argc;index++)
    if(argv[index][0]=='-')
    {
      if(!strcmp(argv[index],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[index],"-size4"))
        size=4;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[index]);
        return EXIT_FAILURE;
      } 
    }
    else
      break;
  if(argc<index+2)
  { 
    fprintf(stderr,"Error: Argument(s) missing.\n"
                   USAGE);
    return EXIT_FAILURE;
  }
  n=argc-index-1;
  out=fopen(argv[argc-1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[argc-1],strerror(errno));
    return EXIT_FAILURE;
  }
  for(i=0;i<n;i++)
  {
    in=fopen(argv[index+i],"rb");
    if(in==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[index+i],strerror(errno));
      return EXIT_FAILURE;
    }
    if(i)
    {
      version=setversion;
      if(freadheader(in,&header,&swap,id,&version))
      {
        fprintf(stderr,"Error reading header in '%s'.\n",argv[i+index]);
        return EXIT_FAILURE;
      }
      filesize=getfilesize(argv[index+i])-headersize(id,version);
      if(filesize!=((version==3) ? typesizes[header.datatype] : size)*header.ncell*header.nbands*header.nyear)
      {
        fprintf(stderr,"Error: file length of '%s' does not match header.\n",argv[index+i]);
        return EXIT_FAILURE;
      }
      if(version==3 && header.datatype!=oldheader.datatype)
      {
        fprintf(stderr,"Error: Different datatype in '%s'.\n",argv[i+index]);
        return EXIT_FAILURE;
      }
      if(header.order!=oldheader.order)
      {
        fprintf(stderr,"Error: Different order in '%s'.\n",argv[i+index]);
        return EXIT_FAILURE;
      }
      if(header.ncell!=oldheader.ncell)
      {
        fprintf(stderr,"Error: Different number of cells in '%s'.\n",argv[i+index]);
        return EXIT_FAILURE;
      }
      if(header.firstcell!=oldheader.firstcell)
      {
        fprintf(stderr,"Error: Different index of first cell in '%s'.\n",argv[i+index]);
        return EXIT_FAILURE;
      }
      if(header.nbands!=oldheader.nbands)
      {
        fprintf(stderr,"Error: Different number of bands in '%s'.\n",argv[i+index]);
        return EXIT_FAILURE;
      }
      if(header.firstyear!=oldheader.firstyear+oldheader.nyear)
      {
        fprintf(stderr,"Error: First year=%d in '%s' is different from %d.\n",header.firstyear,argv[i+index],oldheader.firstyear+oldheader.nyear);
        return EXIT_FAILURE;
      }
    }
    else
    {
      version=setversion;
      if(freadanyheader(in,&header,&swap,id,&version))
      {
        fprintf(stderr,"Error reading header in '%s'.\n",argv[i+index]);
        return EXIT_FAILURE;
      }
      filesize=getfilesize(argv[index])-headersize(id,version);
      if(filesize!=((version==3) ? typesizes[header.datatype] : size)*header.ncell*header.nbands*header.nyear)
      {
        fprintf(stderr,"Error: file length of '%s' does not match header.\n",argv[index]);
        return EXIT_FAILURE;
      }
      fseek(out,headersize(id,version),SEEK_SET);
      firstyear=header.firstyear;
      firstversion=version;
    }
    if(version==3)
      size=typesizes[header.datatype];
    switch(size)
    {
      case 1:
        bvals=newvec(Byte,header.nbands*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(bvals,1,header.nbands*header.ncell,in)!=header.nbands*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+index]);
            return EXIT_FAILURE;
          }
          if(fwrite(bvals,1,header.nbands*header.ncell,out)!=header.nbands*header.ncell)
          {
            fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
            return EXIT_FAILURE;
          }
        }
        free(bvals);
        break;
      case 2:
        values=newvec(short,header.nbands*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(values,sizeof(short),header.nbands*header.ncell,in)!=header.nbands*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+index]);
            return EXIT_FAILURE;
          }
          if(swap)
            for(k=0;k<header.nbands*header.ncell;k++)
              values[k]=swapshort(values[k]);
          if(fwrite(values,sizeof(short),header.nbands*header.ncell,out)!=header.nbands*header.ncell)
          {
            fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
            return EXIT_FAILURE;
          }
        }
        free(values);
        break;
      case 4:
        ivals=newvec(int,header.nbands*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(ivals,sizeof(int),header.nbands*header.ncell,in)!=header.nbands*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+index]);
            return EXIT_FAILURE;
          }
          if(swap)
            for(k=0;k<header.nbands*header.ncell;k++)
              ivals[k]=swapint(ivals[k]);
          if(fwrite(ivals,sizeof(int),header.nbands*header.ncell,out)!=header.nbands*header.ncell)
          {
            fprintf(stderr,"Error writing to '%s'.\n",argv[argc-1]);
            return EXIT_FAILURE;
          }
        }
        free(ivals);
        break;
      case 8:
        lvals=newvec(long long,header.nbands*header.ncell);
        for(j=0;j<header.nyear;j++)
        {
          if(fread(lvals,sizeof(long long),header.nbands*header.ncell,in)!=header.nbands*header.ncell)
          {
            fprintf(stderr,"Error reading from '%s'.\n",argv[i+index]);
            return EXIT_FAILURE;
          }
          if(swap)
            for(k=0;k<header.nbands*header.ncell;k++)
              lvals[k]=swaplong(lvals[k]);
          if(fwrite(lvals,sizeof(long long),header.nbands*header.ncell,out)!=header.nbands*header.ncell)
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
