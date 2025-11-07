/**************************************************************************************/
/**                                                                                \n**/
/**                      b  s  q  2  c  l  m  .  c                                 \n**/
/**                                                                                \n**/
/**     Program converts ARC/Info ASCII (plain text) input data into the           \n**/
/**     CLM format for LPJ                                                         \n**/
/**     Input must be in the following form:                                       \n**/
/**     ncols   288                                                                \n**/
/**     nrows   288                                                                \n**/
/**     xllcorner       98.021                                                     \n**/
/**     yllcorner       12.021                                                     \n**/
/**     cellsize        0.042                                                      \n**/
/**     NODATA_value    -9999.000                                                  \n**/
/**     3636.0 3737.0 ...                                                          \n**/
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

#define ASC2CLM "1.0.003"
#define USAGE "Usage: bsq2clm [-h] [-f] [-firstyear y] [-grid file] [-nbands n] [-header s]\n       [-{int|float}] [-scale s] infile ... clmfile\n"
#define FIRSTYEAR 1901

int main(int argc,char **argv)
{
  FILE *file,*out,*headerfile;
  Coordfile coordfile;
  Bool swap;
  String id;
  float scale;
  int row,col,nrows,ncols,nyear,nbands,nodata_int,version,ngrid;
  int nrows_first,ncols_first,ncell_first;
  float xllcorner_first,yllcorner_first,cellsize_first;
  float xllcorner,yllcorner,cellsize,nodata,value;
  short **data;
  int **data_int,value_int;
  float **data_float;
  char *endptr;
  Coord grid;
  Type type;
  Header header, gridheader;
  char *head;
  int i,m,y;
  Bool force;
  struct stat buf;
  char c;
  header.firstyear=FIRSTYEAR;
  header.firstcell=0;
  header.datatype=LPJ_SHORT;
  nyear=1;
  nbands=1;
  header.order=CELLYEAR;
  scale=1;
  head=LPJ_CLIMATE_HEADER;
  gridfile=NULL;
  force=FALSE;
  type=LPJ_SHORT;
  coordfile=new(struct coordfile);
  /* parse command line options */
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("   bsq2clm version " BSQ2CLM " (" __DATE__ ") help\n"
               "   ==========================================\n\n"
               "Convert gridded BSQ files to clm data files for LPJ C version\n\n"
               USAGE
               "\nArguments:\n"
               "-h           print this help text\n"
               "-f           force overwrite of output file\n"
               "-firstyear y first year, default is %d\n"
               "-grid file   create grid file\n" 
               "-nbands n    number of bands, default is 1\n"
               "-nyear  n    number of years, default is 1\n"
               "-header s    header string, default is " LPJ_CLIMATE_HEADER "\n"
               "-scale s     scale data by a factor of s\n"
               "-int         write integer data into clm file\n"
               "-float       write float data into clm file\n"
               "infile       filename(s) of gridded data file\n"
               "clmfile      filename of clm data file\n",FIRSTYEAR);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[i],"-firstyear"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-firstyear' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-firstyear'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-f"))
        force=TRUE;
      else if(!strcmp(argv[i],"-int"))
        type=LPJ_INT;
      else if(!strcmp(argv[i],"-float"))
        type=LPJ_FLOAT;
      else if(!strcmp(argv[i],"-grid"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-grid' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        gridfile=fopen(argv[++i],"rb");
        if(gridfile==NULL)
        {
          fprintf(stderr,"Error open '%s': %s.\n",argv[i],strerror(errno));
          return EXIT_FAILURE;
        }
        freadheader(gridfile,&gridheader,&swap,id,&version);
        coordfile->offset=headersize(LPJGRID_HEADER,version);
        ngrid=(getfilesize(argv[iarg+1])-coordfile->offset)/sizeof(short)/2;
      }
      else if(!strcmp(argv[i],"-nbands"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-nbands' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        nbands=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-nbands'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(nbands<1)
        {
          fputs("Error: number of bands must be >0.\n",stderr);
          return EXIT_FAILURE;
        }
      }
     else if(!strcmp(argv[i],"-nyear"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-nyear' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        nyear=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-nyear'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(nyear<1)
        {
          fputs("Error: number of nyear must be >0.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-header"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-header' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        head=argv[++i];
      }
      else if(!strcmp(argv[i],"-scale"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-scale' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        scale=(float)strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-scale'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(scale==0.0)
        {
          fputs("Error: scale factor must not be zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n"
                USAGE,argv[i]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<i+2)
  {
    fputs("Filename(s) missing.\n"
          USAGE,stderr);
    return EXIT_FAILURE;
  }
  if(!force)
  {
    if(!stat(argv[i+1],&buf))
    {
      fprintf(stderr,"File '%s' already exists, overwrite (y/n)?\n",argv[i+1]);
      scanf("%c",&c);
      if(c!='y')
        return EXIT_FAILURE;
    }
  }
  out=fopen(argv[i+1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  header.nbands=nbands;
  fwriteheader(out,&header,head,LPJ_CLIMATE_VERSION);
  file=fopen(argv[i],"rb");
  headerfile=fopen(argv[i],".hdr","rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[i],strerror(errno));
    return EXIT_FAILURE;
  }
        /* parse header  */
      if(fscanf(headerfile,"programm %d\n",&programm)!=1)
      {
        fputs("Error reading programm.\n",stderr);
        return EXIT_FAILURE;
      }
      if(fscanf(headerfile,"description %d\n",&description)!=1)
      {
        fputs("Error reading description.\n",stderr);
        return EXIT_FAILURE;
      }
      if(fscanf(headerfile,"samples %d\n",&ncols)!=1)
      {
        fputs("Error reading ncols.\n",stderr);
        return EXIT_FAILURE;
      }
      if(fscanf(headerfile,"lines %d\n",&nrows)!=1)
      {
        fputs("Error reading nrows.\n",stderr);
        return EXIT_FAILURE;
      }
    if(readintcoord(file->file,&icoord,gridfile->swap))
      coord->lat=icoord.lat*coordfile->scalar;
    coord->lon=icoord.lon*coordfile->scalar;
   
  switch(type)
  {
    case LPJ_INT:
      data_int=newmatrix(int,nrows*ncols,nbands);
      value_int=newmatrix(int,nrows*ncols,nbands);
      if(data_int==NULL || value_int==NULL)
      {
        fputs("Error allocating memory for data.\n",stderr);
        return EXIT_FAILURE;
      }
      break;
    case LPJ_SHORT:
      data=newmatrix(short,nrows*ncols,nbands);
      value=newmatrix(short,nrows*ncols,nbands);
      if(data==NULL || value==NULL)
      {
        fputs("Error allocating memory for data.\n",stderr);
        return EXIT_FAILURE;
      }
      break;
    case LPJ_FLOAT:
      data_float=newmatrix(float,nrows*ncols,nbands);
      value_float=newmatrix(float,nrows*ncols,nbands);
      if(data_float==NULL || value_float==NULL)
      {
        fputs("Error allocating memory for data.\n",stderr);
        return EXIT_FAILURE;
      }
  } /* of 'switch' */
  header.cellsize_lon=header.cellsize_lat=cellsize;
  header.ncell=0;
  if(type==LPJ_INT)
  {
    fseeko64(ifp,(long)(sizeof(int)*((row*ncols*years*nbands)+(col*years*nbands))),SEEK_SET);
    fread(value_int,sizeof(int),years*nbands,file);}
  else
        for(row=0;row<nrows;row++)
          for(col=0;col<ncols;col++)
          {
            if(fscanf(file,"%f",&value)!=1)
            {
              fprintf(stderr,"Error reading data at (%d,%d) at year %d and band %d.\n",
                      row+1,col+1,header.firstyear+y,m+1);
              return EXIT_FAILURE;
            }
            if(value!=nodata)
            {
              if(first && gridfile!=NULL)
              {
                grid.lon=xllcorner+col*cellsize+cellsize*0.5;
                grid.lat=yllcorner+(nrows-row-1)*cellsize+cellsize*0.5; 
                writefloatcoord(gridfile,&grid);
              }
              if(type==LPJ_SHORT)
                data[header.ncell][m]=(short)(value*scale);
              else
                data_float[header.ncell][m]=value;
              header.ncell++;
            }
          }
      if(first)
      {
        ncell_first=header.ncell;
        printf("Number of valid grid cells: %d\n",header.ncell);
      }
      else if(header.ncell!=ncell_first)
      {
        fprintf(stderr,"Number of cells %d in '%s' differs from %d.\n",
                header.ncell,argv[y*nbands+m],ncell_first);
        return EXIT_FAILURE;
      }
      first=FALSE;
      fclose(file);
    }
    switch(type)
    {
      case LPJ_INT:
        for(i=0;i<header.ncell;i++)
          if(fwrite(data_int[i],sizeof(int),nbands,out)!=nbands)
          {
            fprintf(stderr,"Error writing output: %s.\n",strerror(errno));
            return EXIT_FAILURE;
          }
        break;
      case LPJ_SHORT:
        for(i=0;i<header.ncell;i++)
          if(fwrite(data[i],sizeof(short),nbands,out)!=nbands)
          {
            fprintf(stderr,"Error writing output: %s.\n",strerror(errno));
            return EXIT_FAILURE;
          }
        break;
      case LPJ_FLOAT:
        for(i=0;i<header.ncell;i++)
          if(fwrite(data_float[i],sizeof(float),nbands,out)!=nbands)
          {
            fprintf(stderr,"Error writing output: %s.\n",strerror(errno));
            return EXIT_FAILURE;
          }
        break;
     }
  }  
  if(gridfile!=NULL)
  {
    /* write header for grid file */
    rewind(gridfile);
    header.nbands=2;
    header.nyear=1;
    header.scalar=1;
    header.datatype=LPJ_FLOAT;
    fwriteheader(gridfile,&header,LPJGRID_HEADER,LPJGRID_VERSION);
    fclose(gridfile);
  }
  /* write header for data file */
  rewind(out);
  header.nyear=nyear;
  header.datatype=type;
  header.nbands=nbands;
  header.scalar=(type==LPJ_SHORT) ? 1./scale : 1;
  fwriteheader(out,&header,head,LPJ_CLIMATE_VERSION);
  fclose(out);
  return EXIT_SUCCESS;
} /* of 'main' */
