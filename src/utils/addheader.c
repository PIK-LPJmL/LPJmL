/**************************************************************************************/
/**                                                                                \n**/
/**                       a  d  d  h  e  a  d  e  r  .  c                          \n**/
/**                                                                                \n**/
/**     Program adds CLM header to binary file                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-swap] [-nyear n] [-firstyear n] [-lastyear n] [-ncell n]\n       [-firstcell n] [-nbands n] [-nstep n] [-timestep n] [-order n] [-version n] [-cellsize s]\n       [-scale s] [-id s] [-type {byte|short|int|float|double}] binfile clmfile\n"
#define ERR_USAGE USAGE "\nTry \"%s --help\" for more information.\n"

#define BUFSIZE (1024*1024) /* size of read buffer */

int main(int argc,char **argv)
{
  FILE *infile,*outfile;
  Header header;
  int version;
  char *id;
  const char *progname;
  char *endptr;
  int i,index,rc;
  size_t len;
  long long filesize;
  void *buffer;
  int *vector;
  Bool swap;
  progname=strippath(argv[0]);
  /* set default values for header */
  version=LPJ_CLIMATE_VERSION;
  id=LPJ_CLIMATE_HEADER;
  header.firstyear=1901;
  header.ncell=67420;
  header.firstcell=0;
  header.nyear=109;
  header.nbands=12;
  header.nstep=1;
  header.timestep=1;
  header.order=CELLYEAR;
  header.scalar=1;
  header.datatype=LPJ_SHORT;
  len=typesizes[header.datatype];
  header.cellsize_lon=header.cellsize_lat=0.5;
  swap=FALSE;
  for(index=1;index<argc;index++)
    if(argv[index][0]=='-')
    {
      if(!strcmp(argv[index],"-h") || !strcmp(argv[index],"--help"))
      {
        printf("     ");
        rc=printf("%s (" __DATE__ ") Help",progname);
        printf("\n     ");
        repeatch('=',rc);
        printf("\n\nAdd clm header to binary file for LPJmL version %s\n\n",getversion());
        printf(USAGE
               "\nArguments:\n"
               "-swap        byte order is changed in raw binary file\n"
               "-nyear n     set number of years, default is 109\n"
               "-firstyear n set first year, default is 1901\n"
               "-lastyear    set last year, default is 2009\n"
               "-ncell n     set number of cells, default is 67420, if 0 calculated from file size\n"
               "-firstcell n set index of first cell, default is 0\n"
               "-nbands n    set number of bands, default is 12\n"
               "-nstep n     set number of steps within years, default is 1\n"
               "-timestep n  set number of years between time steps, default is 1\n"
               "-order n     set order of data, default is 0\n"
               "-version n   set version of clm file, default is %d\n"
               "-cellsize s  set cell size, default is 0.5\n"
               "-scale s     set scaling factor, default is 1\n"
               "-id s        set id string, default is %s\n"
               "-type t      set datatype of clm file, default is short\n"
               "binfile      filename of raw binary file\n"
               "clmfile      filename of clm data file\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               progname,version,id);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[index],"-firstyear"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-firstyear'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-firstyear'.\n",argv[index]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-lastyear"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-lastyear'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++index],&endptr,10)-header.firstyear+1;
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-lastyear'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nyear<=0)
        {
          fprintf(stderr,"Invalid value %d for last year less than first year %d.\n",header.firstyear-1+header.nyear,header.firstyear);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-nyear"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-nyear'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-nyear'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nyear<=0)
        {
          fputs("Number of years less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-ncell"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-ncell'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.ncell=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-ncell'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.ncell<0)
        {
          fputs("Number of cells less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-firstcell"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-firstcell'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.firstcell=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-firstcell'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.firstcell<0)
        {
          fputs("First cell is less than zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-nbands"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-nbands'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.nbands=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-nbands'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nbands<=0)
        {
          fputs("Number of bands less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-nstep"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-nstep'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.nstep=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-nstep'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nstep<=0)
        {
          fputs("Number of steps less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-timestep"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-timestep'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.timestep=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-timestep'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.timestep<=0)
        {
          fputs("Number of time steps less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-cellsize"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-cellsize'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.cellsize_lon=header.cellsize_lat=(float)strtod(argv[++index],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-cellsize'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.cellsize_lon<=0)
        {
          fputs("Cell size less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-scale"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-scale'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.scalar=(float)strtod(argv[++index],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-scale'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.scalar<=0)
        {
          fputs("Scale factor less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-order"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-order'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        header.order=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          header.order=findstr(argv[index],ordernames,4);
          if(header.order==NOT_FOUND)
          {
            fprintf(stderr,"Invalid number '%s' for order.\n",argv[index]);
            return EXIT_FAILURE;
          }
          header.order++;
        }
      }
      else if(!strcmp(argv[index],"-version"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-version'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        version=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-version'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(version<1)
        {
          fprintf(stderr,"Version=%d must be greater than zero.\n",version);
          return EXIT_FAILURE;
        }
        if(version>CLM_MAX_VERSION)
        {
          fprintf(stderr,"Version %d greater than maximum version %d supported.\n",
                  version,CLM_MAX_VERSION);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-type"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-type'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        i=findstr(argv[++index],typenames,N_TYPES);
        if(i==NOT_FOUND)
        {
          fprintf(stderr,"Invalid argument '%s' for option '-type'.\n"
                  ERR_USAGE,argv[index],progname,progname);
          return EXIT_FAILURE;
        }
        header.datatype=(Type)i;
        len=typesizes[header.datatype];
      }
      else if(!strcmp(argv[index],"-id"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-id'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        id=argv[++index];
        if(strncmp(id,"LPJ",3))
          fputs("Warning: Header string does not start with 'LPJ'.\n",stderr);
      }
      else if(!strcmp(argv[index],"-swap"))
        swap=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                ERR_USAGE,argv[index],progname,progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<index+2)
  {
    fprintf(stderr,"Argument(s) missing.\n"
            ERR_USAGE,progname,progname);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[index],argv[index+1]))
  {
    fputs("Error: source and destination filename are the same.\n",stderr);
    return EXIT_FAILURE;
  }
  infile=fopen(argv[index],"rb");
  if(infile==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[index],strerror(errno));
    return EXIT_FAILURE;
  }
  filesize=getfilesizep(infile);
  if(header.ncell==0) // if ncell=0 then calculate ncell from file size
    header.ncell=filesize/header.nyear/header.nbands/header.nstep/len;
  if((header.order==CELLINDEX && filesize!=sizeof(int)*header.ncell+(long long)header.ncell*header.nbands*header.nstep*header.nyear*len) ||
    (header.order!=CELLINDEX && filesize!=(long long)header.ncell*header.nbands*header.nstep*header.nyear*len))
  {
     fprintf(stderr,"Error: File size of '%s' does not match nyear*nbands*nstep*ncell*%d.\n",argv[index],(int)len);
     fclose(infile);
     return EXIT_FAILURE;
  }
  outfile=fopen(argv[index+1],"wb");
  if(outfile==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[index+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(outfile,&header,id,version);
  if(header.order==CELLINDEX)
  {
    filesize-=sizeof(int)*header.ncell;
    vector=newvec(int,header.ncell);
    if(vector==NULL)
    {
      printallocerr("index");
      return EXIT_FAILURE;
    }
    freadint(vector,header.ncell,swap,infile);
    if(fwrite(vector,sizeof(int),header.ncell,outfile)!=header.ncell)
      fprintf(stderr,"Error writing index in '%s'.\n",argv[index+1]);
    free(vector);
  }
  buffer=malloc(BUFSIZE);
  if(buffer==NULL)
  {
    printallocerr("buffer");
    return EXIT_FAILURE;
  }
  for(i=0;i<filesize / BUFSIZE;i++)
  {
    switch(len)
    {
      case 2:
        freadshort(buffer,BUFSIZE/2,swap,infile);
        break;
      case 4:
        freadint(buffer,BUFSIZE/4,swap,infile);
        break;
      case 8:
        freadlong(buffer,BUFSIZE/8,swap,infile);
        break;
      default:
        fread(buffer,1,BUFSIZE,infile);
    }
    if(fwrite(buffer,1,BUFSIZE,outfile)!=BUFSIZE)
    {
      fprintf(stderr,"Error writing data in '%s'.\n",argv[index+1]);
      return EXIT_FAILURE;
    }
  }
  if(filesize % BUFSIZE>0)
  {
    switch(len)
    {
      case 2:
        freadshort(buffer,(filesize % BUFSIZE)/2,swap,infile);
        break;
      case 4:
        freadint(buffer,(filesize % BUFSIZE)/4,swap,infile);
        break;
      case 8:
        freadlong(buffer,(filesize % BUFSIZE)/8,swap,infile);
        break;
      default:
        fread(buffer,1,filesize % BUFSIZE,infile);
    }
    if(fwrite(buffer,1,filesize % BUFSIZE,outfile)!=filesize % BUFSIZE)
    {
      fprintf(stderr,"Error writing data in '%s'.\n",argv[index+1]);
      return EXIT_FAILURE;
    }
  }
  free(buffer);
  fclose(infile);
  fclose(outfile);
  return EXIT_SUCCESS;
} /* of 'main' */
