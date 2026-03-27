/**************************************************************************************/
/**                                                                                \n**/
/**                 s  p  l  i  t  c  l  m  .  c                                   \n**/
/**                                                                                \n**/
/**     Program cuts specific bands out of CLM file                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: splitclm [-h] [-longheader] [-metafile] [-json] first last infile outfile\n"
#define ERR_USAGE USAGE "\nTry \"splitclm --help\" for more information.\n"

int main(int argc,char **argv)
{
  FILE *file,*out;
  char *endptr,*out_json,*arglist;
  Header header;
  Metadata metadata;
  Filename grid_name;
  Type grid_type;
  Bool swap,ismeta=FALSE,isjson=FALSE;
  size_t offset;
  int version,first,last,iarg,*idata,i,nbands,format;
  long long size,*ldata,filesize;
  short *sdata;
  Byte *bdata;
  String id;
  version=READ_VERSION;
  grid_name.name=NULL;
  for(iarg=1;iarg<argc;iarg++)
  {
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {
        printf("   splitclm (" __DATE__ ") Help\n"
               "   ============================\n\n"
               "Copy specific bands of clm data files for LPJmL version %s\n\n"
               USAGE
               "\nArguments:\n"
               "-h,--help   print this help text\n"
               "-longheader force version of clm file to 2\n"
               "-metafile   set the input format to JSON metafile instead of CLM\n"
               "-json       JSON metafile is created with suffix '.json'\n"
               "first       index of first band to be copied into new file\n"
               "last        index of last band to be copied into new file\n"
               "infile      filename of clm file to be splitted\n"
               "outfile     filename of new clm file\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               getversion());
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-longheader"))
        version=2;
       else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                ERR_USAGE,argv[iarg]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  }
  if(argc<iarg+4)
  {
    fprintf(stderr,"Argument(s) missing.\n"
            ERR_USAGE);
    return EXIT_FAILURE;
  }
  first=strtol(argv[iarg],&endptr,10);
  if(*endptr!='\0')
  {
    fprintf(stderr,"Invalid number '%s' for first.\n",argv[iarg]);
    return EXIT_FAILURE;
  }
  if(first<=0)
  {
    fprintf(stderr,"Invalid number %d for first, must be >0.\n",first);
    return EXIT_FAILURE;
  }
  last=strtol(argv[iarg+1],&endptr,10);
  if(*endptr!='\0')
  {
    fprintf(stderr,"Invalid number '%s' for last.\n",argv[iarg]);
    return EXIT_FAILURE;
  }
  if(last<first)
  {
    fprintf(stderr,"Invalid number %d for last band, must be >=%d.\n",last,first);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[iarg+2],argv[iarg+3]))
  {
    fputs("Error: source and destination filename are the same.\n",stderr);
    return EXIT_FAILURE;
  }
  format=CLM;
  initmetadata(&metadata,NULL);
  if(ismeta)
  {
    /* set default values */
    header.datatype=LPJ_SHORT;
    header.timestep=1;
    header.nbands=1;
    header.nstep=1;
    header.order=CELLYEAR;
    header.firstcell=0;
    header.firstyear=1901;
    header.cellsize_lon=header.cellsize_lat=0.5;
    header.ncell=1;
    header.nyear=1;
    version=CLM_MAX_VERSION;
    grid_type=LPJ_SHORT;
    file=openmetafile(&header,&metadata,&grid_name,&grid_type,&format,&swap,&offset,argv[iarg+2],TRUE);
    if(file==NULL)
      return EXIT_FAILURE;
    if(format==CLM)
    {
      if(freadheaderid(file,id,TRUE))
        return EXIT_FAILURE;
    }
    fseek(file,offset,SEEK_SET);
    size=typesizes[header.datatype];
  }
  else
  {
    file=fopen(argv[iarg+2],"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[iarg+2],strerror(errno));
      return EXIT_FAILURE;
    }
    if(freadanyheader(file,&header,&swap,id,&version,TRUE))
    {
      fprintf(stderr,"Error reading header in '%s'.\n",
              argv[iarg+2]);
      return EXIT_FAILURE;
    }
    if(version>CLM_MAX_VERSION)
    {
      fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
              version,argv[iarg+2],CLM_MAX_VERSION+1);
      return EXIT_FAILURE;
    }
    if(version>=3)
      size=typesizes[header.datatype];
    else
    {
      filesize=getfilesizep(file);
      size=(filesize-headersize(id,version))/header.ncell/header.nbands/header.nyear/header.nstep;
      printf("Size of data: %Ld bytes\n",size);
      if(size!=1 && size!=2 && size!=4 && size!=8)
      {
        fprintf(stderr,"Invalid size of data=%Ld.\n",size);
        return EXIT_FAILURE;
      }
      if((filesize-headersize(id,version)) % ((long long)header.ncell*header.nbands*header.nyear*header.nstep)!=0)
        fprintf(stderr,"Warning: file size of '%s' is not multiple of ncell*nbands*nstep*nyear.\n",argv[iarg+2]);
    }
  }
  if(first>header.nbands)
  {
    fprintf(stderr,"Invalid first band %d, must be in [1,%d].\n",
            first,header.nbands);
    return EXIT_FAILURE;
  }
  if(last>header.nbands)
  {
    fprintf(stderr,"Invalid last band %d, must be in [%d,%d].\n",
            last,first,header.nbands);
    return EXIT_FAILURE;
  }
  out=fopen(argv[iarg+3],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[iarg+3],strerror(errno));
    return EXIT_FAILURE;
  }
  nbands=header.nbands;
  header.nbands=last-first+1;
  first--;
  fwriteheader(out,&header,id,version);
  switch(size)
  {
    case 1:
      bdata=malloc(nbands);
      check(bdata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(fread(&bdata,1,nbands,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[iarg+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(bdata+first,1,header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[iarg+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
    case 2:
      sdata=newvec(short,nbands);
      check(sdata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(freadshort(sdata,nbands,swap,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[iarg+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(sdata+first,sizeof(short),header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[iarg+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
    case 4:
      idata=newvec(int,nbands);
      check(idata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(freadint(idata,nbands,swap,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[iarg+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(idata+first,sizeof(int),header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[iarg+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
    case 8:
      ldata=newvec(long long,nbands);
      check(ldata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(freadlong(ldata,nbands,swap,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[iarg+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(ldata+first,sizeof(long long),header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[iarg+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
  } /* of switch */
  fclose(file);
  fclose(out);
  if(ismeta || isjson)
  {
    out_json=malloc(strlen(argv[iarg+3])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[iarg+3]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    out=fopen(out_json,"w");
    if(out==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    fprintjson(out,argv[iarg+3],NULL,arglist,&header,&metadata,(grid_name.name==NULL) ? NULL : &grid_name,grid_type,format,id,FALSE,version);
    free(out_json);
    free(arglist);
    fclose(out);
  }
  freemetadata(&metadata);
  free(grid_name.name);
  return EXIT_SUCCESS;
} /* of 'main' */
