/**************************************************************************************/
/**                                                                                \n**/
/**                      t  x  t  2  g  r  i  d  .  c                              \n**/
/**                                                                                \n**/
/**     Program converts text data to grid clm file format. Data must be           \n**/
/**     in the following format:                                                   \n**/
/**                                                                                \n**/
/**     Id,Lon,Lat,X_CRU,Y_CRU                                                     \n**/
/**     1,-179.75,-16.25,1,148                                                     \n**/
/**     2,..                                                                       \n**/
/**     or in a format specified by the -fmt option                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "\nUsage: txt2grid [-h] [-v] [-map file] [-fmt s] [-skip n] [-cellsize size] [-cellsize_lon size] [-cellsize_lat size]\n       [-float] [-double] [-latlon] [-json] gridfile clmfile\n"
#define ERR_USAGE USAGE "\nTry \"txt2grid --help\" for more information.\n"

typedef  struct
{
  float lon,lat;
} Float_coord;

typedef  struct
{
  double lon,lat;
} Double_coord;

static Bool scancoord(FILE *file,const char *fmt,Double_coord *coord,Bool latlon,Type type)
{
  Float_coord fcoord;
  char *pos,clon,clat;
  if((pos=strstr(fmt,"%c"))!=NULL && strstr(pos,"%c")!=NULL)
  {
    if(latlon)
    {
      if(type!=LPJ_DOUBLE)
      {
        if(fscanf(file,fmt,&fcoord.lat,&clat,&fcoord.lon,&clon)!=4)
          return FALSE;
        coord->lat=fcoord.lat;
        coord->lon=fcoord.lon;
      }
      else if(fscanf(file,fmt,&coord->lat,&clat,&coord->lon,&clon)!=4)
        return FALSE;
    }
    else
    {
      if(type!=LPJ_DOUBLE)
      {
        if(fscanf(file,fmt,&fcoord.lon,&clon,&fcoord.lat,&clat)!=4)
          return FALSE;
        coord->lat=fcoord.lat;
        coord->lon=fcoord.lon;
      }
      else if(fscanf(file,fmt,&coord->lon,&clon,&coord->lat,&clat)!=4)
        return FALSE;
    }
    if(clon=='W')
      coord->lon= -coord->lon;
    else if(clon!='E')
    {
      fprintf(stderr,"Invalid character '%c' for longitude.\n",clon);
      return FALSE;
    }
    if(clat=='S')
      coord->lat= -coord->lat;
    else if(clat!='N')
    {
      fprintf(stderr,"Invalid character '%c' for latitude.\n",clat);
      return FALSE;
    }
    return TRUE;
  }
  else
  {
    if(type!=LPJ_DOUBLE)
    {
      if(fscanf(file,fmt,(latlon) ? &fcoord.lat : &fcoord.lon,(latlon) ? &fcoord.lon : &fcoord.lat)!=2)
        return FALSE;
      coord->lat=fcoord.lat;
      coord->lon=fcoord.lon;
      return TRUE;
    }
    else
     return (fscanf(file,fmt,(latlon) ? &coord->lat : &coord->lon,(latlon) ? &coord->lon : &coord->lat)==2);
  }
} /* of 'scancoord' */

int main(int argc,char **argv)
{
  FILE *file,*gridfile;
  Coordfile coordfile=NULL;
  Filename filename;
  const char *fmt;
  Coord grid,*grid_ref=NULL,res;
  String line;
  int i,iarg,nskip,n,index;
  Double_coord dcoord;
  Float_coord coord;
  Real dist_min;
  Header header;
  char *endptr,*map_name;
  char *arglist,*out_json;
  Bool latlon,verbose,isjson;
  fmt="%*d,%f,%f,%*d,%*d";
  Type type;
  nskip=1;
  header.cellsize_lon=header.cellsize_lat=0.5;
  type=LPJ_SHORT;
  latlon=verbose=isjson=FALSE;
  map_name=NULL;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {
        printf("   txt2grid (" __DATE__ ") Help\n"
               "   ===========================\n\n"
               "Convert text file to clm grid file for LPJmL version " LPJ_VERSION "\n"
               USAGE
               "\nArguments:\n"
               "-h,--help          print this help text\n"
               "-v,--verbose       verbose output\n"
               "-map file          mapping to nearest cell in grid file\n"
               "-fmt s             format string for text input, default is '%s'\n"
               "-cellsize size     cell size, default is %g\n"
               "-cellsize_lat size latitudinal cell size, default is %g\n"
               "-cellsize_lon size longitudinal cell size, default is %g\n"
               "-float             write float data, default is short\n"
               "-double            write double data, default is short\n"
               "-skip n            skip first n lines, default is one\n"
               "-latlon            read latitude then longitude\n"
               "-json              JSON metafile is created with suffix '.json'\n"
               "gridfile           filename of grid text file\n"
               "clmfile            filename of clm data file\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               fmt,header.cellsize_lon,header.cellsize_lat,header.cellsize_lon);
        return EXIT_SUCCESS;
      }
      if(!strcmp(argv[iarg],"-fmt"))
      {
        if(iarg==argc-1)
        {
           fputs("Argument missing after '-fmt' option.\n"
                 ERR_USAGE,stderr);
           return EXIT_FAILURE;
        }
        fmt=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-v") || !strcmp(argv[iarg],"--verbose"))
        verbose=TRUE;
      else if(!strcmp(argv[iarg],"-float"))
        type=LPJ_FLOAT;
      else if(!strcmp(argv[iarg],"-double"))
        type=LPJ_DOUBLE;
      else if(!strcmp(argv[iarg],"-latlon"))
        latlon=TRUE;
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
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
          fprintf(stderr,"Invalid number '%s' for '-cellsize' option.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-cellsize_lat"))
      {
        if(iarg==argc-1)
        {
           fputs("Argument missing after '-cellsize_lat' option.\n"
                 ERR_USAGE,stderr);
           return EXIT_FAILURE;
        }
        header.cellsize_lat=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for '-cellsize_lat' option.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-cellsize_lon"))
      {
        if(iarg==argc-1)
        {
           fputs("Argument missing after '-cellsize_lon' option.\n"
                 ERR_USAGE,stderr);
           return EXIT_FAILURE;
        }
        header.cellsize_lon=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for '-cellsize_lon' option.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-map"))
      {
        if(iarg==argc-1)
        {
           fputs("Argument missing after '-map' option.\n"
                 ERR_USAGE,stderr);
           return EXIT_FAILURE;
        }
        map_name=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-skip"))
      {
        if(iarg==argc-1)
        {
           fputs("Argument missing after '-skip' option.\n"
                 ERR_USAGE,stderr);
           return EXIT_FAILURE;
        }
        nskip=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for '-skip' option.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n"
                ERR_USAGE,argv[iarg]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+2)
  {
    fputs("Filenames missing.\n"
          ERR_USAGE,stderr);
    return EXIT_FAILURE;
  }
  if(map_name!=NULL)
  {
     filename.name=map_name;
     filename.fmt=CLM;
     coordfile=opencoord(&filename,TRUE);
     if(coordfile==NULL)
       return EXIT_FAILURE;
     grid_ref=newvec(Coord,numcoord(coordfile));
     getcellsizecoord(&header.cellsize_lon,&header.cellsize_lat,coordfile);
     res.lon=header.cellsize_lon;
     res.lat=header.cellsize_lat;
     for(i=0;i<numcoord(coordfile);i++)
       if(readcoord(coordfile,grid_ref+i,&res))
       {
         fprintf(stderr,"Error reading coordinate %d in '%s'.\n",i+1,map_name);
         return EXIT_FAILURE;
       }
  }
  file=fopen(argv[iarg],"r");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg],strerror(errno));
    return EXIT_FAILURE;
  }
  for(n=0;n<nskip;n++)
    if(fgets(line,STRING_LEN,file)==NULL)
    {
      fprintf(stderr,"Error skipping file header in '%s'.\n",argv[iarg]);
      return EXIT_FAILURE;
    }
  header.ncell=0;
  header.nbands=2;
  header.order=CELLYEAR;
  header.firstcell=0;
  header.firstyear=0;
  header.nyear=1;
  header.nstep=1;
  header.timestep=1;
  header.datatype=type;
  if(type==LPJ_SHORT)
  {
    header.scalar=0.01;
  }
  else
  {
    header.scalar=1;
  }
  gridfile=fopen(argv[iarg+1],"wb");
  if(gridfile==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[iarg+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(gridfile,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  while(scancoord(file,fmt, &dcoord,latlon,type))
  {
    if(map_name!=NULL)
    {
      grid.lon=dcoord.lon;
      grid.lat=dcoord.lat;
      if(verbose)
      {
        fputs("Coordinate ",stdout);
        printcoord(&grid);
        fputs(" mapped to ",stdout);
      }
      index=findnextcoord(&dist_min,&grid,grid_ref,numcoord(coordfile));
      if(verbose)
      {
        printcoord(grid_ref+index);
        printf(", distance=%g\n",dist_min);
      }
      dcoord.lon=grid_ref[index].lon;
      dcoord.lat=grid_ref[index].lat;
    }
    else if(verbose)
    {
      grid.lon=dcoord.lon;
      grid.lat=dcoord.lat;
      printcoord(&grid);
      putchar('\n');
    }
    switch(type)
    {
       case LPJ_FLOAT:
         coord.lat=(float)dcoord.lat;
         coord.lon=(float)dcoord.lon;
         fwrite(&coord,sizeof(coord),1,gridfile);
         break;
       case LPJ_DOUBLE:
         fwrite(&dcoord,sizeof(dcoord),1,gridfile);
         break;
       default:
         grid.lon=dcoord.lon;
         grid.lat=dcoord.lat;
         if(fabs(coord.lon*100-round(coord.lon*100))>1e-3)
           fprintf(stderr,"Warning: Longitude of %.6g at %d cannot be represented by short value of %g.\n",
                   coord.lon,header.ncell,round(coord.lon*100));
         if(fabs(coord.lat*100-round(coord.lat*100))>1e-3)
           fprintf(stderr,"Warning: Latitude of %.6g at %d cannot be represented by short value of %g.\n",
                   coord.lat,header.ncell,round(coord.lat*100));
         writecoord(gridfile,&grid);
    }
    header.ncell++;
  }
  if(header.ncell==0)
    fprintf(stderr,"Error: No grid cells written to '%s'.\n",argv[iarg+1]);
  else
    printf("Number of grid cells: %d\n",header.ncell);
  rewind(gridfile);
  fwriteheader(gridfile,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  fclose(gridfile);
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
    gridfile=fopen(out_json,"w");
    if(gridfile==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    fprintjson(gridfile,argv[iarg+1],"txt2grid",NULL,arglist,&header,NULL,NULL,NULL,0,"grid","degree",NULL,"cell coordinates",NULL,LPJ_SHORT,CLM,LPJGRID_HEADER,FALSE,LPJGRID_VERSION);
    fclose(gridfile);

  }
  if(map_name!=NULL)
    closecoord(coordfile);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
