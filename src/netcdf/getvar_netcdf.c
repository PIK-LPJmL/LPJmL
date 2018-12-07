/**************************************************************************************/
/**                                                                                \n**/
/**              g  e  t  v  a  r  _  n  e  t  c  d  f  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets variable and units in NetCDF file                            \n**/
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
#endif

Bool getvar_netcdf(Climatefile *file,    /**< climate data file */
                   const char *filename, /**< filename */
                   const char *var,      /**< variable name or NULL */
                   const char *units,    /**< units or NULL*/
                   const Config *config  /**< LPJ configuration */
                  )                      /** \return TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int i,nvars,rc;
  char name[NC_MAX_NAME+1];
  nc_type type;
#ifdef USE_UDUNITS
  char *fromstr,*newstr;
  size_t len;
  utUnit from,to;
#endif
  float f;
  double d;
  if(var==NULL)
  {
    nc_inq_nvars(file->ncid,&nvars);
    for(i=0;i<nvars;i++)
    {
      nc_inq_varname(file->ncid,i,name);
      if(strcmp(name,LON_NAME) && strcmp(name,LAT_NAME) && strcmp(name,TIME_NAME) && strcmp(name,"NamePFT"))
      {
        file->varid=i;
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
  else if(nc_inq_varid(file->ncid,var,&file->varid))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR406: Cannot find variable '%s' in '%s'.\n",var,filename);
    return TRUE;
  }
#ifdef USE_UDUNITS
  if(units==NULL || nc_inq_attlen(file->ncid,file->varid,"units",&len))
  {
    file->slope=1;
    file->intercept=0;
  }
  else
  {
    if(utInit(""))
    {
      file->slope=1;
      file->intercept=0;
      if(isroot(*config))
        fputs("ERROR430: Cannot initialize udunits.\n",stderr);
    }
    else
    {

      fromstr=malloc(len+1);
      if(fromstr==NULL)
      {
        utTerm();
        printallocerr("fromstr");
        return TRUE;
      }
      nc_get_att_text(file->ncid,file->varid,"units",fromstr);
      fromstr[len]='\0';
      /* if unit for precipitation is mm convert it to kg/m2/day */
      if(!isdaily(*file) && !strcmp(units,"kg/m2/day"))
        units="kg/m2/month";
      if(!strcmp(fromstr,"mm")|| !strcmp(fromstr,"mm/day"))
      {
         free(fromstr);
         fromstr=strdup("kg/m2/day");
      }
      if(!strcmp(fromstr,"-"))
      {
        file->slope=1;
        file->intercept=0;
      }
      else
      {
        utIni(&from);
        if(utScan(fromstr,&from))
        {
          if(isroot(*config))
            fprintf(stderr,"WARNING404: Invalid unit %s for variable %s in '%s', assume none.\n",fromstr,(var==NULL) ? name : var,filename); 
          file->slope=1;
          file->intercept=0;
        }
        else
        {
          if(strstr(units,"day")!=NULL && !isdaily(*file))
          {
            newstr=malloc(strlen(units)+3);
            if(newstr==NULL)
            {
              utTerm();
              printallocerr("fromstr");
              return TRUE;
            }
            strncpy(newstr,units,strlen(units)-3);
            newstr[strlen(units)-3]='\0';
            strcat(newstr,"month");
            utIni(&to);
            utScan(newstr,&to);
            free(newstr);
          }
          else
          {
            utIni(&to);
            utScan(units,&to);
          }
          if(utConvert(&from,&to,&file->slope,&file->intercept))
          {
            if(isroot(*config))
              fprintf(stderr,"WARNING401: Invalid conversion for %s from %s to %s in '%s', assume none.\n",
                      (var==NULL) ? name : var,fromstr,units,filename);
            file->slope=1;
            file->intercept=0;
          }
        }
#ifdef DEBUG
        printf("%s -> %s= %g %g\n",fromstr,units,file->slope,file->intercept);
#endif
      }
      free(fromstr);
      utTerm();
    }
  }
#else
  file->slope=1;
  file->intercept=0;
#endif
  nc_inq_vartype(file->ncid,file->varid,&type);
  switch(type)
  {
    case NC_INT:
      rc=nc_get_att_int(file->ncid,file->varid,"missing_value",&file->missing_value.i);
      if(rc)
      {
        rc=nc_get_att_int(file->ncid,file->varid,"_FillValue",&file->missing_value.i);
        if(rc)
          file->missing_value.i=MISSING_VALUE_INT;
      }
      file->datatype=LPJ_INT;
      break;
    case NC_FLOAT:
      rc=nc_get_att_float(file->ncid,file->varid,"missing_value",&file->missing_value.f);
      if(rc)
      {
        rc=nc_get_att_float(file->ncid,file->varid,"_FillValue",&file->missing_value.f);
        if(rc)
          file->missing_value.f=config->missing_value;
      }
      file->datatype=LPJ_FLOAT;
      break;
    case NC_SHORT:
      rc=nc_get_att_short(file->ncid,file->varid,"missing_value",&file->missing_value.s);
      if(rc)
      {
        rc=nc_get_att_short(file->ncid,file->varid,"_FillValue",&file->missing_value.s);
        if(rc)
          file->missing_value.s=MISSING_VALUE_SHORT;
      }
      file->datatype=LPJ_SHORT;
      break;
    case NC_DOUBLE:
      rc=nc_get_att_double(file->ncid,file->varid,"missing_value",&file->missing_value.d);
      if(rc)
      {
        rc=nc_get_att_double(file->ncid,file->varid,"_FillValue",&file->missing_value.d);
        if(rc)
          file->missing_value.d=config->missing_value;
      }
      file->datatype=LPJ_DOUBLE;
      break;
    default:
      if(isroot(*config))
        fprintf(stderr,"ERROR428: Invalid data type of %s in NetCDF file '%s'.\n",
                (var==NULL) ? name : var,filename);
      return TRUE;
  }
  if(rc)
    fprintf(stderr,"WARNING402: Cannot read missing value of %s in '%s': %s.\n",
            (var==NULL) ? name : var,filename,nc_strerror(rc));
  if(!nc_inq_atttype(file->ncid,file->varid,"add_offset",&type))
  {
    switch(type)
    {
      case NC_FLOAT:
        nc_get_att_float(file->ncid,file->varid,"add_offset",&f);
        file->intercept+=file->slope*f;
        break; 
      case NC_DOUBLE:
        nc_get_att_double(file->ncid,file->varid,"add_offset",&d);
        file->intercept+=file->slope*d;
        break; 
    }
  }
  if(!nc_inq_atttype(file->ncid,file->varid,"scale_factor",&type))
  {
    switch(type)
    {
      case NC_FLOAT:
        nc_get_att_float(file->ncid,file->varid,"scale_factor",&f);
        file->slope*=f;
        break; 
      case NC_DOUBLE:
        nc_get_att_double(file->ncid,file->varid,"scale_factor",&d);
        file->slope*=d;
        break; 
    }
  }
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'getvar_netcdf' */
