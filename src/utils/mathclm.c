/**************************************************************************************/
/**                                                                                \n**/
/**                      m  a  t  h  c  l  m  .  c                                 \n**/
/**                                                                                \n**/
/**     Performs mathematical operations with CLM files                            \n**/
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
  Header header1,header2,header3;
  int version,yr,cell,k;
  Bool swap1,swap2;
  String id;
  float *data1,*data2,*data3;
  float value;
  char *endptr;
  Bool isvalue;
  enum {ADD,SUB,MUL,DIV,AVG,MAX,MIN} op;
  FILE *in1,*in2,*out;
  if(argc<5)
  {
    fprintf(stderr,"Missing argument(s).\n"
            "Usage: %s {add|sub|mul|div|avg|max|min} infile1.clm {infile2.clm|value} outfile.clm\n",argv[0]);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[1],"add"))
    op=ADD;
  else if(!strcmp(argv[1],"sub"))
    op=SUB;
  else if(!strcmp(argv[1],"mul"))
    op=MUL;
  else if(!strcmp(argv[1],"div"))
    op=DIV;
  else if(!strcmp(argv[1],"avg"))
    op=AVG;
  else if(!strcmp(argv[1],"max"))
    op=MAX;
  else if(!strcmp(argv[1],"min"))
    op=MIN;
  else
  {
    fprintf(stderr,"Invalid operator '%s'.\n",argv[1]); 
    return EXIT_FAILURE;
  }
  in1=fopen(argv[2],"rb");
  if(in1==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[2],strerror(errno));
    return EXIT_FAILURE;
  }
  version=READ_VERSION; 
  if(freadanyheader(in1,&header1,&swap1,id,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  value=(float)strtod(argv[3],&endptr);
  if(*endptr=='\0')
    isvalue=TRUE;
  else
  {
    isvalue=FALSE;
    in2=fopen(argv[3],"rb");
    if(in2==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s.\n",argv[3],strerror(errno));
      return EXIT_FAILURE;
    }
    version=READ_VERSION; 
    if(freadheader(in2,&header2,&swap2,id,&version))
    {
      fprintf(stderr,"Error reading header in '%s'.\n",argv[3]);
      return EXIT_FAILURE;
    }
    if(header1.nyear!=header2.nyear)
    {
      fprintf(stderr,"nyear %d differs from %d.\n",header1.nyear,header2.nyear);
      return EXIT_FAILURE;
    }
    if(header1.nbands!=header2.nbands)
    {
      fprintf(stderr,"nbands %d differs from %d.\n",header1.nbands,header2.nbands);
      return EXIT_FAILURE;
    }
    if(header1.firstyear!=header2.firstyear)
    {
      fprintf(stderr,"firstyear %d differs from %d.\n",header1.firstyear,header2.firstyear);
      return EXIT_FAILURE;
    }
    if(header1.ncell!=header2.ncell)
    {
      fprintf(stderr,"ncell %d differs from %d.\n",header1.ncell,header2.ncell);
      return EXIT_FAILURE;
    }
    if(header1.firstcell!=header2.firstcell)
    {
      fprintf(stderr,"firstcell %d differs from %d.\n",header1.firstcell,header2.firstcell);
      return EXIT_FAILURE;
    }
    if(header1.cellsize_lon!=header2.cellsize_lon)
    {
      fprintf(stderr,"cellsize %g differs from %g.\n",header1.cellsize_lon,header2.cellsize_lon);
      return EXIT_FAILURE;
    }
    if(header1.cellsize_lat!=header2.cellsize_lat)
    {
      fprintf(stderr,"cellsize %g differs from %g.\n",header1.cellsize_lat,header2.cellsize_lat);
      return EXIT_FAILURE;
    }
    data2=newvec(float,header1.nbands);
    check(data2);
  }
  data1=newvec(float,header1.nbands);
  check(data1);
  data3=newvec(float,header1.nbands);
  check(data3);
  out=fopen(argv[4],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[4],strerror(errno));
    return EXIT_FAILURE;
  }
  header3=header1;
  header3.scalar=1;
  header3.datatype=LPJ_FLOAT; 
  fwriteheader(out,&header3,id,3);
  for(yr=0;yr<header1.nyear;yr++)
    for(cell=0;cell<header1.ncell;cell++)
    {
      if(readfloatvec(in1,data1,header1.scalar,header1.nbands,swap1,header1.datatype))
      {
        fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                argv[2],yr+header1.firstyear);
        return EXIT_FAILURE;
      }
      if(isvalue)
        switch(op)
        {
          case ADD:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]+value;
            break;
          case SUB:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]-value;
            break;
          case MUL:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]*value;
            break;
          case DIV:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]/value;
            break;
          case AVG:
            for(k=0;k<header1.nbands;k++)
              data3[k]=(data1[k]+value)*0.5f;
            break;
          case MAX:
            for(k=0;k<header1.nbands;k++)
              data3[k]=max(data1[k],value);
            break;
          case MIN:
            for(k=0;k<header1.nbands;k++)
              data3[k]=min(data1[k],value);
            break;
        } /* of switch */
      else
      {
        if(readfloatvec(in2,data2,header2.scalar,header1.nbands,swap2,header2.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[3],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        switch(op)
        {
          case ADD:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]+data2[k];
            break;
          case SUB:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]-data2[k];
            break;
          case MUL:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]*data2[k];
            break;
          case DIV:
            for(k=0;k<header1.nbands;k++)
              data3[k]=data1[k]/data2[k];
            break;
          case AVG:
            for(k=0;k<header1.nbands;k++)
              data3[k]=(data1[k]+data2[k])*0.5f;
            break;
          case MAX:
            for(k=0;k<header1.nbands;k++)
              data3[k]=max(data1[k],data2[k]);
            break;
          case MIN:
            for(k=0;k<header1.nbands;k++)
              data3[k]=min(data1[k],data2[k]);
            break;
        } /* of switch */
      }
      /* write float data to file */
      if(fwrite(data3,sizeof(float),header1.nbands,out)!=header1.nbands)
      {
        fprintf(stderr,"Error writing '%s' in year %d.\n",
                argv[4],yr+header1.firstyear);
        return EXIT_FAILURE;
      }
    }
  fclose(in1);
  if(!isvalue)
    fclose(in2);
  fclose(out);
  return EXIT_FAILURE;
} /* of 'main' */
