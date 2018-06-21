/**************************************************************************************/
/**                                                                                \n**/
/**                      a  s  c  2  c  l  m  .  c                                 \n**/
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
#define USAGE "Usage: asc2clm [-h] [-f] [-firstyear y] [-grid file] [-nbands n] [-header s]\n       [-{int|float}] [-scale s] infile ... clmfile\n"
#define FIRSTYEAR 1901

int main(int argc,char **argv)
{
  FILE *file,*gridfile,*out;
  Bool first;
  float scale;
  int row,col,nrows,ncols,nyear,nbands,nodata_int;
  int nrows_first,ncols_first,ncell_first;
  float xllcorner_first,yllcorner_first,cellsize_first;
  float xllcorner,yllcorner,cellsize,nodata,value;
  short **data;
  int **data_int,value_int;
  float **data_float;
  char *endptr;
  Coord grid;
  Type type;
  Header header;
  char *head;
  int i,m,y;
  Bool force;
  struct stat buf;
  char c;
  header.firstyear=FIRSTYEAR;
  header.firstcell=0;
  header.datatype=LPJ_SHORT;
  nyear=1;
  nbands=NMONTH;
  header.order=CELLYEAR;
  scale=1;
  head=LPJ_CLIMATE_HEADER;
  gridfile=NULL;
  force=FALSE;
  type=LPJ_SHORT;
  /* parse command line options */
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("   asc2clm version " ASC2CLM " (" __DATE__ ") help\n"
               "   ==========================================\n\n"
               "Convert gridded ASCII files to clm data files for LPJ C version\n\n"
               USAGE
               "\nArguments:\n"
               "-h           print this help text\n"
               "-f           force overwrite of output file\n"
               "-firstyear y first year, default is %d\n"
               "-grid file   create grid file\n" 
               "-nbands n    number of bands, default is 12\n"
               "-header s    header string, default is " LPJ_CLIMATE_HEADER "\n"
               "-scale s     scale data by a factor of s\n"
               "-int         integer values to read\n"
               "-float       write float values\n"
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
        gridfile=fopen(argv[++i],"wb");
        if(gridfile==NULL)
        {
          fprintf(stderr,"Error creating '%s': %s.\n",argv[i],strerror(errno));
          return EXIT_FAILURE;
        }
        fwriteheader(gridfile,&header,LPJGRID_HEADER,LPJGRID_VERSION);

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
  argc-=i;
  argv+=i;
  if((argc-1)% nbands)
  {
    fputs("Number of input files is not multiple of nbands.\n",stderr);
    return EXIT_FAILURE;
  }
  nyear=(argc-1)/nbands;
  if(!force)
  {
    if(!stat(argv[argc-1],&buf))
    {
      fprintf(stderr,"File '%s' already exists, ovewrite (y/n)?\n",argv[argc-1]);
      scanf("%c",&c);
      if(c!='y')
        return EXIT_FAILURE;
    }
  }
  out=fopen(argv[argc-1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[argc-1],strerror(errno));
    return EXIT_FAILURE;
  }
  header.nbands=nbands;
  fwriteheader(out,&header,head,LPJ_CLIMATE_VERSION);
  first=TRUE;
  for(y=0;y<nyear;y++)
  {
    for(m=0;m<nbands;m++)
    {
      file=fopen(argv[y*nbands+m],"r");
      if(file==NULL)
      {
        fprintf(stderr,"Error opening '%s': %s.\n",argv[y],strerror(errno));
        return EXIT_FAILURE;
      }
      /* parse header  */
      if(fscanf(file,"ncols %d\n",&ncols)!=1)
      {
        fputs("Error reading ncols.\n",stderr);
        return EXIT_FAILURE;
      }
      if(fscanf(file,"nrows %d\n",&nrows)!=1)
      {
        fputs("Error reading nrows.\n",stderr);
        return EXIT_FAILURE;
      }
      if(fscanf(file,"xllcorner %f\n",&xllcorner)!=1)
      {
        fputs("Error reading xllcorner.\n",stderr);
        return EXIT_FAILURE;
      }
      if(fscanf(file,"yllcorner %f\n",&yllcorner)!=1)
      {
        fputs("Error reading yllcorner.\n",stderr);
        return EXIT_FAILURE;
      }
      if(fscanf(file,"cellsize %f\n",&cellsize)!=1)
      {
        fputs("Error reading cellsize.\n",stderr);
        return EXIT_FAILURE;
      }
      if(type==LPJ_INT)
      {
        if(fscanf(file,"NODATA_value %d\n",&nodata_int)!=1)
        {
          fputs("Error reading nodata value.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else
      {
        if(fscanf(file,"NODATA_value %f\n",&nodata)!=1)
        {
          fputs("Error reading nodata value.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      if(first)
      {
        switch(type)
        {
          case LPJ_INT:
            data_int=newmatrix(int,nrows*ncols,nbands);
            if(data_int==NULL)
            {
              fputs("Error allocating memory for data.\n",stderr);
              return EXIT_FAILURE;
            }
            break;
          case LPJ_SHORT:
            data=newmatrix(short,nrows*ncols,nbands);
            if(data==NULL)
            {
              fputs("Error allocating memory for data.\n",stderr);
              return EXIT_FAILURE;
            }
            break;
          case LPJ_FLOAT:
            data_float=newmatrix(float,nrows*ncols,nbands);
            if(data_float==NULL)
            {
              fputs("Error allocating memory for data.\n",stderr);
              return EXIT_FAILURE;
            }
        } /* of 'switch' */
        nrows_first=nrows;
        ncols_first=ncols;
        xllcorner_first=xllcorner;
        yllcorner_first=yllcorner;
        cellsize_first=cellsize;
      }
      else if(nrows_first!=nrows)
      {
        fprintf(stderr,"Number of rows %d in '%s' differs from %d.\n",
                nrows,argv[y],nrows_first);
        return EXIT_FAILURE;
      }
      else if(ncols_first!=ncols)
      {
        fprintf(stderr,"Number of columns %d in '%s' differs from %d.\n",
                ncols,argv[y],ncols_first);
        return EXIT_FAILURE;
      }
      else if(xllcorner_first!=xllcorner)
      {
        fprintf(stderr,"xllcorner %g in '%s' differs from %g.\n",
                xllcorner,argv[y],xllcorner_first);
        return EXIT_FAILURE;
      }
      else if(yllcorner_first!=yllcorner)
      {
        fprintf(stderr,"yllcorner %g in '%s' differs from %g.\n",
                yllcorner,argv[y],yllcorner_first);
        return EXIT_FAILURE;
      }
      else if(cellsize_first!=cellsize)
      {
        fprintf(stderr,"cellsize %g in '%s' differs from %g.\n",
                cellsize,argv[y],cellsize_first);
        return EXIT_FAILURE;
      }
      header.cellsize_lon=header.cellsize_lat=cellsize;
      header.ncell=0;
      if(type==LPJ_INT)
        for(row=0;row<nrows;row++)
          for(col=0;col<ncols;col++)
          {
            if(fscanf(file,"%d",&value_int)!=1)
            {
              fprintf(stderr,"Error reading data at (%d,%d) at year %d and band %d.\n",
                      row+1,col+1,header.firstyear+y,m+1);
              return EXIT_FAILURE;
            }
            if(value_int!=nodata_int)
            {
              if(first && gridfile!=NULL)
              {
                grid.lon=xllcorner+col*cellsize+cellsize*0.5;
                grid.lat=yllcorner+(nrows-row-1)*cellsize+cellsize*0.5; 
                writefloatcoord(gridfile,&grid);
              }
              data_int[header.ncell][m]=value_int;
              header.ncell++;
            }
          }
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
