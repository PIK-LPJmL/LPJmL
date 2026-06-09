/**************************************************************************************/
/**                                                                                \n**/
/**                  g  e  t  c  o  u  n  t  r  y  .  c                            \n**/
/**                                                                                \n**/
/**     Extract country from grid file                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-list] [-json] countryfile outfile country ...\n"

static int findcountryname(const char *name,
                           const Countryname countryname[],
                           int ncountries)
{
  int i;
  for(i=0;i<ncountries;i++)
    if(!strcmp(name,countryname[i].alpha_3))
      return i;
  return NOT_FOUND;
} /* 'findcountryname' */

static Bool findcountry(const int country[],int n,int c)
{
  int i;
  for(i=0;i<n;i++)
    if(country[i]==c)
      return TRUE;
  return FALSE;
} /* of 'findcountry' */

int main(int argc,char **argv)
{
  FILE *file,*out,*gridfile;
  int i,*country,n,iarg,ngrid;
  char *endptr;
  char *out_json,*arglist;
  size_t offset,grid_offset;
  Header header,outheader;
  Filename grid_name;
  Coord *grid,res;
  Coordfile coordfile;
  Type grid_type;
  Metadata metadata;
  int code;
  int format;
  float cellsize_lon,cellsize_lat;
  Bool swap,rc,isregion,isjson,islist,gridswap,ismeta;
  outheader.nyear=1;
  outheader.nstep=1;
  outheader.timestep=1;
  outheader.firstcell=0;
  outheader.order=0;
  outheader.ncell=0;
  outheader.nbands=2;
  isjson=islist=FALSE;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-list"))
        islist=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if((islist && argc<iarg+1) || (!islist && argc<iarg+3))
  {
    fprintf(stderr,"Argument(s) missing.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
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
  grid_type=LPJ_SHORT;
  initmetadata(&metadata,NULL);
  file=openmetafile(&header,&metadata,&grid_name,&grid_type,&format,&swap,&offset,argv[iarg],TRUE);
  if(file==NULL)
    return EXIT_FAILURE;
  if(metadata.countrymap==NULL)
  {
    fprintf(stderr,"Country map is missing in '%s.\n",argv[iarg]);
    fclose(file);
    freemetadata(&metadata);
    return EXIT_FAILURE;
  }
  if(islist)
  {
    fclose(file);
    puts("List of country codes:\nCode Name");
    for(i=0;i<metadata.countrymap_size;i++)
       printf("%s  %s\n",metadata.countrymap[i].alpha_3,metadata.countrymap[i].name);
    freemetadata(&metadata);
    return EXIT_SUCCESS;
  }
  fseek(file,offset,SEEK_SET);
  if(header.nbands==1)
    isregion=FALSE;
  else if(header.nbands!=2)
  {
    fprintf(stderr,"Invalid number of bands=%d in `%s', must be 1 or 2.\n",
            header.nbands,argv[iarg]);
    fclose(file);
    freemetadata(&metadata);
    return EXIT_FAILURE;
  }
  else
    isregion=TRUE;
  if(grid_name.name==NULL)
  {
    fprintf(stderr,"Filename of grid file missing in '%s'.\n",argv[iarg]);
    fclose(file);
    freemetadata(&metadata);
    return EXIT_FAILURE;
  }
  n=argc-iarg-2;
  country=newvec(int,n);
  if(country==NULL)
  {
    printallocerr("country");
    free(grid_name.name);
    fclose(file);
    freemetadata(&metadata);
    return EXIT_FAILURE;
  }
  for(i=0;i<n;i++)
  {
    country[i]=strtol(argv[iarg+2+i],&endptr,10);
    if(*endptr!='\0')
    {
      /* argument is not a number */
      country[i]=findcountryname(argv[iarg+2+i],metadata.countrymap,metadata.countrymap_size);
      if(country[i]==NOT_FOUND)
      {
        fprintf(stderr,"Invalid number/name '%s' for country.\n",argv[iarg+2+i]);
        free(grid_name.name);
        free(country);
        fclose(file);
        freemetadata(&metadata);
        return EXIT_FAILURE;
      }
    }
  }
  freemetadata(&metadata);
  coordfile=opencoord(&grid_name,TRUE);
  if(coordfile==NULL)
  {
    free(grid_name.name);
    free(country);
    fclose(file);
    return EXIT_FAILURE;
  }
  ngrid=numcoord(coordfile);
  if(ngrid!=header.ncell)
  {
    fprintf(stderr,"Number of cells %d in grid file '%s' is different from number of cells %d in '%s'.\n",
            ngrid,grid_name.name,header.ncell,argv[iarg]);
    free(grid_name.name);
    closecoord(coordfile);
    free(country);
    fclose(file);
    return EXIT_FAILURE;
  }
  getcellsizecoord(&cellsize_lon,&cellsize_lat,coordfile);
  res.lon=cellsize_lon;
  res.lat=cellsize_lat;
  grid=newvec(Coord,numcoord(coordfile));
  if(grid==NULL)
  {
    printallocerr("grid");
    free(grid_name.name);
    closecoord(coordfile);
    free(country);
    fclose(file);
    return EXIT_FAILURE;
  }
  for(i=0;i<numcoord(coordfile);i++)
    readcoord(coordfile,grid+i,&res);
  outheader.nstep=1;
  outheader.nbands=2;
  outheader.timestep=1;
  outheader.order=CELLYEAR;
  outheader.firstyear=header.firstyear;
  outheader.datatype=grid_type;
  outheader.cellsize_lon=cellsize_lon;
  outheader.cellsize_lat=cellsize_lat;
  outheader.scalar=getcoordscale(coordfile);
  closecoord(coordfile);
  if(isjson && grid_name.fmt==META)
  {
    ismeta=TRUE;
    initmetadata(&metadata,NULL);
    gridfile=openmetafile(&outheader,&metadata,NULL,NULL,NULL,&gridswap,&grid_offset,grid_name.name,TRUE);
    if(gridfile==NULL)
      ismeta=FALSE;
    else
      fclose(gridfile);
  }
  else
    ismeta=FALSE;
  free(grid_name.name);
  outheader.firstcell=0;
  outheader.ncell=0;
  out=fopen(argv[iarg+1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[iarg+1],strerror(errno));
    if(ismeta)
      freemetadata(&metadata);
    free(grid);
    free(country);
    fclose(file);
    return EXIT_FAILURE;
  }
  fwriteheader(out,&outheader,LPJGRID_HEADER,LPJGRID_VERSION);
  for(i=0;i<header.ncell;i++)
  {
    if(readcountrycode(file,&code,header.datatype,isregion,swap))
    {
      fprintf(stderr,"Error reading country code at %d.\n",i+1);
      if(ismeta)
        freemetadata(&metadata);
      free(grid);
      free(country);
      fclose(file);
      fclose(out);
      return EXIT_FAILURE;
    }
    if(findcountry(country,n,code))
    {
      rc=writecoord(out,grid+i,outheader.scalar,outheader.datatype);
      if(rc)
      {
        fprintf(stderr,"Error writing coordinate at %d.\n",i+1);
        if(ismeta)
          freemetadata(&metadata);
        free(grid);
        free(country);
        fclose(file);
        fclose(out);
        return EXIT_FAILURE;
      }
      outheader.ncell++;
    }
  }
  free(grid);
  free(country);
  fclose(file);
  rewind(out);
  fwriteheader(out,&outheader,LPJGRID_HEADER,LPJGRID_VERSION);
  fclose(out);
  if(isjson)
  {
    out_json=malloc(strlen(argv[iarg+1])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      if(ismeta)
        freemetadata(&metadata);
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[iarg+1]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    if(arglist==NULL)
    {
      printallocerr("arglist");
      free(out_json);
      if(ismeta)
        freemetadata(&metadata);
      return EXIT_FAILURE;
    }
    out=fopen(out_json,"w");
    if(out==NULL)
    {
      printfcreateerr(out_json);
      free(arglist);
      free(out_json);
      if(ismeta)
        freemetadata(&metadata);
      return EXIT_FAILURE;
    }
    if(!ismeta)
    {
      initmetadata(&metadata,NULL);
      metadata.source=argv[0];
      metadata.variable="grid";
      metadata.unit="degree";
      metadata.long_name="cell coordinates";
    }
    fprintjson(out,argv[iarg+1],NULL,arglist,&outheader,&metadata,NULL,LPJ_SHORT,CLM,LPJGRID_HEADER,FALSE,LPJGRID_VERSION);
    if(ismeta)
      freemetadata(&metadata);
    fclose(out);
    free(out_json);
    free(arglist);
  }
  if(header.ncell)
    printf("Number of cells: %d\n",outheader.ncell);
  else
    fputs("Warning: no cells found.\n",stderr);
  return EXIT_SUCCESS;
} /* of 'main' */
