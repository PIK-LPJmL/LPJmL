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

#define USAGE "Usage: %s [-longheader] [-type {byte|short|int|float|double}] {add|sub|mul|div|avg|max|min|float|int} infile1.clm [{infile2.clm|value}] outfile.clm\n"

int main(int argc,char **argv)
{
  Header header1,header2,header3;
  int version,yr,cell,k;
  Bool swap1,swap2;
  String id;
  float *data1,*data2,*data3;
  int *idata1,*idata2,*idata3;
  float value;
  int iarg,ivalue,setversion;
  char *endptr;
  size_t size;
  Type type;
  int index;
  Bool isvalue,intvalue,isint;
  enum {ADD,SUB,MUL,DIV,AVG,MAX,MIN,FLOAT,INT} op;
  FILE *in1,*in2,*out;
  setversion=READ_VERSION;
  index=NOT_FOUND;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[iarg],"-type"))
      {
        if(argc-1==iarg)
        {
          fprintf(stderr,"Argument missing for option '-type'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        index=findstr(argv[++iarg],typenames,5);
        if(index==NOT_FOUND)
        {
          fprintf(stderr,"Invalid argument for option '-type'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        type=(Type)index;
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n",argv[iarg]);
        fprintf(stderr,USAGE,argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+3)
  {
    fprintf(stderr,"Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[iarg],"add"))
    op=ADD;
  else if(!strcmp(argv[iarg],"sub"))
    op=SUB;
  else if(!strcmp(argv[iarg],"mul"))
    op=MUL;
  else if(!strcmp(argv[iarg],"div"))
    op=DIV;
  else if(!strcmp(argv[iarg],"avg"))
    op=AVG;
  else if(!strcmp(argv[iarg],"max"))
    op=MAX;
  else if(!strcmp(argv[iarg],"min"))
    op=MIN;
  else if(!strcmp(argv[iarg],"float"))
    op=FLOAT;
  else if(!strcmp(argv[iarg],"int"))
    op=INT;
  else
  {
    fprintf(stderr,"Invalid operator '%s'.\n",argv[iarg]);
    fprintf(stderr,USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(op!=FLOAT && op!=INT && argc<iarg+4)
  {
    fprintf(stderr,"Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  in1=fopen(argv[iarg+1],"rb");
  if(in1==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+1],strerror(errno));
    return EXIT_FAILURE;
  }
  version=setversion;
  if(freadanyheader(in1,&header1,&swap1,id,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[iarg+1]);
    return EXIT_FAILURE;
  }
  if(index!=NOT_FOUND)
    header1.datatype=type;
  size=getfilesizep(in1)-headersize(id,version);
  if(size!=(long long)header1.nyear*header1.ncell*header1.nbands*typesizes[header1.datatype])
    fprintf(stderr,"Warning: File size of '%s' does not match nbands*ncell*nyear.\n",argv[iarg+1]);
  if(op!=FLOAT && op!=INT)
  {
    value=(float)strtod(argv[iarg+2],&endptr);
    if(*endptr=='\0')
    {
      ivalue=(float)strtol(argv[iarg+2],&endptr,10);
      intvalue=(*endptr=='\0');
      isvalue=TRUE;
      if(intvalue && ivalue==0 && op==DIV)
      {
        fprintf(stderr,"Value must not be zero for div operator.\n");
        return EXIT_FAILURE;
      }
    }
    else
    {
      isvalue=FALSE;
      in2=fopen(argv[iarg+2],"rb");
      if(in2==NULL)
      {
        fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
        return EXIT_FAILURE;
      }
      version=setversion;
      if(freadheader(in2,&header2,&swap2,id,&version))
      {
        fprintf(stderr,"Error reading header in '%s'.\n",argv[iarg+2]);
        return EXIT_FAILURE;
      }
      if(index!=NOT_FOUND)
        header2.datatype=type;
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
      size=getfilesizep(in2)-headersize(id,version);
      if(size!=(long long)header2.nyear*header2.ncell*header1.nbands*typesizes[header2.datatype])
        fprintf(stderr,"Warning: File size of '%s' does not match nbands*ncell*nyear.\n",argv[iarg+2]);
    }
    if(isvalue)
      isint=(intvalue && header1.datatype==LPJ_INT && header1.scalar==1);
    else
      isint=(header1.datatype==LPJ_INT && header2.datatype==LPJ_INT && header1.scalar==1 && header2.scalar==1);
    if(isint)
    {
      idata1=newvec(int,header1.nbands);
      check(idata1);
      idata3=newvec(int,header1.nbands);
      check(idata3);
      if(!isvalue)
      {
        idata2=newvec(int,header1.nbands);
        check(idata2);
      }
    }
    else
    {
      data1=newvec(float,header1.nbands);
      check(data1);
      data3=newvec(float,header1.nbands);
      check(data3);
      if(!isvalue)
      {
        data2=newvec(float,header1.nbands);
        check(data2);
      }
    }
  }
  else if(op==INT)
  {
    idata1=newvec(int,header1.nbands);
    check(idata1);
  }
  else
  {
    data1=newvec(float,header1.nbands);
    check(data1);
  }
  out=fopen(argv[iarg+((op==FLOAT || op==INT) ? 2 : 3)],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[iarg+((op==FLOAT) ? 2 : 3)],strerror(errno));
    return EXIT_FAILURE;
  }
  header3=header1;
  if(op==INT)
  {
    header3.datatype=LPJ_INT;
    header3.scalar=header1.scalar;
  }
  else
  {
    header3.scalar=1;
    header3.datatype=(op!=FLOAT && isint) ? LPJ_INT : LPJ_FLOAT;
  }
  fwriteheader(out,&header3,id,3);
  if(op==FLOAT)
  {
    for(yr=0;yr<header1.nyear;yr++)
      for(cell=0;cell<header1.ncell;cell++)
      {
        if(readfloatvec(in1,data1,header1.scalar,header1.nbands,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        if(fwrite(data1,sizeof(float),header1.nbands,out)!=header1.nbands)
        {
          fprintf(stderr,"Error writing '%s' in year %d.\n",
                  argv[iarg+2],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
      }
  }
  else if(op==INT)
  {
    for(yr=0;yr<header1.nyear;yr++)
      for(cell=0;cell<header1.ncell;cell++)
      {
        if(readintvec(in1,idata1,header1.nbands,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        if(fwrite(idata1,sizeof(int),header1.nbands,out)!=header1.nbands)
        {
          fprintf(stderr,"Error writing '%s' in year %d.\n",
                  argv[iarg+2],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
      }
  }
  else if(isint)
    for(yr=0;yr<header1.nyear;yr++)
      for(cell=0;cell<header1.ncell;cell++)
      {
        if(readintvec(in1,idata1,header1.nbands,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        if(isvalue)
          switch(op)
          {
            case ADD:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=idata1[k]+ivalue;
              break;
            case SUB:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=idata1[k]-ivalue;
              break;
            case MUL:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=idata1[k]*ivalue;
              break;
            case DIV:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=idata1[k]/ivalue;
              break;
            case AVG:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=(idata1[k]+ivalue)/2;
              break;
            case MAX:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=max(idata1[k],ivalue);
              break;
            case MIN:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=min(idata1[k],ivalue);
              break;
          } /* of switch */
        else
        {
          if(readintvec(in2,idata2,header1.nbands,swap2,header2.datatype))
          {
            fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                    argv[iarg+2],yr+header1.firstyear);
            return EXIT_FAILURE;
          }
          switch(op)
          {
            case ADD:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=idata1[k]+idata2[k];
              break;
            case SUB:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=idata1[k]-idata2[k];
              break;
            case MUL:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=idata1[k]*idata2[k];
              break;
            case DIV:
              for(k=0;k<header1.nbands;k++)
              {
                if(idata2[k]==0)
                {
                  fprintf(stderr,"Value is zero for year %d and band %d.\n",
                          yr+header1.firstyear,k+1);
                  return EXIT_FAILURE;
                }
                idata3[k]=idata1[k]/idata2[k];
              }
              break;
            case AVG:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=(idata1[k]+idata2[k])/2;
              break;
            case MAX:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=max(idata1[k],idata2[k]);
              break;
            case MIN:
              for(k=0;k<header1.nbands;k++)
                idata3[k]=min(idata1[k],idata2[k]);
              break;
          } /* of switch */
        }
        /* write int data to file */
        if(fwrite(idata3,sizeof(int),header1.nbands,out)!=header1.nbands)
        {
          fprintf(stderr,"Error writing '%s' in year %d.\n",
                  argv[iarg+3],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
      }
  else
    for(yr=0;yr<header1.nyear;yr++)
      for(cell=0;cell<header1.ncell;cell++)
      {
        if(readfloatvec(in1,data1,header1.scalar,header1.nbands,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
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
                    argv[iarg+2],yr+header1.firstyear);
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
                  argv[iarg+3],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
      }
  fclose(in1);
  if(op!=FLOAT && !isvalue)
    fclose(in2);
  fclose(out);
  return EXIT_FAILURE;
} /* of 'main' */
