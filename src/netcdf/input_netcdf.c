/**************************************************************************************/
/**                                                                                \n**/
/**             i  n  p  u  t  _  n  e  t  c  d  f  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens data file in NetCDF format                                  \n**/
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
#ifdef USE_UDUNITS
#include <udunits.h>
#endif

#define error(var,rc) if(rc) {if(isroot(*config))fprintf(stderr,"ERROR403: Cannot read '%s' in '%s': %s.\n",var,filename,nc_strerror(rc)); nc_close(input->ncid); free(input); return NULL;}
#endif
#define checkptr(ptr) if(ptr==NULL) {printallocerr(#ptr); return TRUE; }

struct input_netcdf
{
  int ncid;
  int varid;
  float lon_min,lat_min,lon_res,lat_res;
  double slope,intercept;
  size_t offset;
  size_t lon_len,lat_len;
  size_t var_len;
  Type type;
  union
  {
    Byte b;
    short s;
    int i;
    float f;
    double d;
  } missing_value;
};

void closeinput(Infile file,int fmt)
{
  if(fmt==CDF)
    closeinput_netcdf(file.cdf);
  else
    fclose(file.file);
} /* of 'closeinput' */

Input_netcdf dupinput_netcdf(const Input_netcdf input)
{
  Input_netcdf copy;
  copy=new(struct input_netcdf);
  if(copy==NULL)
    printallocerr("copy");
  else
    *copy=*input;
  return copy;
} /* of 'dupinput_netcdf' */

static Bool setvarinput_netcdf(Input_netcdf input,const char *filename,const char *var,
                               const char *units,const Config *config)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int i,rc,nvars,ndims;
  int *dimids;
  size_t len;
  nc_type type;
  char *fromstr;
  char name[NC_MAX_NAME+1];
#ifdef USE_UDUNITS
  utUnit from,to;
#endif
  if(input==NULL)
  {
    fputs("ERROR424: Invalid data pointer in setvarinput_netcdf().\n",stderr);
    return TRUE;
  }
  if(var==NULL)
  {
    nc_inq_nvars(input->ncid,&nvars);
    for(i=0;i<nvars;i++)
    {
      nc_inq_varname(input->ncid,i,name);
      if(strcmp(name,LON_NAME) && strcmp(name,LAT_NAME) && strcmp(name,TIME_NAME))
      {
        input->varid=i;
        break;
      }
    }
    if(i==nvars)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR405: No variable found in '%s'.\n",filename);
      return TRUE;
    }
  }
  else if(nc_inq_varid(input->ncid,var,&input->varid))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR406: Cannot find variable '%s' in '%s'.\n",
              var,filename);
    return TRUE;
  }
  nc_inq_varndims(input->ncid,input->varid,&ndims);
  if(ndims==2)
    input->var_len=0;
  else if (ndims==3)
  {
    dimids=newvec(int,ndims);
    if(dimids==NULL)
    {
      printallocerr("dimids");
      free_netcdf(input->ncid);
      return TRUE;
    }
    nc_inq_vardimid(input->ncid,input->varid,dimids);
    nc_inq_dimlen(input->ncid,dimids[0],&input->var_len);
    free(dimids);
  }
  else
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s'.\n",
              ndims,filename);
    return TRUE;
  }
  nc_inq_vartype(input->ncid,input->varid,&type);
  switch(type)
  {
    case NC_SHORT:
      rc=nc_get_att_short(input->ncid,input->varid,"missing_value",
                          &input->missing_value.s);
      if(rc)
      {
        rc=nc_get_att_short(input->ncid,input->varid,"_FillValue",
                            &input->missing_value.s);
        if(rc)
          input->missing_value.s=MISSING_VALUE_SHORT;
      }
      input->type=LPJ_SHORT;
      break;
    case NC_INT:
      rc=nc_get_att_int(input->ncid,input->varid,"missing_value",
                        &input->missing_value.i);
      if(rc)
      {
        rc=nc_get_att_int(input->ncid,input->varid,"_FillValue",
                          &input->missing_value.i);
        if(rc)
          input->missing_value.i=MISSING_VALUE_INT;
      }
      input->type=LPJ_INT;
      break;
    case NC_DOUBLE:
      rc=nc_get_att_double(input->ncid,input->varid,"missing_value",
                           &input->missing_value.d);
      if(rc)
      {
        rc=nc_get_att_double(input->ncid,input->varid,"_FillValue",
                             &input->missing_value.d);
        if(rc)
          input->missing_value.d=config->missing_value;
      }
      input->type=LPJ_DOUBLE;
      break;
    case NC_FLOAT:
      rc=nc_get_att_float(input->ncid,input->varid,"missing_value",
                          &input->missing_value.f);
      if(rc)
      {
        rc=nc_get_att_float(input->ncid,input->varid,"_FillValue",
                            &input->missing_value.f);
        if(rc)
          input->missing_value.f=config->missing_value;
      }
      input->type=LPJ_FLOAT;
      break;
    case NC_BYTE:
      rc=nc_get_att_uchar(input->ncid,input->varid,"missing_value",
                          &input->missing_value.b);
      if(rc)
      {
        rc=nc_get_att_uchar(input->ncid,input->varid,"_FillValue",
                            &input->missing_value.b);
        if(rc)
          input->missing_value.b=MISSING_VALUE_BYTE;
      }
      input->type=LPJ_BYTE;
      break;
    default:
      if(isroot(*config))
        fprintf(stderr,"ERROR428: Invalid data type of %s in NetCDF in '%s'.\n",
               (var==NULL) ? name : var,filename);
      return TRUE;
  }
  if(rc)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR402: Cannot read missing value of %s in '%s': %s.\n",
              (var==NULL) ? name : var,filename,nc_strerror(rc));
  }
#ifdef USE_UDUNITS
  if(units==NULL || nc_inq_attlen(input->ncid,input->varid,"units",&len))
  {
    input->slope=1;
    input->intercept=0;
  }
  else
  {
    if(utInit(""))
    {
      input->slope=1;
      input->intercept=0;
      if(isroot(*config))
        fputs("ERROR430: Cannot initialize udunits.\n",stderr);
    }
    else
    {
      fromstr=malloc(len+1);
      if(fromstr==NULL)
      {
        printallocerr("fromstr");
        utTerm();
        return TRUE;
      }
      nc_get_att_text(input->ncid,input->varid,"units",fromstr);
      fromstr[len]='\0';
      if(!strcmp(fromstr,"-"))
      {
        input->slope=1;
        input->intercept=0;
      }
      else
      {
        utIni(&to);
        utScan(units,&to);
        utIni(&from);
        utScan(fromstr,&from);
        if(utConvert(&from,&to,&input->slope,&input->intercept))
        {
          if(isroot(*config))
            fprintf(stderr,"ERROR414: Invalid conversion of %s from %s to %s in '%s'.\n",
                    (var==NULL) ? name : var,fromstr,units,filename);
          free(fromstr);
          utTerm();
          return TRUE;
        }
      }
      free(fromstr);
      utTerm();
    }
  }
#else
  input->slope=1;
  input->intercept=0;
#endif
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'setvarinput_netcdf' */
 
Input_netcdf openinput_netcdf(const char *filename, /**< filename */
                              const char *var,      /**< variable name or NULL */
                              const char *units,    /**< units or NULL */
                              size_t len,           /**< dim of variable */
                              const Config *config  /**< LPJ configuration */
                             )                      /** \return NULL on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  Input_netcdf input;
  int rc,var_id,*dimids,ndims,index;
  char name[NC_MAX_NAME+1];
  float *dim;
  if(filename==NULL)
  {
    fputs("ERROR424: Invalid data pointer in openinput_netcdf().\n",stderr);
    return NULL;
  }
  input=new(struct input_netcdf);
  if(input==NULL)
  {
    printallocerr("input");
    return NULL;
  }
  rc=nc_open(filename,NC_NOWRITE,&input->ncid);
  if(rc)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR409: Cannot open '%s': %s.\n",
              filename,nc_strerror(rc));
    free(input);
    return NULL;
  }
  if(setvarinput_netcdf(input,filename,var,units,config))
  {
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  if(len!=input->var_len)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR433: Invalid length %d of vector.\n",(int)input->var_len);
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  if(input->var_len==0)
    input->var_len=1;
  index=(input->var_len>1) ? 1 : 0;
  nc_inq_varndims(input->ncid,input->varid,&ndims);
  dimids=newvec(int,ndims);
  if(dimids==NULL)
  {
    printallocerr("dimids");
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  nc_inq_vardimid(input->ncid,input->varid,dimids);
  nc_inq_dimname(input->ncid,dimids[index+1],name);
  rc=nc_inq_varid(input->ncid,name,&var_id);
  if(rc)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
              name,filename,nc_strerror(rc));
    free(dimids);
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  nc_inq_dimlen(input->ncid,dimids[index+1],&input->lon_len);
  dim=newvec(float,input->lon_len);
  if(dim==NULL)
  {
    printallocerr("dim");
    free(dimids);
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  rc=nc_get_var_float(input->ncid,var_id,dim);
  if(rc)
  {
    free(dim);
    free(dimids);
    if(isroot(*config))
      fprintf(stderr,"ERROR410: Cannot read longitude in '%s': %s.\n",
              filename,nc_strerror(rc));
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  input->lon_min=dim[0];
  if(input->lon_len==1)
    input->lon_res=config->resolution.lon;
  else
    input->lon_res=(dim[input->lon_len-1]-dim[0])/(input->lon_len-1);
  if(fabs(input->lon_res-config->resolution.lon)/config->resolution.lon>1e-3)
  {
    if(isroot(*config))
      fprintf(stderr,"WARNING405: Incompatible resolution %g for longitude in '%s', not %g.\n",input->lon_res,filename,config->resolution.lon);
    //free(dim);
    //nc_close(input->ncid);
    //free(input);
    //return NULL;
  }
  free(dim);
  nc_inq_dimname(input->ncid,dimids[index],name);
  rc=nc_inq_varid(input->ncid,name,&var_id);
  if(rc)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
              name,filename,nc_strerror(rc));
    free(dimids);
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  nc_inq_dimlen(input->ncid,dimids[index],&input->lat_len);
  free(dimids);
  dim=newvec(float,input->lat_len);
  if(dim==NULL)
  {
    printallocerr("dim");
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  rc=nc_get_var_float(input->ncid,var_id,dim);
  if(rc)
  {
    free(dim);
    if(isroot(*config))
      fprintf(stderr,"ERROR411: Cannot read %s in '%s': %s.\n",
              name,filename,nc_strerror(rc));
    nc_close(input->ncid);
    free(input);
    return NULL;
  }
  if(input->lat_len==1)
  {
    input->lat_min=dim[0];
    input->lat_res=config->resolution.lat;
    input->offset=0;
  }
  else if(dim[1]>dim[0])
  {
    input->lat_min=dim[0];
    input->lat_res=(dim[input->lat_len-1]-dim[0])/(input->lat_len-1);
    input->offset=0;
  }
  else
  {
    input->lat_min=dim[input->lat_len-1];
    input->lat_res=(dim[0]-dim[input->lat_len-1])/(input->lat_len-1);
    input->offset=input->lat_len-1;
  }
  if(fabs(input->lat_res-config->resolution.lat)/config->resolution.lat>1e-3)
  {
    if(isroot(*config))
      fprintf(stderr,"WARNING406: Incompatible resolution %g for latitude in '%s', not %g.\n",input->lat_res,filename,config->resolution.lat);
    //free(dim);
    //nc_close(input->ncid);
    //free(input);
    //return NULL;
  }
  free(dim);
  return input;
#else
  if(isroot(*config))
    fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return NULL;
#endif

} /* of 'openinput_netcdf' */

void closeinput_netcdf(Input_netcdf input)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  if(input!=NULL)
  {
    nc_close(input->ncid);
    free(input);
  }
#endif
} /* of closeinput_netcdf' */

Type getinputtype_netcdf(const Input_netcdf input)
{
  return input->type;
} /* of getinputtype_netcdf' */

size_t getindexinput_netcdf(const Input_netcdf input,
                            const Coord *coord)
{
  size_t offsets[2];
  if(input->offset)
    offsets[0]=input->offset-(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  else
    offsets[0]=(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  offsets[1]=(int)((coord->lon-input->lon_min)/input->lon_res+0.5);
  return offsets[0]*input->lon_len+offsets[1];
} /* of 'getindexinput_netcdf' */

size_t getindexsize_netcdf(const Input_netcdf input)
{
  return input->lat_len*input->lon_len;
} /* of 'getindexsize_netcdf' */

Bool readinput_netcdf(const Input_netcdf input,Real *data,
                      const Coord *coord)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc;
  float *f;
  double *d;
  short *s;
  int *in;
  int index;
  size_t i;
  size_t offsets[3];
  size_t counts[3]={1,1,1};
  String line;
  if(input==NULL || data==NULL)
  {
    fputs("ERROR424: Invalid data pointer in readinput_netcdf().\n",stderr);
    return TRUE;
  }
  if(input->var_len>1)
  {
    counts[0]=input->var_len;
    offsets[0]=0;
    index=1;
  }
  else
    index=0;
  if(input->offset)
    offsets[index]=input->offset-(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  else
    offsets[index]=(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  offsets[index+1]=(int)((coord->lon-input->lon_min)/input->lon_res+0.5);
  switch(input->type)
  {
    case LPJ_FLOAT:
      f=newvec(float,input->var_len);
      checkptr(f);
      if((rc=nc_get_vara_float(input->ncid,input->varid,offsets,counts,f)))
      {
        fprintf(stderr,"ERROR415: Cannot read float data for cell (%s): %s.\n",
                sprintcoord(line,coord),nc_strerror(rc));
        free(f);
        return TRUE;
      }
      for(i=0;i<input->var_len;i++)
      {
        if(f[i]==input->missing_value.f)
        {
          fprintf(stderr,"ERROR423: Missing value for cell (%s).\n",
                  sprintcoord(line,coord));
          free(f);
          return TRUE;
        }
        else if(isnan(f[i]))
        {
          fprintf(stderr,"ERROR434: Invalid value for cell (%s).\n",
                  sprintcoord(line,coord));
          free(f);
          return TRUE;
        }
        data[i]=input->slope*f[i]+input->intercept;
      }
      free(f);
      break;
    case LPJ_DOUBLE:
      d=newvec(double,input->var_len);
      checkptr(d);
      if((rc=nc_get_vara_double(input->ncid,input->varid,offsets,counts,d)))
      {
        fprintf(stderr,"ERROR415: Cannot read double data for cell (%s): %s.\n",
                sprintcoord(line,coord),nc_strerror(rc));
        free(d);
        return TRUE;
      }
      for(i=0;i<input->var_len;i++)
      {
        if(d[i]==input->missing_value.d)
        {
          fprintf(stderr,"ERROR423: Missing value for cell (%s).\n",
                  sprintcoord(line,coord));
          free(d);
          return TRUE;
        }
        else if(isnan(d[i]))
        {
          fprintf(stderr,"ERROR434: Invalid value for cell (%s).\n",
                  sprintcoord(line,coord));
          free(d);
          return TRUE;
        }
        data[i]=input->slope*d[i]+input->intercept;
      }
      free(d);
      break;
    case LPJ_INT:
      in=newvec(int,input->var_len);
      checkptr(in);
      if((rc=nc_get_vara_int(input->ncid,input->varid,offsets,counts,in)))
      {
        fprintf(stderr,"ERROR415: Cannot read int data for cell (%s): %s.\n",
                sprintcoord(line,coord),nc_strerror(rc));
        free(in);
        return TRUE;
      }
      for(i=0;i<input->var_len;i++)
      {
        if(in[i]==input->missing_value.i)
        {
          fprintf(stderr,"ERROR423: Missing value for cell (%s).\n",
                  sprintcoord(line,coord));
          free(in);
          return TRUE;
        }
        data[i]=input->slope*in[i]+input->intercept;
      }
      free(in);
      break;
    case LPJ_SHORT:
      s=newvec(short,input->var_len);
      checkptr(s);
      if((rc=nc_get_vara_short(input->ncid,input->varid,offsets,counts,s)))
      {
        fprintf(stderr,"ERROR434: Invalid value for cell (%s).\n",
                sprintcoord(line,coord));
        free(s);
        return TRUE;
      }
      for(i=0;i<input->var_len;i++)
      {
        if(s[i]==input->missing_value.s)
        {
          fprintf(stderr,"ERROR423: Missing value for cell (%s).\n",
                  sprintcoord(line,coord));
          free(s);
          return TRUE;
        }
        data[i]=input->slope*s[i]+input->intercept;
      }
      free(s);
      break;
    default:
      fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      return TRUE;
  }
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'readinput_netcdf' */

Bool readintinput_netcdf(const Input_netcdf input,int *data,
                         const Coord *coord)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc,index;
  short *s;
  size_t i;
  size_t offsets[3];
  size_t counts[3]={1,1,1};
  String line;
  if(input==NULL || data==NULL)
  {
    fputs("ERROR424: Invalid data pointer in readintinput_netcdf().\n",stderr);
    return TRUE;
  }
  if(input->var_len>1)
  {
    counts[0]=input->var_len;
    offsets[0]=0;
    index=1;
  }
  else
    index=0;
  if(input->offset)
    offsets[index]=input->offset-(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  else
    offsets[index]=(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  offsets[index+1]=(int)((coord->lon-input->lon_min)/input->lon_res+0.5);
  switch(input->type)
  {
    case LPJ_INT:
      if((rc=nc_get_vara_int(input->ncid,input->varid,offsets,counts,data)))
      {
        fprintf(stderr,"ERROR415: Cannot read int data for cell (%s): %s.\n",
                sprintcoord(line,coord),nc_strerror(rc));
        return TRUE;
      }
      for(i=0;i<input->var_len;i++)
        if(data[i]==input->missing_value.i)
        {
          fprintf(stderr,"ERROR423: Missing value for cell (%s).\n",
                  sprintcoord(line,coord));
          return TRUE;
        }
      break;
    case LPJ_SHORT:
      s=newvec(short,input->var_len);
      checkptr(s);
      if((rc=nc_get_vara_short(input->ncid,input->varid,offsets,counts,s)))
      {
        fprintf(stderr,"ERROR415: Cannot read short data for cell (%s): %s.\n",
                sprintcoord(line,coord),nc_strerror(rc));
        free(s);
        return TRUE;
      }
      for(i=0;i<input->var_len;i++)
      {
        if(s[i]==input->missing_value.s)
        {
          fprintf(stderr,"ERROR423: Missing value for cell (%s).\n",
                  sprintcoord(line,coord));
          free(s);
          return TRUE;
        }
        data[i]=s[i];
      }
      free(s);
      break;
    default:
      fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      return TRUE;
  }
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'readintinput_netcdf' */

Bool readshortinput_netcdf(const Input_netcdf input,short *data,
                           const Coord *coord)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc,index;
  size_t i;
  size_t offsets[3];
  size_t counts[3]={1,1,1};
  String line;
  if(input==NULL || data==NULL)
  {
    fputs("ERROR424: Invalid data pointer in readshortinput_netcdf().\n",stderr);
    return TRUE;
  }
  if(input->var_len>1)
  {
    counts[0]=input->var_len;
    offsets[0]=0;
    index=1;
  }
  else
    index=0;
  if(input->offset)
    offsets[index]=input->offset-(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  else
    offsets[index]=(int)((coord->lat-input->lat_min)/input->lat_res+0.5);
  offsets[index+1]=(int)((coord->lon-input->lon_min)/input->lon_res+0.5);
  if((rc=nc_get_vara_short(input->ncid,input->varid,offsets,counts,data)))
  {
    fprintf(stderr,"ERROR415: Cannot read short data for cell (%s): %s.\n",
            sprintcoord(line,coord),nc_strerror(rc));
    return TRUE;
  }
  for(i=0;i<input->var_len;i++)
    if(data[i]==input->missing_value.s)
    {
      fprintf(stderr,"ERROR423: Missing value for cell (%s).\n",
              sprintcoord(line,coord));
      return TRUE;
    }
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'readshortinput_netcdf' */
