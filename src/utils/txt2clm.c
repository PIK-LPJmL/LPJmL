/**************************************************************************************/
/**                                                                                \n**/
/**                      t  x  t  2  c  l  m  .  c                                 \n**/
/**                                                                                \n**/
/**     Converts text files into LPJ climate data files                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#undef USE_MPI /* no MPI */

#include "lpj.h"

#define USAGE "\nUsage: txt2clm [-h] [-version v] [-cellindex] [-scale s] [-float] [-int] [-nbands n] [-nstep n] [-timestep t] [-cellsize size]\n               [-firstcell n] [-ncell n] [-firstyear f] [-header id] [-csv c] [-json] txtfile clmfile\n"
#define ERR_USAGE USAGE "\nTry \"txt2clm --help\" for more information.\n"

static int getfloat(FILE *file,char sep,float *value)
{
  String s;
  int c,len;
  char *endptr;
  len=0;
  while((c=fgetc(file))!=EOF)
  {
    if(len==STRING_LEN)
      return 0;
    if(c=='\n' || c==sep)
    {
      s[len]='\0';
      *value=(float)strtod(s,&endptr);
      if(*endptr=='\0')
        return 1;
      fprintf(stderr,"Invalid number '%s'.\n",s);
      return 0;
    }
    s[len++]=(char)c;
  }
  return 0;
} /* of 'getfloat' */

static int getint(FILE *file,char sep,int *value)
{
  String s;
  int c,len;
  char *endptr;
  len=0;
  while((c=fgetc(file))!=EOF)
  {
    if(len==STRING_LEN)
      return 0;
    if(c=='\n' || c==sep)
    {
      s[len]='\0';
      *value=strtol(s,&endptr,10);
      if(*endptr=='\0')
        return 1;
      fprintf(stderr,"Invalid number '%s'.\n",s);
      return 0;
    }
    s[len++]=(char)c;
  }
  return 0;
} /* of 'getint' */

int main(int argc,char **argv)
{
  FILE *file,*out;
  float multiplier;
  char *endptr;
  float value;
  short s;
  int data;
  Header header;
  int version;
  char *id;
  char *arglist,*out_json;
  int i,iarg,rc;
  Bool isjson;
  char sep;
  /* set default values */
  header.order=CELLYEAR;
  header.firstyear=1901;
  header.nbands=12;
  header.nstep=1;
  header.timestep=1;
  header.firstcell=0;
  header.ncell=67420;
  multiplier=1;
  header.datatype=LPJ_SHORT;
  header.cellsize_lon=header.cellsize_lat=0.5;
  id=LPJ_CLIMATE_HEADER;
  version=LPJ_CLIMATE_VERSION;
  isjson=FALSE;
  sep='\0';
  /* parse command line options */
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {
        printf("   txt2clm (" __DATE__ ") Help\n"
               "   ==========================\n\n"
               "Convert text files to clm data files for LPJmL version %s\n",getversion());
        printf(USAGE
               "\nArguments:\n"
               "-h,--help    print this help text\n"
               "-version     set version, default is %d\n"
               "-cellindex   set order to cell index\n"
               "-csv c       file is in CSV format with separator c\n"
               "-float       write data as float, default is short\n"
               "-int         write data as int, default is short\n"
               "-nbands n    number of bands, default is %d\n"
               "-nstep n     number of steps, default is %d\n"
               "-timestep n  number of years between time step, defauls is %d\n"
               "-firstcell n index of first cell\n"
               "-ncell n     number of cells, default is %d\n"
               "-firstyear f first year, default is %d\n"
               "-scale s     scale data by a factor of s\n"
               "-cellsize s  cell size, default is %g\n"
               "-header id   clm header string, default is '%s'\n"
               "-json        JSON metafile is created with suffix '.json'\n"
               "txtfile      filename of text file\n"
               "clmfile      filename of clm data file\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               version,header.nbands,header.nstep,header.timestep,header.ncell,header.firstyear,header.cellsize_lon,id);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-float"))
        header.datatype=LPJ_FLOAT;
      else if(!strcmp(argv[iarg],"-int"))
        header.datatype=LPJ_INT;
      else if(!strcmp(argv[iarg],"-cellindex"))
        header.order=CELLINDEX;
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-scale"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-scale' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        multiplier=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-scale'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
        if(multiplier<=0)
        {
          fprintf(stderr,"Scale=%g must be greater than zero.\n",multiplier);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-cellsize"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-cellsize' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.cellsize_lon=header.cellsize_lat=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-cellsize'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.cellsize_lon<=0)
        {
          fprintf(stderr,"Cell size=%g must be greater than zero.\n",header.cellsize_lon);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-nbands"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-nbands' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.nbands=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-nbands'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.nbands<1)
        {
          fprintf(stderr,"Number of bands=%d must be greater than zero.\n",header.nbands);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-csv"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-csv' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        sep=argv[++iarg][0];
      }
      else if(!strcmp(argv[iarg],"-nstep"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-nstep' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.nstep=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-nstep'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.nstep!=1 && header.nstep!=NMONTH && header.nstep!=NDAYYEAR)
        {
           fprintf(stderr,"Error: Number of steps=%d must be 1, 12, or 365.\n",header.nstep);
           return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-timestep"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-timestep' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.timestep=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-timestep'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.timestep<1)
        {
          fprintf(stderr,"Number of time steps=%d must be greater than zero.\n",header.timestep);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-version"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-version' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        version=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-version'.\n",
                  argv[iarg]);
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
      else if(!strcmp(argv[iarg],"-firstcell"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-firstcell' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.firstcell=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-firstcell'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-ncell"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-ncell' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.ncell=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-ncell'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.ncell<1)
        {
          fprintf(stderr,"Number of cells=%d must be greater than zero.\n",header.ncell);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-firstyear"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-firstyear' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-firstyear'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-header"))
      {
        if(iarg==argc-1)
        {
          fputs("Argument missing after '-header' option.\n"
                ERR_USAGE,stderr);
          return EXIT_FAILURE;
        }
        id=argv[++iarg];
      }
      else
      {
        fprintf(stderr,"Error: Invalid option '%s'.\n",argv[iarg]);
        fprintf(stderr,ERR_USAGE);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+2)
  {
    fputs("Filename(s) missing.\n"
          ERR_USAGE,stderr);
    return EXIT_FAILURE;
  }
  if(header.timestep>1 && header.nstep>1)
  {
    fprintf(stderr,"Error: Number of time steps=%d within a year must be 1 if years between time steps=%d is greater than 1.\n",
            header.nstep,header.timestep);
    return EXIT_FAILURE;
  }
  if(version<4)
  {
    if(header.nstep>1 && header.nbands==1)
    {
      fprintf(stderr,"Warning: Version of clm file is %d and nstep>1, nbands set to nstep=%d\n",
              version,header.nstep);
      header.nbands=header.nstep;
    }
    else if(header.nstep>1 && header.nbands>1)
    {
      fprintf(stderr,"Error: Number of steps=%d and bands=%d must not be both >1 for verion %d clm files.\n",
              header.nstep,header.nbands,version);
      return EXIT_FAILURE;
    }
    if(header.timestep>1)
      fprintf(stderr,"Warning: Version of clm file is %d and timestep>1, is not written to clm file\n",version);
  }
  file=fopen(argv[iarg],"r");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[iarg],strerror(errno));
    return EXIT_FAILURE;
  }
  header.nyear=0;
  header.scalar=1/multiplier;
  out=fopen(argv[iarg+1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[iarg+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(out,&header,id,version);
  if(header.order==CELLINDEX)
  {
    for(i=0;i<header.ncell;i++)
    {
      if(sep)
        rc=getint(file,sep,&data);
      else
        rc=fscanf(file,"%d",&data);
      if(rc!=1)
      {
        fprintf(stderr,"Unexpected end of file in '%s' reading cell index.\n",argv[iarg]);
        return EXIT_FAILURE;
      }
      if(fwrite(&data,sizeof(int),1,out)!=1)
      {
        fprintf(stderr,"Error writing data in '%s': %s.\n",argv[iarg+1],strerror(errno));
        return EXIT_FAILURE;
      }
    }
  }
  do
  {
    if(header.datatype==LPJ_INT)
    {
      for(i=0;i<header.ncell*header.nbands*header.nstep;i++)
      {
        if(sep)
          rc=getint(file,sep,&data);
        else
          rc=fscanf(file,"%d",&data);
        if(rc!=1)
          break;
        if(fwrite(&data,sizeof(data),1,out)!=1)
        {
          fprintf(stderr,"Error writing data in '%s': %s.\n",argv[iarg+1],strerror(errno));
          return EXIT_FAILURE;
        }
      }
    }
    else
      for(i=0;i<header.ncell*header.nbands*header.nstep;i++)
      {
        if(sep)
          rc=getfloat(file,sep,&value);
        else
          rc=fscanf(file,"%g",&value);
        if(rc!=1)
          break;
        if(header.datatype==LPJ_SHORT)
        {
          if(value*multiplier<SHRT_MIN || value*multiplier>SHRT_MAX)
             fprintf(stderr,"WARNING: Data overflow %g in year %d at cell %d and band %d\n",
                     value*multiplier,header.nyear,i/header.nbands/header.nstep+1,i % (header.nbands*header.nstep)+1);
          s=(short)(value*multiplier);
          if(fwrite(&s,sizeof(short),1,out)!=1)
          {
            fprintf(stderr,"Error writing data in '%s': %s.\n",argv[iarg+1],strerror(errno));
            return EXIT_FAILURE;
          }
        }
        else
        {
          if(fwrite(&value,sizeof(float),1,out)!=1)
          {
            fprintf(stderr,"Error writing data in '%s': %s.\n",argv[iarg+1],strerror(errno));
            return EXIT_FAILURE;
          }
        }
    }
    if(i==header.ncell*header.nbands*header.nstep)
      header.nyear++;
    else if(i!=0)
      fprintf(stderr,"Cannot read data in year %d at cell %d and band %d.\n",
              header.nyear+1,i / header.nbands/header.nstep+1,i % (header.nbands*header.nstep)+1);
    else if(!feof(file))
      fprintf(stderr,"End of file not reached in '%s'.\n",argv[iarg]);
  } while(rc==1);
  fclose(file);
  rewind(out);
  fwriteheader(out,&header,id,version);
  fclose(out);
  if(isjson)
  {
    out_json=malloc(strlen(argv[iarg+1])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[iarg+1]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    out=fopen(out_json,"w");
    if(out==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    fprintjson(out,argv[iarg+1],NULL,"txt2clm",NULL,arglist,&header,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL,NULL,LPJ_SHORT,CLM,id,FALSE,version);
    fclose(out);
  }
  return EXIT_SUCCESS;
} /* of 'main' */
