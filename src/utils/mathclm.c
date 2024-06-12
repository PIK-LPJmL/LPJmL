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
#include <sys/stat.h>

#define USAGE "Usage: %s [-v] [-f] [-longheader] [-raw] [-metafile] [-json] [-type {byte|short|int|float|double}] {add|sub|mul|div|avg|max|min|repl|float|int} infile1.clm [{infile2.clm|value}] outfile.clm\n"

int main(int argc,char **argv)
{
  Header header1,header2,header3;
  int version,yr,cell,k;
  Bool swap1,swap2,flag;
  String id;
  float *data1=NULL,*data2=NULL,*data3=NULL;
  int *idata1=NULL,*idata2=NULL,*idata3=NULL;
  float value;
  int iarg,ivalue,setversion;
  char *endptr;
  size_t size;
  char *map_name=BAND_NAMES;
  char *arglist;
  char *out_json;
  size_t offset;
  Map *map=NULL,*map2=NULL;
  Attr *attrs;
  int n_attr;
  char *units=NULL,*long_name=NULL,*variable=NULL,*standard_name=NULL,*source=NULL,*history=NULL;
  char *units2=NULL;
  Type grid_type,grid_type2;
  Filename grid_name,grid_name2;
  Type type=LPJ_SHORT;
  int index,format;
  int *cell_index,*cell_index2;
  char *out_name;
  Bool isvalue=FALSE,intvalue,isint=FALSE,ismeta,israw,isjson,isforce;
  enum {ADD,SUB,MUL,DIV,AVG,MAX,MIN,REPL,FLOAT,INT} op;
  FILE *in1,*in2=NULL,*out;
  struct stat filestat;
  char c;
  setversion=READ_VERSION;
  index=NOT_FOUND;
  ismeta=israw=isjson=isforce=FALSE;
  format=CLM;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version"))
      {
        puts(LPJ_VERSION);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[iarg],"-f"))
        isforce=TRUE;
      else if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-raw"))
      {
        format=RAW;
        israw=TRUE;
      }
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
          fprintf(stderr,"Invalid argument '%s' for option '-type'.\n"
                  USAGE,argv[iarg],argv[0]);
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
  else if(!strcmp(argv[iarg],"repl"))
    op=REPL;
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
  if(ismeta)
  {
    /* set default values */
    header1.datatype=(index!=NOT_FOUND) ? type : LPJ_SHORT;
    header1.timestep=1;
    header1.nbands=1;
    header1.nstep=1;
    header1.order=CELLYEAR;
    header1.firstcell=0;
    header1.firstyear=1901;
    header1.cellsize_lon=header1.cellsize_lat=0.5;
    header1.ncell=1;
    header1.nyear=1;
    version=LPJ_CLIMATE_VERSION;
    grid_type=LPJ_SHORT;
    in1=openmetafile(&header1,&map,map_name,&attrs,&n_attr,&source,&history,&variable,&units,&standard_name,&long_name,&grid_name,&grid_type,&format,&swap1,&offset,argv[iarg+1],TRUE);
    if(in1==NULL)
      return EXIT_FAILURE;
    if(format==CLM)
    {
      if(freadheaderid(in1,id,TRUE))
        return EXIT_FAILURE;
    }
    fseek(in1,offset,SEEK_SET);
  }
  else
  {
    in1=fopen(argv[iarg+1],"rb");
    if(in1==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+1],strerror(errno));
      return EXIT_FAILURE;
    }
    if(israw)
    {
      header1.datatype=(index!=NOT_FOUND) ? type : LPJ_SHORT;
      header1.nyear=1;
      header1.nbands=1;
      header1.nstep=1;
      header1.scalar=1;
      size=getfilesizep(in1);
      header1.ncell=size/typesizes[header1.datatype];
      header1.order=CELLYEAR;
    }
    else
    {
      version=setversion;
      if(freadanyheader(in1,&header1,&swap1,id,&version,TRUE))
      {
        fprintf(stderr,"Error reading header in '%s'.\n",argv[iarg+1]);
        return EXIT_FAILURE;
      }
      if(version>CLM_MAX_VERSION)
      {
        fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
                version,argv[iarg+1],CLM_MAX_VERSION+1);
        return EXIT_FAILURE;
      }
      if(index!=NOT_FOUND)
        header1.datatype=type;
      size=getfilesizep(in1)-headersize(id,version);
      if((header1.order==CELLINDEX && size!=sizeof(int)*header1.ncell+(long long)header1.nyear*header1.ncell*header1.nbands*header1.nstep*typesizes[header1.datatype]) ||
         (header1.order!=CELLINDEX && size!=(long long)header1.nyear*header1.ncell*header1.nbands*header1.nstep*typesizes[header1.datatype]))
        fprintf(stderr,"Warning: File size of '%s' does not match nbands*nstep*ncell*nyear.\n",argv[iarg+1]);
    }
  }
  if(op!=FLOAT && op!=INT)
  {
    value=(float)strtod(argv[iarg+2],&endptr);
    if(op!=REPL && *endptr=='\0')
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
      if(ismeta)
      {
        /* set default values */
        header2.datatype=(index!=NOT_FOUND) ? type : LPJ_SHORT;
        header2.timestep=1;
        header2.nbands=1;
        header2.nstep=1;
        header2.order=CELLYEAR;
        header2.firstcell=0;
        header2.firstyear=1901;
        header2.cellsize_lon=header2.cellsize_lat=0.5;
        header2.ncell=1;
        header2.nyear=1;
        grid_type2=LPJ_SHORT;
        in2=openmetafile(&header2,&map2,map_name,NULL,NULL,NULL,NULL,NULL,&units2,NULL,NULL,&grid_name2,&grid_type2,NULL,&swap2,&offset,argv[iarg+2],TRUE);
        if(in1==NULL)
          return EXIT_FAILURE;
        fseek(in2,offset,SEEK_SET);
        if(units!=NULL && units2!=NULL && strcmp(units,units2))
          fprintf(stderr,"Warning: Unit '%s' in '%s' differs from unit '%s' in '%s'.\n",
                  units,argv[iarg+1],units2,argv[iarg+2]);
        if(grid_name.name!=NULL && grid_name2.name!=NULL && strcmp(grid_name.name,grid_name2.name))
          fprintf(stderr,"Warning: Grid filename '%s' in '%s' differs from grid filename '%s' in '%s'.\n",
                  grid_name.name,argv[iarg+1],grid_name2.name,argv[iarg+2]);
        if(map!=NULL  && map2!=NULL && !cmpmap(map,map2))
          fprintf(stderr,"Warning: Map '%s' in '%s' differs from map in '%s'.\n",
                  map_name,argv[iarg+1],argv[iarg+2]);

      }
      else
      {
        in2=fopen(argv[iarg+2],"rb");
        if(in2==NULL)
        {
          fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
          return EXIT_FAILURE;
        }
        if(israw)
        {
          header2.datatype=(index!=NOT_FOUND) ? type : LPJ_SHORT;
          header2.nyear=1;
          header2.nbands=1;
          header2.nstep=1;
          header2.scalar=1;
          header2.order=CELLYEAR;
          size=getfilesizep(in2);
          header2.ncell=size/typesizes[header2.datatype];
        }
        else
        {
          version=setversion;
          if(freadheader(in2,&header2,&swap2,id,&version,TRUE))
          {
            fprintf(stderr,"Error reading header in '%s'.\n",argv[iarg+2]);
            return EXIT_FAILURE;
          }
          if(version>CLM_MAX_VERSION)
          {
            fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
                    version,argv[iarg+2],CLM_MAX_VERSION+1);
            return EXIT_FAILURE;
          }
          if(index!=NOT_FOUND)
            header2.datatype=type;
          size=getfilesizep(in2)-headersize(id,version);
          if((header1.order==CELLINDEX && size!=sizeof(int)*header2.ncell+(long long)header2.nyear*header2.ncell*header2.nbands*header2.nstep*typesizes[header2.datatype]) ||
             (header1.order!=CELLINDEX && size!=(long long)header2.nyear*header2.ncell*header2.nbands*header2.nstep*typesizes[header2.datatype]))
            fprintf(stderr,"Warning: File size of '%s' does not match nbands*nstep*ncell*nyear.\n",argv[iarg+2]);
        }
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
      if(header1.nstep!=header2.nstep)
      {
        fprintf(stderr,"nstep %d differs from %d.\n",header1.nstep,header2.nstep);
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
      if(header1.order!=header2.order)
      {
        fprintf(stderr,"cell order %d differs from %d.\n",header1.order,header2.order);
        return EXIT_FAILURE;
      }
    }
    if(isvalue)
      isint=(intvalue && header1.datatype==LPJ_INT && header1.scalar==1);
    else
      isint=(header1.datatype==LPJ_INT && header2.datatype==LPJ_INT && header1.scalar==1 && header2.scalar==1);
    if(isint)
    {
      idata1=newvec(int,header1.nbands*header1.nstep);
      check(idata1);
      idata3=newvec(int,header1.nbands*header1.nstep);
      check(idata3);
      if(!isvalue)
      {
        idata2=newvec(int,header1.nbands*header1.nstep);
        check(idata2);
      }
    }
    else
    {
      data1=newvec(float,header1.nbands*header1.nstep);
      check(data1);
      data3=newvec(float,header1.nbands*header1.nstep);
      check(data3);
      if(!isvalue)
      {
        data2=newvec(float,header1.nbands*header1.nstep);
        check(data2);
      }
    }
  }
  else if(op==INT)
  {
    idata1=newvec(int,header1.nbands*header1.nstep);
    check(idata1);
  }
  else
  {
    data1=newvec(float,header1.nbands*header1.nstep);
    check(data1);
  }
  out_name=argv[iarg+((op==FLOAT || op==INT) ? 2 : 3)];
  if(!isforce)
  {
    if(!stat(out_name,&filestat))
    {
      fprintf(stderr,"File '%s' already exists, overwrite (y/n)?\n",out_name);
      scanf("%c",&c);
      if(c!='y')
        return EXIT_FAILURE;
    }
  }
  out=fopen(out_name,"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",out_name,strerror(errno));
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
  if(format==CLM)
    fwriteheader(out,&header3,id,max(version,(ismeta) ? 4 : 3));
  if(header1.order==CELLINDEX)
  {
    cell_index=newvec(int,header1.ncell);
    if(cell_index==NULL)
    {
      printallocerr("index");
      return EXIT_FAILURE;
    }
    if(freadint(cell_index,header1.ncell,swap1,in1)!=header1.ncell)
    {
      fprintf(stderr,"Unexpected end of file in '%s' at reading cell index.\n",
              argv[iarg+1]);
      return EXIT_FAILURE;
    }
    fwrite(cell_index,sizeof(int),header1.ncell,out);
    if(op!=FLOAT && op!=INT && !isvalue)
    {
      cell_index2=newvec(int,header1.ncell);
      if(cell_index2==NULL)
      {
        printallocerr("index2");
        return EXIT_FAILURE;
      }
      if(freadint(cell_index2,header1.ncell,swap2,in2)!=header1.ncell)
      {
        fprintf(stderr,"Unexpected end of file in '%s' at reading cell index.\n",
                argv[iarg+2]);
        return EXIT_FAILURE;
      }
      for(cell=0;cell<header1.ncell;cell++)
        if(cell_index[cell]!=cell_index2[cell])
        {
          fprintf(stderr,"Cell index in '%s' of %d=%d not equal %d in '%s'.\n",
                  argv[iarg+1],cell,cell_index[cell],cell_index2[cell],argv[iarg+2]);
          return EXIT_FAILURE;
        }
      free(cell_index);
      free(cell_index2);
    }
  }
  if(op==FLOAT)
  {
    for(yr=0;yr<header1.nyear;yr++)
      for(cell=0;cell<header1.ncell;cell++)
      {
        if(readfloatvec(in1,data1,header1.scalar,header1.nbands*header1.nstep,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        if(fwrite(data1,sizeof(float),header1.nbands*header1.nstep,out)!=header1.nbands*header1.nstep)
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
        if(readintvec(in1,idata1,header1.nbands*header1.nstep,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        if(fwrite(idata1,sizeof(int),header1.nbands*header1.nstep,out)!=header1.nbands)
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
        if(readintvec(in1,idata1,header1.nbands*header1.nstep,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        if(isvalue)
          switch(op)
          {
            case ADD:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=idata1[k]+ivalue;
              break;
            case SUB:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=idata1[k]-ivalue;
              break;
            case MUL:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=idata1[k]*ivalue;
              break;
            case DIV:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=idata1[k]/ivalue;
              break;
            case AVG:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=(idata1[k]+ivalue)/2;
              break;
            case MAX:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=max(idata1[k],ivalue);
              break;
            case MIN:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=min(idata1[k],ivalue);
              break;
            default:
              break;
          } /* of switch */
        else
        {
          if(readintvec(in2,idata2,header1.nbands*header1.nstep,swap2,header2.datatype))
          {
            fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                    argv[iarg+2],yr+header1.firstyear);
            return EXIT_FAILURE;
          }
          switch(op)
          {
            case ADD:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=idata1[k]+idata2[k];
              break;
            case SUB:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=idata1[k]-idata2[k];
              break;
            case MUL:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=idata1[k]*idata2[k];
              break;
            case DIV:
              for(k=0;k<header1.nbands*header1.nstep;k++)
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
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=(idata1[k]+idata2[k])/2;
              break;
            case MAX:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=max(idata1[k],idata2[k]);
              break;
            case MIN:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                idata3[k]=min(idata1[k],idata2[k]);
              break;
            case REPL:
              flag=FALSE;
              for(k=0;k<header1.nbands*header1.nstep;k++)
                if(idata2[k]!=0)
                {
                  flag=TRUE;
                  break;
                }
              if(flag)
                for(k=0;k<header1.nbands*header1.nstep;k++)
                  idata3[k]=idata2[k];
              else
                for(k=0;k<header1.nbands*header1.nstep;k++)
                  idata3[k]=idata1[k];
              break;
            case FLOAT: case INT:
              break;
          } /* of switch */
        }
        /* write int data to file */
        if(fwrite(idata3,sizeof(int),header1.nbands*header1.nstep,out)!=header1.nbands*header1.nstep)
        {
          fprintf(stderr,"Error writing '%s' in year %d.\n",
                  out_name,yr+header1.firstyear);
          return EXIT_FAILURE;
        }
      }
  else
    for(yr=0;yr<header1.nyear;yr++)
      for(cell=0;cell<header1.ncell;cell++)
      {
        if(readfloatvec(in1,data1,header1.scalar,header1.nbands*header1.nstep,swap1,header1.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                  argv[iarg+1],yr+header1.firstyear);
          return EXIT_FAILURE;
        }
        if(isvalue)
          switch(op)
          {
            case ADD:
              for(k=0;k<header1.nbands*header1.nstep;k++)
              data3[k]=data1[k]+value;
              break;
            case SUB:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=data1[k]-value;
              break;
            case MUL:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=data1[k]*value;
              break;
            case DIV:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=data1[k]/value;
              break;
            case AVG:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=(data1[k]+value)*0.5f;
              break;
            case MAX:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=max(data1[k],value);
              break;
            case MIN:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=min(data1[k],value);
              break;
            default:
              break;
          } /* of switch */
        else
        {
          if(readfloatvec(in2,data2,header2.scalar,header1.nbands*header1.nstep,swap2,header2.datatype))
          {
            fprintf(stderr,"Unexpected end of file in '%s' in year %d.\n",
                    argv[iarg+2],yr+header1.firstyear);
            return EXIT_FAILURE;
          }
          switch(op)
          {
            case ADD:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=data1[k]+data2[k];
              break;
            case SUB:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=data1[k]-data2[k];
              break;
            case MUL:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=data1[k]*data2[k];
              break;
            case DIV:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=data1[k]/data2[k];
              break;
            case AVG:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=(data1[k]+data2[k])*0.5f;
              break;
            case MAX:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=max(data1[k],data2[k]);
              break;
            case MIN:
              for(k=0;k<header1.nbands*header1.nstep;k++)
                data3[k]=min(data1[k],data2[k]);
              break;
            case REPL:
              flag=FALSE;
              for(k=0;k<header1.nbands*header1.nstep;k++)
                if(data2[k]!=0)
                {
                  flag=TRUE;
                  break;
                }
              if(flag)
                for(k=0;k<header1.nbands*header1.nstep;k++)
                  data3[k]=data2[k];
              else
                for(k=0;k<header1.nbands*header1.nstep;k++)
                  data3[k]=data1[k];
              break;
            case FLOAT: case INT:
              break;
          } /* of switch */
        }
        /* write float data to file */
        if(fwrite(data3,sizeof(float),header1.nbands*header1.nstep,out)!=header1.nbands*header1.nstep)
        {
          fprintf(stderr,"Error writing '%s' in year %d.\n",
                  out_name,yr+header1.firstyear);
          return EXIT_FAILURE;
        }
      }
  fclose(in1);
  if(op!=FLOAT && !isvalue)
    fclose(in2);
  fclose(out);
  if(ismeta || isjson)
  {
    out_json=malloc(strlen(out_name)+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,out_name),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    out=fopen(out_json,"w");
    if(out==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    fprintjson(out,out_name,source,history,arglist,&header3,map,map_name,attrs,n_attr,variable,units,standard_name,long_name,(grid_name.name==NULL) ? NULL : &grid_name,grid_type,format,id,FALSE,max(version,(ismeta) ? 4 : 3));
    fclose(out);
  }
  return EXIT_FAILURE;
} /* of 'main' */
