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

static size_t isnetcdfinput(const Config *config)
{
  size_t width;
  width=0;
  if(config->soil_filename.fmt==CDF)
    width=max(width,strlen(config->soil_filename.var));
  if(config->temp_filename.fmt==CDF)
    width=max(width,strlen(config->temp_filename.var));
  if(config->prec_filename.fmt==CDF)
    width=max(width,strlen(config->prec_filename.var));
#ifdef IMAGE
  if(config->temp_var_filename.fmt==CDF)
    width=max(width,strlen(config->temp_var_filename.var));
  if(config->prec_var_filename.fmt==CDF)
    width=max(width,strlen(config->prec_var_filename.var));
  if(config->prodpool_init_filename.fmt==CDF)
    width=max(width,strlen(config->prodpool_init_filename.var));
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
      if(config->lwnet_filename.fmt==CDF)
        width=max(width,strlen(config->lwnet_filename.var));
    if(config->swdown_filename.fmt==CDF)
      width=max(width,strlen(config->swdown_filename.var));
  }
  else if(config->cloud_filename.fmt==CDF)
    width=max(width,strlen(config->cloud_filename.var));
  if(config->with_nitrogen)
  {
    if(config->with_nitrogen!=UNLIM_NITROGEN)
    {
      if(config->no3deposition_filename.fmt==CDF)
        width=max(width,strlen(config->no3deposition_filename.var));
      if(config->nh4deposition_filename.fmt==CDF)
        width=max(width,strlen(config->nh4deposition_filename.var));
    }
    if(config->soilph_filename.fmt==CDF)
      width=max(width,strlen(config->soilph_filename.var));
  }
  if(config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)
  {
    if(config->tamp_filename.fmt==CDF)
      width=max(width,strlen(config->tamp_filename.var));
    if(config->fire==SPITFIRE_TMAX && config->tmax_filename.fmt==CDF)
      width=max(width,strlen(config->tmax_filename.var));
    if(config->lightning_filename.fmt==CDF)
      width=max(width,strlen(config->lightning_filename.var));
    if(config->human_ignition_filename.fmt==CDF)
      width=max(width,strlen(config->human_ignition_filename.var));
  }
  if(config->ispopulation && config->popdens_filename.fmt==CDF)
    width=max(width,strlen(config->popdens_filename.var));
  if(config->grassfix_filename.name!=NULL && config->grassfix_filename.fmt==CDF)
    width=max(width,strlen(config->grassfix_filename.var));
  if(config->grassharvest_filename.name!=NULL && config->grassharvest_filename.fmt==CDF)
    width=max(width,strlen(config->grassharvest_filename.var));
  if(config->withlanduse!=NO_LANDUSE)
  {
    if(config->countrycode_filename.fmt==CDF)
      width=max(width,strlen(config->countrycode_filename.var));
    if(config->landuse_filename.fmt==CDF)
      width=max(width,strlen(config->landuse_filename.var));
    if(config->sdate_option==PRESCRIBED_SDATE && config->sdate_filename.fmt==CDF)
      width=max(width,strlen(config->sdate_filename.var));
    if(config->with_nitrogen && config->fertilizer_input && config->fertilizer_nr_filename.fmt==CDF)
      width=max(width,strlen(config->fertilizer_nr_filename.var));
    if(config->read_residue_data)
      width=max(width, strlen(config->residue_data_filename.var));
    if(config->tillage_type)
      width=max(width, strlen(config->with_tillage_filename.var));
  }
  if(config->reservoir)
  {
    if(config->elevation_filename.fmt==CDF)
      width=max(width,strlen(config->elevation_filename.var));
    if(config->reservoir_filename.fmt==CDF)
      width=max(width,strlen(config->reservoir_filename.var));
  }
  if(config->wet_filename.name!=NULL && config->wet_filename.fmt==CDF)
    width=max(width,strlen(config->wet_filename.var));
  if(config->river_routing)
  {
    if(config->drainage_filename.fmt==CDF)
      width=max(width,strlen(config->drainage_filename.var));
    if(config->lakes_filename.fmt==CDF)
      width=max(width,strlen(config->lakes_filename.var));
    if(config->withlanduse!=NO_LANDUSE && config->neighb_irrig_filename.fmt==CDF)
      width=max(width,strlen(config->neighb_irrig_filename.var));
  }
  if(config->prescribe_burntarea && config->burntarea_filename.fmt==CDF)
    width=max(width,strlen(config->burntarea_filename.var));
  if(config->prescribe_landcover && config->landcover_filename.fmt==CDF)
    width=max(width,strlen(config->landcover_filename.var));
  if(config->wateruse && config->wateruse_filename.fmt==CDF)
    width=max(width,strlen(config->wateruse_filename.var));
  if(width)
    width=max(width,strlen("Varname"));
  return width;
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
                           int width)
{
  if(width)
    fprintf(file,"%-10s %-4s %-*s %s\n",descr,fmt[filename->fmt],
            width,notnull(filename->var),notnull(filename->name));
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
  int i,count=0,width,width_unit;
  Bool isnetcdf;
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
  if(config->const_climate)
    len=printsim(file,len,&count,"const. climate");
  if(config->const_deposition)
    len=printsim(file,len,&count,"const. deposition");
  if(config->no_ndeposition)
    len=printsim(file,len,&count,"no N deposition");
  if(config->river_routing)
    len=printsim(file,len,&count,"river routing");
  if(config->with_nitrogen)
    len=printsim(file,len,&count,(config->with_nitrogen==UNLIM_NITROGEN) ? "unlimited nitrogen" : "nitrogen limitation");
  if(config->permafrost)
    len=printsim(file,len,&count,"permafrost");
  if(config->black_fallow)
  {
    len=printsim(file,len,&count,"black fallow");
    if(config->till_fallow)
      len=printsim(file,len,&count,"tillage fallow");
    if(config->prescribe_residues)
      len=printsim(file,len,&count,"prescribe residues");
  }
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
    if(config->fix_fertilization)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"fixed fertilization",78);
    }
    if(config->intercrop)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"intercropping",78);
    }
    if(config->istimber)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"timber",78);
    }
    if(config->with_tillage)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"tillage",78);
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
    else if(config->laimax_interpolate==LAIMAX_PAR)
    {
      len+=fprintf(file,", ");
      len=fputstring(file,len,"pft.js LAImax",78);
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
  if(config->grassharvest_filename.name!=NULL)
    len=printsim(file,len,&count,"grassland harvest options");
  if(config->firewood)
    len=printsim(file,len,&count,"wood fires");
  if(config->reservoir)
    len=printsim(file,len,&count,"dam reservoirs");
  if(config->wateruse)
    len=printsim(file,len,&count,(config->wateruse==ALL_WATERUSE) ? "always water use" : "water use");
  if(count)
    fputs(".\n",file);
  fprintf(file,"Working directory: %s\n",getdir());
  if(isreadrestart(config))
    fprintf(file,"Starting from restart file '%s'.\n",config->restart_filename);
  else
    fputs("Starting from scratch.\n",file);
  width=(int)isnetcdfinput(config);
  fputs("Input files:\n",file);
  if(width)
  {
    fprintf(file,"Variable   Fmt  %-*s Filename\n"
          "---------- ---- ",width,"Varname");
    frepeatch(file,'-',width);
    fputc(' ',file);
    frepeatch(file,'-',79-18-width);
    fputc('\n',file);
  }
  else
    fputs("Variable   Fmt  Filename\n"
          "---------- ---- --------------------------------------------------------------\n",file);
  printinputfile(file,"soil",&config->soil_filename,width);
  if(config->soil_filename.fmt!=CDF)
    printinputfile(file,"coord",&config->coord_filename,width);
  printinputfile(file,"temp",&config->temp_filename,width);
  printinputfile(file,"prec",&config->prec_filename,width);
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
  {
    printinputfile(file,"temp var",&config->temp_var_filename,width);
    printinputfile(file,"prec var",&config->prec_var_filename,width);
    printinputfile(file,"prod pool",&config->prodpool_init_filename,width);
  }
#endif
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
      printinputfile(file,(config->with_radiation==RADIATION) ? "lwnet" : "lwdown",
                     &config->lwnet_filename,width);
    printinputfile(file,"swdown",&config->swdown_filename,width);
  }
  else
    printinputfile(file,"cloud",&config->cloud_filename,width);
  if(config->with_nitrogen)
  {
    if(config->with_nitrogen!=UNLIM_NITROGEN)
    {
      printinputfile(file,"no3_depo",&config->no3deposition_filename,width);
      printinputfile(file,"nh4_depo",&config->nh4deposition_filename,width);
    }
    printinputfile(file,"soilpH",&config->soilph_filename,width);
  }
  printinputfile(file,"co2",&config->co2_filename,width);
  if(config->with_nitrogen || config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    printinputfile(file,"windspeed",&config->wind_filename,width);
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->tmax_filename.name!=NULL)
    {
      printinputfile(file,"temp min",&config->tamp_filename,width);
      printinputfile(file,"temp max",&config->tmax_filename,width);
    }
    else
      printinputfile(file,"temp ampl",&config->tamp_filename,width);
    printinputfile(file,"lightning",&config->lightning_filename,width);
    printinputfile(file,"human ign",&config->human_ignition_filename,width);
  }
  if(config->ispopulation)
    printinputfile(file,"pop. dens",&config->popdens_filename,width);
  if(config->prescribe_burntarea)
    printinputfile(file,"burntarea",&config->burntarea_filename,width);
  if(config->prescribe_landcover)
    printinputfile(file,"landcover",&config->landcover_filename,width);
  if(config->grassfix_filename.name!=NULL)
    printinputfile(file,"Grassfix",&config->grassfix_filename,width);
  if(config->grassharvest_filename.name!=NULL)
    printinputfile(file,"Grassharvest",&config->grassharvest_filename,width);
  if(config->withlanduse!=NO_LANDUSE)
  {
    printinputfile(file,"countries",&config->countrycode_filename,width);
    if(config->countrycode_filename.fmt==CDF)
      printinputfile(file,"regions",&config->regioncode_filename,width);
    printinputfile(file,"landuse",&config->landuse_filename,width);
    if(config->sdate_option==PRESCRIBED_SDATE)
      printinputfile(file,"sdates",&config->sdate_filename,width);
    if(config->with_nitrogen && config->fertilizer_input)
      printinputfile(file,"fertilizer",&config->fertilizer_nr_filename,width);
  }
  if(config->reservoir)
  {
    printinputfile(file,"elevation",&config->elevation_filename,width);
    printinputfile(file,"reservoir",&config->reservoir_filename,width);
  }
  if(config->wet_filename.name!=NULL)
    printinputfile(file,"wetdays",&config->wet_filename,width);
  if(config->river_routing)
  {
    printinputfile(file,"drainage",&config->drainage_filename,width);
    if(config->drainage_filename.fmt==CDF)
      printinputfile(file,"river",&config->river_filename,width);
    printinputfile(file,"lakes",&config->lakes_filename,width);
    if(config->withlanduse!=NO_LANDUSE)
      printinputfile(file,"neighbour",&config->neighb_irrig_filename,width);
  }
  if(config->sim_id==LPJML_FMS)
    printinputfile(file,"runoff2ocean_map",&config->runoff2ocean_filename,width);
  if(config->wateruse)
    printinputfile(file,"wateruse",&config->wateruse_filename,width);
  if(width)
  {
    fputs("---------- ---- ",file);
    frepeatch(file,'-',width);
    fputc(' ',file);
    frepeatch(file,'-',79-18-width);
    fputc('\n',file);
  }
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
    width=strlen("Variable");
    width_unit=strlen("Unit");
    for(i=0;i<config->n_out;i++)
    {
      if(config->outnames[config->outputvars[i].id].name!=NULL &&
         width<strlen(config->outnames[config->outputvars[i].id].name))
        width=strlen(config->outnames[config->outputvars[i].id].name);
      width_unit=max(width_unit,strlen(config->outnames[config->outputvars[i].id].unit));
    }
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
    fprintf(file,"%*s Fmt %*s Type  Filename\n",-width,"Variable",-width_unit,"Unit");
    frepeatch(file,'-',width);
    fputs(" --- ",file);
    frepeatch(file,'-',width_unit);
    fputs(" ----- ",file);
    frepeatch(file,'-',77-width-4-width_unit-7);
    putc('\n',file);
    for(i=0;i<config->n_out;i++)
    {
      if(config->outnames[config->outputvars[i].id].name==NULL)
        fprintf(file,"%*d",width,config->outputvars[i].id);
      else
        fprintf(file,"%*s",width,config->outnames[config->outputvars[i].id].name);
      fprintf(file," %s %*s %5s ",fmt[config->outputvars[i].filename.fmt],
              width_unit,strlen(config->outnames[config->outputvars[i].id].unit)==0 ? "-" : config->outnames[config->outputvars[i].id].unit,
              typenames[getoutputtype(config->outputvars[i].id)]);
      printoutname(file,config->outputvars[i].filename.name,config->outputvars[i].oneyear,config);
      putc('\n',file);
    }
    frepeatch(file,'-',width);
    fputs(" --- ",file);
    frepeatch(file,'-',width_unit);
    fputs(" ----- ",file);
    frepeatch(file,'-',77-width-4-width_unit-7);
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
  {
    if(config->isfirstspinupyear)
      fprintf(file,"First spinup year:           %6d\n",config->firstspinupyear);
    fprintf(file,"Spinup years:                %6d\n"
            "Cycle length during spinup:  %6d\n",
             config->nspinup,config->nspinyear);
  }
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
