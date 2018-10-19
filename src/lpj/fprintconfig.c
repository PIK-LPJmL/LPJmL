/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  c  o  n  f  i  g  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints LPJ configuration                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define notnull(s) (s==NULL) ? "" : s

static int printsim(FILE *file,int len,int *count,const char *s)
{
  len=fputstring(file,len,(*count) ? ", " : "Simulation with ",78);
  (*count)++;
  len=fputstring(file,len,s,78);
  return len;
} /* of 'printsim' */

static Bool isnetcdfinput(const Config *config)
{
  if(config->soil_filename.fmt==CDF)
    return TRUE;
  if(config->temp_filename.fmt==CDF)
    return TRUE;
  if(config->prec_filename.fmt==CDF)
    return TRUE;
#ifdef IMAGE
  if(config->temp_var_filename.fmt==CDF)
    return TRUE;
  if(config->prec_var_filename.fmt==CDF)
    return TRUE;
  if(config->prodpool_init_filename.fmt==CDF)
    return TRUE;
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
      if(config->lwnet_filename.fmt==CDF)
         return TRUE;
    if(config->swdown_filename.fmt==CDF)
       return TRUE;
  }
  else if(config->cloud_filename.fmt==CDF)
    return TRUE;
  if(config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)
  {
    if(config->tamp_filename.fmt==CDF)
       return TRUE;
    if(config->fire==SPITFIRE_TMAX && config->tmax_filename.fmt==CDF)
       return TRUE;
    if(config->wind_filename.fmt==CDF)
       return TRUE;
    if(config->lightning_filename.fmt==CDF)
       return TRUE;
    if(config->human_ignition_filename.fmt==CDF)
       return TRUE;
  }
  if(config->ispopulation && config->popdens_filename.fmt==CDF)
    return TRUE;
  if(config->grassfix_filename.name!=NULL && config->grassfix_filename.fmt==CDF)
    return TRUE;
  if(config->withlanduse!=NO_LANDUSE)
  {
    if(config->countrycode_filename.fmt==CDF)
      return TRUE;
    if(config->withlanduse!=ALL_CROPS && config->landuse_filename.fmt==CDF)
      return TRUE;
    if(config->sdate_option==PRESCRIBED_SDATE && config->sdate_filename.fmt==CDF)
      return TRUE;
  }
  if(config->reservoir)
  {
    if(config->elevation_filename.fmt==CDF)
      return TRUE;
    if(config->reservoir_filename.fmt==CDF)
      return TRUE;
  }
  if(config->wet_filename.name!=NULL && config->wet_filename.fmt==CDF)
    return TRUE;
  if(config->river_routing)
  {
    if(config->drainage_filename.fmt==CDF)
      return TRUE;
    if(config->lakes_filename.fmt==CDF)
      return TRUE;
    if(config->withlanduse!=NO_LANDUSE && config->neighb_irrig_filename.fmt==CDF)
      return TRUE;
  }
  if(config->prescribe_burntarea && config->burntarea_filename.fmt==CDF)
    return TRUE;
  if(config->prescribe_landcover && config->landcover_filename.fmt==CDF)
    return TRUE;

  if(config->wateruse_filename.name!=NULL && config->wateruse_filename.fmt==CDF)
    return TRUE;
  return FALSE;
} /* of 'isnetcdfinput' */

static void printoutname(FILE *file,const char *filename,Bool isoneyear,
                         const Config *config)
{
  char *fmt;
  char *pos;
  if(isoneyear)
  {
    fmt=malloc(strlen(filename)+6);
    if(fmt!=NULL)
    {
      pos=strstr(filename,"%d");
      strncpy(fmt,filename,pos-filename);
      fmt[pos-filename]='\0';
      strcat(fmt,"[%d-%d]");
      strcat(fmt,pos+2);
      fprintf(file,fmt,config->firstyear,config->lastyear);
      free(fmt);
    }
  }
  else
    fputs(filename,file);
} /* of printoutname' */

static char *fmt[]={"raw","clm","clm2","txt","fms","meta","cdf"};

static void printinputfile(FILE *file,const char *descr,const Filename *filename,
                           Bool iscdf)
{
  if(iscdf)
    fprintf(file,"%-10s %-4s %-8s %s\n",descr,fmt[filename->fmt],
            notnull(filename->var),notnull(filename->name));
  else
    fprintf(file,"%-10s %-4s %s\n",descr,fmt[filename->fmt],
            notnull(filename->name));
} /* of 'printinputfile' */

void fprintconfig(FILE *file,           /**< File pointer to text output file */
                  const Config *config, /**< LPJmL configuration */
                  int npft,             /**< Number of natural PFTs */
                  int ncft              /**< Number of crop PFTs */
                 )
{
  char *irrig[]={"no","limited","potential","all","irrigation on rainfed"};
  String s;
  int len;
  char *method[]={"write","MPI-2","gathered","socket"};
  int i,size,count=0;
  Bool isnetcdf,iscdfinput;
  fputs("==============================================================================\n",file);
  fprintf(file,"Simulation \"%s\"",config->sim_name);
  if(config->ntask>1)
    fprintf(file," running on %d tasks\n",config->ntask);
  else
    putc('\n',file);
  len=0;
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
    len=printsim(file,len,&count,"IMAGE coupling");
#endif
  if(config->wet_filename.name!=NULL)
    len=printsim(file,len,&count,"random precipitation");
  if(config->fire)
  {
    len=printsim(file,len,&count,(config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)  ? "spitfire" : "fire");
    if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->ispopulation)
      len=printsim(file,len,&count,"and population");
    if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->prescribe_burntarea)
      len=printsim(file,len,&count,"prescribe burntarea");
  }
  if(config->river_routing)
    len=printsim(file,len,&count,"river routing");
  if(config->permafrost)
    len=printsim(file,len,&count,"permafrost");
  if(config->prescribe_landcover)
    len=printsim(file,len,&count,(config->prescribe_landcover==LANDCOVEREST) ? "prescribed establishment":"prescribed maximum FPC");
  if(config->new_phenology)
    len=printsim(file,len,&count,"new phenology");

  if(config->withlanduse)
  {
    switch(config->withlanduse)
    {
      case CONST_LANDUSE:
        snprintf(s,STRING_LEN," const landuse set to year %d, ",config->landuse_year_const);
        len=printsim(file,len,&count,s);
        break;
      case ALL_CROPS:
        len=printsim(file,len,&count,"all crops land use, ");
        break;
      default:
        len=printsim(file,len,&count,"land use, ");
    } /* of switch */
    if(config->rw_manage)
      len=printsim(file,len,&count,"rainwater management, ");
    len=fputstring(file,len,irrig[config->irrig_scenario],78);
    len=fputstring(file,len," irrigation",78);
    if(config->intercrop)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"intercropping",78);
    }
    if(config->remove_residuals)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"remove residuals",78);
    }
    if(config->residues_fire)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"fire in residuals",78);
    }
    if(config->laimax_interpolate==LAIMAX_INTERPOLATE)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"interpolated LAImax",78);
    }
    else if(config->laimax_interpolate==CONST_LAI_MAX)
    {
      len+=fprintf(file,", ");
      snprintf(s,STRING_LEN,"const LAImax=%.1f",config->laimax);
      len=fputstring(file,len,s,78);
    }
    if(config->sdate_option==FIXED_SDATE)
    {
      len=fputstring(file,len,", ",78);
      count++;
      snprintf(s,STRING_LEN,"fixed sowing date after %d",config->sdate_fixyear);
      len=fputstring(file,len,s,78);
    }
    else if(config->sdate_option==PRESCRIBED_SDATE)
    {
      len=fputstring(file,len,", ",78);
      count++;
      len=fputstring(file,len,"prescribed sowing date",78);
    }
  }
  if(config->grassfix_filename.name!=NULL)
    len=printsim(file,len,&count,"grassland fixed PFT");
  if(config->firewood)
    len=printsim(file,len,&count,"wood fires");
  if(config->reservoir)
    len=printsim(file,len,&count,"dam reservoirs");
  if(config->wateruse_filename.name!=NULL)
    len=printsim(file,len,&count,"water use");
  if(count)
    fputs(".\n",file);
  fprintf(file,"Working directory: %s\n",getdir());
  if(isreadrestart(config))
    fprintf(file,"Starting from restart file '%s'.\n",config->restart_filename);
  else
    fputs("Starting from scratch.\n",file);
  iscdfinput=isnetcdfinput(config);
  fputs("Input files:\n",file);
  if(iscdfinput)
    fputs("Variable   Fmt  Varname  Filename\n"
          "---------- ---- -------- -----------------------------------------------------\n",file);
  else
    fputs("Variable   Fmt  Filename\n"
          "---------- ---- --------------------------------------------------------------\n",file);
  printinputfile(file,"soil",&config->soil_filename,iscdfinput);
  if(config->soil_filename.fmt!=CDF)
    printinputfile(file,"coord",&config->coord_filename,iscdfinput);
  printinputfile(file,"temp",&config->temp_filename,iscdfinput);
  printinputfile(file,"prec",&config->prec_filename,iscdfinput);
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
  {
    printinputfile(file,"temp var",&config->temp_var_filename,iscdfinput);
    printinputfile(file,"prec var",&config->prec_var_filename,iscdfinput);
    printinputfile(file,"prod pool",&config->prodpool_init_filename,iscdfinput);
  }
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
      printinputfile(file,(config->with_radiation==RADIATION) ? "lwnet" : "lwdown",
                     &config->lwnet_filename,iscdfinput);
    printinputfile(file,"swdown",&config->swdown_filename,iscdfinput);
  }
  else
    printinputfile(file,"cloud",&config->cloud_filename,iscdfinput);
  printinputfile(file,"co2",&config->co2_filename,iscdfinput);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->tmax_filename.name!=NULL)
    {
      printinputfile(file,"temp min",&config->tamp_filename,iscdfinput);
      printinputfile(file,"temp max",&config->tmax_filename,iscdfinput);
    }
    else
      printinputfile(file,"temp ampl",&config->tamp_filename,iscdfinput);
    printinputfile(file,"windspeed",&config->wind_filename,iscdfinput);
    printinputfile(file,"lightning",&config->lightning_filename,iscdfinput);
    printinputfile(file,"human ign",&config->human_ignition_filename,iscdfinput);
  }
  if(config->ispopulation)
    printinputfile(file,"pop. dens",&config->popdens_filename,iscdfinput);
  if(config->prescribe_burntarea)
    printinputfile(file,"burntarea",&config->burntarea_filename,iscdfinput);
  if(config->prescribe_landcover)
    printinputfile(file,"landcover",&config->landcover_filename,iscdfinput);
  if(config->grassfix_filename.name!=NULL)
    printinputfile(file,"Grassfix",&config->grassfix_filename,iscdfinput);
  if(config->withlanduse!=NO_LANDUSE)
  {
    printinputfile(file,"countries",&config->countrycode_filename,iscdfinput);
    if(config->countrycode_filename.fmt==CDF)
      printinputfile(file,"regions",&config->regioncode_filename,iscdfinput);
    if(config->withlanduse!=ALL_CROPS)
      printinputfile(file,"landuse",&config->landuse_filename,iscdfinput);
    if(config->sdate_option==PRESCRIBED_SDATE)
      printinputfile(file,"sdates",&config->sdate_filename,iscdfinput);
  }
  if(config->reservoir)
  {
    printinputfile(file,"elevation",&config->elevation_filename,iscdfinput);
    printinputfile(file,"reservoir",&config->reservoir_filename,iscdfinput);
  }
  if(config->wet_filename.name!=NULL)
    printinputfile(file,"wetdays",&config->wet_filename,iscdfinput);
  if(config->river_routing)
  {
    printinputfile(file,"drainage",&config->drainage_filename,iscdfinput);
    if(config->drainage_filename.fmt==CDF)
      printinputfile(file,"river",&config->river_filename,iscdfinput);
    printinputfile(file,"lakes",&config->lakes_filename,iscdfinput);
    if(config->withlanduse!=NO_LANDUSE)
      printinputfile(file,"neighbour",&config->neighb_irrig_filename,iscdfinput);
  }
  if(config->sim_id==LPJML_FMS)
    printinputfile(file,"runoff2ocean_map",&config->runoff2ocean_filename,iscdfinput);
  if(config->wateruse_filename.name!=NULL)
    printinputfile(file,"wateruse",&config->wateruse_filename,iscdfinput);
  if(iscdfinput)
    fputs("---------- ---- -------- -----------------------------------------------------\n",file);
  else
    fputs("---------- ---- --------------------------------------------------------------\n",file);
  if(config->param_out)
    fprintparam(file,npft,ncft,config);
  if(iswriterestart(config))
    fprintf(file,"Writing restart file '%s' after year %d.\n",
            config->write_restart_filename,config->restartyear);
  if(ischeckpointrestart(config))
    fprintf(file,"Checkpoint restart file: '%s'.\n",
            config->checkpoint_restart_filename);

#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
    fprintf(file,
            "Coupled to IMAGE model running on host %s using port %d and %d.\n"
            "Time to wait for connection: %6d sec\n",
            config->image_host,config->image_inport,
            config->image_outport,config->wait_image);

#endif
  if(config->wet_filename.name!=NULL)
    fprintf(file,"Random seed: %d\n",config->seed);
  if(config->n_out)
  {
    size=strlen("Variable");
    for(i=0;i<config->n_out;i++)
      if(config->outnames[config->outputvars[i].id].name!=NULL &&
         size<strlen(config->outnames[config->outputvars[i].id].name))
        size=strlen(config->outnames[config->outputvars[i].id].name);
    fprintf(file,"Number of output files:       %d\n"
                 "Output written in year:       %d\n"
                 "Byte order in output files:   %s\n"
                 "Output method:                %s",
            config->n_out,config->outputyear,
            bigendian() ? "big endian" : "little endian",
            method[config->outputmethod]);
    if(config->outputmethod==LPJ_SOCKET)
      fprintf(file," to %s using port %d",config->hostname,config->port);
    fputc('\n',file);
    isnetcdf=FALSE;
    for(i=0;i<config->n_out;i++)
      if(config->outputvars[i].filename.fmt==CDF)
      {
        isnetcdf=TRUE;
        break;
      }
    if(isnetcdf)
    {
      if(config->compress)
        fprintf(file,"Compression level for NetCDF: %d\n",config->compress);
      fprintf(file,"Missing value in NetCDF:      %g\n"
                   "NetCDF grid:                  %s\n",
              config->missing_value,
              config->global_netcdf ? "global" : "local");
    }
    fprintf(file,"%*s Fmt Unit         Type   Filename\n",-size,"Variable");
    frepeatch(file,'-',size);
    fputs(" --- ------------ ------ ",file);
    frepeatch(file,'-',77-size-4-13-7);
    putc('\n',file);
    for(i=0;i<config->n_out;i++)
    {
      if(config->outnames[config->outputvars[i].id].name==NULL)
        fprintf(file,"%*d",size,config->outputvars[i].id);
      else
        fprintf(file,"%*s",size,config->outnames[config->outputvars[i].id].name);
      fprintf(file," %s %12s %6s ",fmt[config->outputvars[i].filename.fmt],
              strlen(config->outnames[config->outputvars[i].id].unit)==0 ? "-" : config->outnames[config->outputvars[i].id].unit,
              typenames[getoutputtype(config->outputvars[i].id)]);
      printoutname(file,config->outputvars[i].filename.name,config->outputvars[i].oneyear,config);
      putc('\n',file);
    }
    frepeatch(file,'-',size);
    fputs(" --- ------------ ------ ",file);
    frepeatch(file,'-',77-size-4-13-7);
    putc('\n',file);
    if(config->crop_index>=0)
    {
      fprintf(file,"CFT for daily output:        %s\n"
                   "Irrigation for daily output: %s\n",
              config->pftpar[config->crop_index].name,
              (config->crop_irrigation) ? "irrigated" : "rain fed");
    }
    if(config->pft_output_scaled)
      fputs("PFT-specific output is grid scaled.\n",file);
  }
  else
    fputs("No output files written.\n",file);
  if(config->nspinup)
    fprintf(file,"Spinup years:                %6d\n"
            "Cycle length during spinup:  %6d\n",
             config->nspinup,config->nspinyear);
  else
    fputs("No spinup years.\n",file);
  fprintf(file,"First year:                  %6d\n"
          "Last year:                   %6d\n",
          config->firstyear,config->lastyear);
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
    fprintf(file,"Start IMAGE coupling:        %6d\n",
            config->start_imagecoupling);
#endif
  if(config->firstgrid)
    fprintf(file,"Index of first cell:        %7d\n",config->firstgrid);
  fprintf(file,"Number of grid cells:       %7d\n",config->nall);
  fputs("==============================================================================\n",file);
  fflush(file);
} /* of 'fprintconfig' */
