/**************************************************************************************/
/**                                                                                \n**/
/**                r  e  s  e  r  v  o  i  r  2  c  d  f  .  c                     \n**/
/**                                                                                \n**/
/**     Converts CLM reservoir file into NetCDF data file                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>

#define error(rc) if(rc) {free(lon);free(lat);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); free(cdf);return NULL;}

#define USAGE "Usage: %s [-h] [-v] [-metafile] [-longheader] [-global] [-cellsize size]\n       [[-attr name=value] ...] [-raw] [-compress level]\n       [gridfile] clmfile netcdffile\n"

typedef struct
{
  const Coord_array *index;
  int ncid;
  int varid_year;
  int varid_area;
  int varid_capacity;
  int varid_inst_cap;
  int varid_height;
  int varid_purpose;
} Cdf;

static Cdf *create_cdf(const char *filename,
                       const char *source,
                       const char *history,
                       float miss,
                       int imiss,
                       const char *args,
                       const Attr *global_attrs,
                       int n_global,
                       const Header *header,
                       int compress,
                       const Coord_array *array)
{
  Cdf *cdf;
  double *lon,*lat;
  int i,rc,dim[3];
  char *s;
  time_t t;
  int lat_var_id,lon_var_id,lat_dim_id,lon_dim_id;
  int purpose_dim_id;
  int len;
  cdf=new(Cdf);
  lon=newvec(double,array->nlon);
  if(lon==NULL)
  {
    printallocerr("lon");
    free(cdf);
    return NULL;
  }
  lat=newvec(double,array->nlat);
  if(lat==NULL)
  {
    printallocerr("lat");
    free(lon);
    free(cdf);
    return NULL;
  }
  for(i=0;i<array->nlon;i++)
    lon[i]=array->lon_min+i*header->cellsize_lon;
  for(i=0;i<array->nlat;i++)
    lat[i]=array->lat_min+i*header->cellsize_lat;
  cdf->index=array;
#ifdef USE_NETCDF4
  rc=nc_create(filename,NC_CLOBBER|NC_NETCDF4,&cdf->ncid);
#else
  rc=nc_create(filename,NC_CLOBBER,&cdf->ncid);
#endif
  if(rc)
  {
    fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
            filename,nc_strerror(rc));
    free(lon);
    free(lat);
    free(cdf);
    return NULL;
  }
  rc=nc_def_dim(cdf->ncid,LAT_DIM_NAME,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LON_DIM_NAME,array->nlon,&lon_dim_id);
  error(rc);
  if(source!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen(source),source);
    error(rc);
  }
  time(&t);
  if(history!=NULL)
  {
    len=snprintf(NULL,0,"%s\n%s: %s",history,strdate(&t),args);
    s=malloc(len+1);
    check(s);
    sprintf(s,"%s\n%s: %s",history,strdate(&t),args);
  }
  else
  {
    len=snprintf(NULL,0,"%s: %s",strdate(&t),args);
    s=malloc(len+1);
    check(s);
    sprintf(s,"%s: %s",strdate(&t),args);
  }
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
  free(s);
  error(rc);
  for(i=0;i<n_global;i++)
  {
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,global_attrs[i].name,strlen(global_attrs[i].value),global_attrs[i].value);
    error(rc);
  }
  rc=nc_def_var(cdf->ncid,LAT_NAME,NC_DOUBLE,1,&lat_dim_id,&lat_var_id);
  error(rc);
  rc=nc_def_var(cdf->ncid,LON_NAME,NC_DOUBLE,1,&lon_dim_id,&lon_var_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lon_var_id,"units",strlen("degrees_east"),
                     "degrees_east");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"long_name",strlen(LON_LONG_NAME),LON_LONG_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"standard_name",strlen(LON_STANDARD_NAME),LON_STANDARD_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"axis",strlen("X"),"X");
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lat_var_id,"units",strlen("degrees_north"),
                   "degrees_north");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"long_name",strlen(LAT_LONG_NAME),LAT_LONG_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"standard_name",strlen(LAT_STANDARD_NAME),LAT_STANDARD_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  rc=nc_def_dim(cdf->ncid,"purpose",NPURPOSE,&purpose_dim_id);
  error(rc);
  dim[0]=lat_dim_id;
  dim[1]=lon_dim_id;
  rc=nc_def_var(cdf->ncid,"year",NC_INT,2,dim,&cdf->varid_year);
  rc=nc_put_att_text(cdf->ncid, cdf->varid_year,"standard_name",strlen("year"),"year");
  nc_put_att_int(cdf->ncid, cdf->varid_year,"missing_value",NC_INT,1,&imiss);
  rc=nc_put_att_int(cdf->ncid, cdf->varid_year,"_FillValue",NC_INT,1,&imiss);
  error(rc);
  rc=nc_def_var(cdf->ncid,"area",NC_FLOAT,2,dim,&cdf->varid_area);
  rc=nc_put_att_text(cdf->ncid, cdf->varid_area,"units",strlen("km2"),"km2");
  nc_put_att_float(cdf->ncid, cdf->varid_area,"missing_value",NC_FLOAT,1,&miss);
  rc=nc_put_att_float(cdf->ncid, cdf->varid_area,"_FillValue",NC_FLOAT,1,&miss);
  error(rc);
  rc=nc_def_var(cdf->ncid,"capacity",NC_FLOAT,2,dim,&cdf->varid_capacity);
  rc=nc_put_att_text(cdf->ncid, cdf->varid_capacity,"units",strlen("km3"),"km3");
  nc_put_att_float(cdf->ncid, cdf->varid_capacity,"missing_value",NC_FLOAT,1,&miss);
  rc=nc_put_att_float(cdf->ncid, cdf->varid_capacity,"_FillValue",NC_FLOAT,1,&miss);
  error(rc);
  rc=nc_def_var(cdf->ncid,"inst_cap",NC_INT,2,dim,&cdf->varid_inst_cap);
  nc_put_att_int(cdf->ncid, cdf->varid_inst_cap,"missing_value",NC_INT,1,&imiss);
  rc=nc_put_att_int(cdf->ncid, cdf->varid_inst_cap,"_FillValue",NC_INT,1,&imiss);
  error(rc);
  rc=nc_def_var(cdf->ncid,"height",NC_INT,2,dim,&cdf->varid_height);
  nc_put_att_int(cdf->ncid, cdf->varid_height,"missing_value",NC_INT,1,&imiss);
  rc=nc_put_att_int(cdf->ncid, cdf->varid_height,"_FillValue",NC_INT,1,&imiss);
  rc=nc_put_att_text(cdf->ncid, cdf->varid_height,"units",strlen("m"),"m");
  rc=nc_put_att_text(cdf->ncid, cdf->varid_height,"standard_name",strlen("height"),"height");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, cdf->varid_height,"long_name",strlen("reservoir height"),"reservoir height");
  error(rc);
  dim[0]=purpose_dim_id;
  dim[1]=lat_dim_id;
  dim[2]=lon_dim_id;
  error(rc);
  rc=nc_def_var(cdf->ncid,"purpose",NC_INT,3,dim,&cdf->varid_purpose);
  error(rc);
  nc_put_att_int(cdf->ncid, cdf->varid_purpose,"missing_value",NC_INT,1,&imiss);
  rc=nc_put_att_int(cdf->ncid, cdf->varid_height,"_FillValue",NC_INT,1,&imiss);
#ifdef USE_NETCDF4
  if(compress)
  {
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid_year, 0, 1,compress);
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid_area, 0, 1,compress);
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid_capacity, 0, 1,compress);
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid_inst_cap, 0, 1,compress);
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid_height, 0, 1,compress);
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid_purpose, 0, 1,compress);
    error(rc);
  }
#endif
  rc=nc_enddef(cdf->ncid);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lon_var_id,lon);
  error(rc);
  free(lat);
  free(lon);
  return cdf;
} /* of 'create_cdf' */

static Bool write_reservoir_cdf(const Cdf *cdf,const Reservoir r[],int size,float miss,int imiss)
{
  int i,rc,index;
  size_t offsets[3],counts[3];
  float *grid;
  int *igrid;
  grid=newvec(float,cdf->index->nlon*cdf->index->nlat);
  if(grid==NULL)
  {
    printallocerr("grid");
    return TRUE;
  }
  igrid=newvec(int,cdf->index->nlon*cdf->index->nlat);
  if(igrid==NULL)
  {
    printallocerr("grid");
    return TRUE;
  }
  for(i=0;i<cdf->index->nlon*cdf->index->nlat;i++)
  {
    igrid[i]=imiss;
    grid[i]=miss;
  }
  for(i=0;i<size;i++)
    igrid[cdf->index->index[i]]=r[i].year;
  rc=nc_put_var_int(cdf->ncid,cdf->varid_year,igrid);
  if(rc!=NC_NOERR)
  {
    fprintf(stderr,"ERROR431: Cannot write output data for year: %s.\n",
            nc_strerror(rc));
    return TRUE;
  }
  for(i=0;i<size;i++)
    igrid[cdf->index->index[i]]=r[i].height;
  rc=nc_put_var_int(cdf->ncid,cdf->varid_height,igrid);
  for(i=0;i<size;i++)
    igrid[cdf->index->index[i]]=r[i].inst_cap;
  rc=nc_put_var_int(cdf->ncid,cdf->varid_inst_cap,igrid);
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=r[i].area;
  rc=nc_put_var_float(cdf->ncid,cdf->varid_area,grid);
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=r[i].capacity;
  rc=nc_put_var_float(cdf->ncid,cdf->varid_capacity,grid);
  offsets[1]=offsets[2]=0;
  counts[0]=1;
  counts[1]=cdf->index->nlat;
  counts[2]=cdf->index->nlon;
  for(index=0;index<NPURPOSE;index++)
  {
    offsets[0]=index;
    for(i=0;i<size;i++)
      igrid[cdf->index->index[i]]=r[i].purpose[index];
    rc=nc_put_vara_int(cdf->ncid,cdf->varid_purpose,offsets,counts,igrid);
    if(rc!=NC_NOERR)
    {
      fprintf(stderr,"ERROR431: Cannot write output data for purpose[%d]: %s.\n",
              index,nc_strerror(rc));
      return TRUE;
    }
  }
  free(grid);
  free(igrid);
  return FALSE;
} /* of 'write_float_cdf' */

static void close_cdf(Cdf *cdf)
{
  nc_close(cdf->ncid);
  free(cdf);
} /* of 'close_cdf' */

#endif
int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  FILE *file;
  Coordfile coordfile;
  Coord_array *index;
  Coord *grid,res;
  Cdf *cdf;
  Header header;
  String headername;
  size_t offset;
  Attr *global_attrs=NULL;
  Attr *global_attrs2=NULL;
  int i,ngrid,version,iarg,compress,setversion;
  Bool swap,isglobal,israw,ismeta;
  int n_global,n_global2;
  float cellsize_lon,cellsize_lat;
  Reservoir *rdata;
  char *endptr,*arglist;
  char *pos;
  const char *progname;
  char *grid_filename,*path;
  Filename grid_name;
  char *filename,*outname;
  size_t filesize;
  char *source=NULL,*history=NULL;
  float miss=MISSING_VALUE_FLOAT;
  int imiss=MISSING_VALUE_INT;
  compress=0;
  cellsize_lon=cellsize_lat=0;
  isglobal=FALSE;
  israw=FALSE;
  ismeta=FALSE;
  setversion=READ_VERSION;
  grid_name.fmt=CLM;
  n_global=0;
  progname=strippath(argv[0]);
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {
        printf("   reservoir2cdf (" __DATE__ ") Help\n"
               "   ================================\n\n"
               "Convert CLM reservoir data into NetCDF input data for LPJmL version %s\n\n",
               getversion());
        printf(USAGE
               "\nArguments:\n"
               "-h,--help        print this help text\n"
               "-v,--version     print LPJmL version\n"
               "-global          use global grid for NetCDF file\n"
               "-longheader      force version of CLM file to 2\n"
               "-cellsize s      set cell size, default is 0.5\n"
               "-metafile        set the input format to JSON metafile instead of CLM\n"
               "-raw             set the input format to raw instead of CLM\n"
               "-compress l      set compression level for NetCDF4 files\n"
               "-attr name=value set global attribute name to value in NetCDF file\n"
               "gridfile         filename of grid data file\n"
               "clmfile          filename of CLM data file\n"
               "netcdffile       filename of NetCDF file created\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               progname);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-v") || !strcmp(argv[iarg],"--version"))
      {
        puts(getversion());
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-global"))
        isglobal=TRUE;
      else if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[iarg],"-raw"))
        israw=TRUE;
      else if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
      else if(!strcmp(argv[iarg],"-attr"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,
                  "Error: Argument missing for '-attr' option.\n"
                 USAGE,progname);
          return EXIT_FAILURE;
        }
        pos=strchr(argv[++iarg],'=');
        if(pos==NULL)
        {
          fprintf(stderr,
                  "Error: Missing '=' for '-attr' option.\n"
                 USAGE,progname);
          return EXIT_FAILURE;
        }
        *pos='\0';
        global_attrs=realloc(global_attrs,(n_global+1)*sizeof(Attr));
        check(global_attrs);
        global_attrs[n_global].name=strdup(argv[iarg]);
        check(global_attrs[n_global].name);
        global_attrs[n_global].value=strdup(pos+1);
        check(global_attrs[n_global].value);
        n_global++;
      }
      else if(!strcmp(argv[iarg],"-cellsize"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-cellsize'.\n"
                  USAGE,progname);
          return EXIT_FAILURE;
        }
        cellsize_lon=cellsize_lat=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-cellsize'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-compress"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-compress'.\n"
                  USAGE,progname);
          return EXIT_FAILURE;
        }
        compress=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-compress'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: Invalid option '%s'.\n"
                USAGE,argv[iarg],progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(ismeta && argc==iarg+2)
  {
    filename=argv[iarg];
    outname=argv[iarg+1];
  }
  else if(argc<iarg+3)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            USAGE,progname);
    return EXIT_FAILURE;
  }
  else
  {
    filename=argv[iarg+1];
    outname=argv[iarg+2];
  }
  if(ismeta)
  {
    header.cellsize_lon=header.cellsize_lat=0.5;
    header.firstyear=1901;
    header.firstcell=0;
    header.nyear=1;
    header.nbands=10;
    header.nstep=1;
    header.datatype=LPJ_INT;
    header.order=CELLYEAR;

    file=openmetafile(&header,NULL,NULL,&global_attrs2,&n_global2,&source,&history,NULL,NULL,NULL,NULL,&grid_name,NULL,NULL,&swap,&offset,filename,TRUE);
    if(file==NULL)
      return EXIT_FAILURE;
    if(fseek(file,offset,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",filename,offset);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(global_attrs2!=NULL)
    {
      mergeattrs(&global_attrs,&n_global,global_attrs2,n_global2,FALSE);
      freeattrs(global_attrs2,n_global2);
    }
  }
  else
  {
    file=fopen(filename,"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s.\n",filename,strerror(errno));
      return EXIT_FAILURE;
    }
  }
  if(argc!=iarg+2)
  {
    grid_filename=argv[iarg];
  }
  else
  {
    if(grid_name.name==NULL)
    {
      fprintf(stderr,"Error: grid filename must be specified in '%s' metafile.\n",filename);
      return EXIT_FAILURE;
    }
    path=getpath(filename);
    grid_filename=addpath(grid_name.name,path);
    if(grid_filename==NULL)
    {
     printallocerr("name");
      return EXIT_FAILURE;
    }
    free(grid_name.name);
    free(path);
  }
  /* Open and read grid file */
  grid_name.name=grid_filename;
  coordfile=opencoord(&grid_name,TRUE);
  if(coordfile==NULL)
    return EXIT_FAILURE;
  ngrid=numcoord(coordfile);
  if(cellsize_lon>0)
    res.lon=res.lat=cellsize_lon;
  else
  {
    getcellsizecoord(&cellsize_lon,&cellsize_lat,coordfile);
    res.lon=cellsize_lon;
    res.lat=cellsize_lat;
  }
  grid=newvec(Coord,numcoord(coordfile));
  if(grid==NULL)
  {
    printallocerr("grid");
    return EXIT_FAILURE;
  }
  for(i=0;i<numcoord(coordfile);i++)
    readcoord(coordfile,grid+i,&res);
  closecoord(coordfile);
  if(!israw)
  {
    if(ismeta)
      version=4;
    else
    {
    version=setversion;
    if(freadanyheader(file,&header,&swap,headername,&version,TRUE))
    {
      fprintf(stderr,"Error reading header of '%s'.\n",filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version<3)
      header.datatype=LPJ_INT;
    if(version>CLM_MAX_VERSION)
    {
      fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
              version,filename,CLM_MAX_VERSION+1);
      fclose(file);
      return EXIT_FAILURE;
    }
    filesize=getfilesizep(file)-headersize(headername,version);
    if(filesize!=(long long)header.nyear*header.ncell*header.nbands*typesizes[header.datatype])
      fprintf(stderr,"Warning: File size of '%s' does not match nbands*ncell*nyear.\n",filename);
    }
    if(header.nyear>1 || header.nstep>1)
    {
      fprintf(stderr,"No time axis set, but number of time steps>1 in '%s'.\n",
              filename);
      fclose(file);
      return EXIT_FAILURE;
    }

    if(header.ncell!=ngrid)
    {
      fprintf(stderr,"Number of cells=%d in '%s' is different from %d in '%s'.\n",
              header.ncell,filename,ngrid,grid_filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version==1 && cellsize_lon>0)
    {
      header.cellsize_lon=cellsize_lon;
      header.cellsize_lat=cellsize_lat;
    }
    if(fabs(header.cellsize_lon-res.lon)>min(res.lon,header.cellsize_lon)*0.5)
    {
      fprintf(stderr,"Longitudinal cell size=%g in '%s' differs from %g in '%s'.\n",
              header.cellsize_lon,filename,res.lon,grid_filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(fabs(header.cellsize_lat-res.lat)>min(res.lat,header.cellsize_lat)*0.5)
    {
      fprintf(stderr,"Latitudinal cell size=%g in '%s' differs from %g in '%s'.\n",
              header.cellsize_lat,filename,res.lat,grid_filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(ismeta)
    {
      header.cellsize_lon=res.lon;
      header.cellsize_lat=res.lat;
    }
  }
  else
  {
    header.cellsize_lon=(float)res.lon;
    header.cellsize_lat=(float)res.lat;
    header.datatype=LPJ_INT;
    header.ncell=ngrid;
    header.nbands=10;
    header.nyear=getfilesizep(file)/typesizes[header.datatype]/ngrid/header.nbands;
    if(getfilesizep(file) % (typesizes[header.datatype]*ngrid*header.nbands))
      fprintf(stderr,"Warning: file size of '%s' is not multiple of bands %d and number of cells %d.\n",filename,header.nbands,ngrid);
  }
  if(header.nbands!=10)
  {
    fprintf(stderr,"Number of bands=%d in '%s' must be 10.\n",
            header.nbands,filename);
    fclose(file);
    return EXIT_FAILURE;
  }
  index=createindex(grid,ngrid,res,isglobal,FALSE);
  if(index==NULL)
    return EXIT_FAILURE;
  free(grid);
  arglist=catstrvec(argv,argc);
  cdf=create_cdf(outname,source,history,miss,imiss,arglist,global_attrs,n_global,&header,compress,index);
  free(arglist);
  if(cdf==NULL)
    return EXIT_FAILURE;
  rdata=newvec(Reservoir,ngrid);
  if(rdata==NULL)
  {
    printallocerr("data");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
  {
    if(readreservoir(rdata+i,swap,file))
    {
      fprintf(stderr,"Error reading reservoir data of cell %d.\n",i);
      return EXIT_FAILURE;
    }
    rdata[i].capacity*=1e-12; /* convert dm3 ->km3 */
  }
  if(write_reservoir_cdf(cdf,rdata,ngrid,miss,imiss))
    return EXIT_FAILURE;
  close_cdf(cdf);
  fclose(file);
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */
