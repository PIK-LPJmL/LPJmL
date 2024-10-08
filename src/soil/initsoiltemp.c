/**************************************************************************************/
/**                                                                                \n**/
/**              i  n  i  t  s  o  i  l  t  e  m  p  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define initfrozen -0.5

Bool initsoiltemp(Climate* climate,    /**< pointer to climate data */
                  Cell *grid,          /**< LPJ grid */
                  const Config *config /**< LPJmL configuration */
                 )                     /** \return TRUE on error */
{
  char *name;
  int year,s,l,day,dayofmonth;
  int month;
  int cell;
  Real whcs_all=0.0;
  Real temp,pet,prec,balance,dl,rad,delta,acos_dl;
  Stand* stand;
  Real nsoilmeanyears;
  nsoilmeanyears = min(30,climate->file_temp.nyear);
  for (year=climate->file_temp.firstyear; year < climate->file_temp.firstyear+nsoilmeanyears; ++year)
  {
    if(readclimate(&climate->file_temp,climate->data.temp,0,climate->file_temp.scalar,grid,year,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->temp_filename);
        fprintf(stderr,"ERROR131: Cannot read temperature of year %d from '%s'.\n",
                year,name);
        free(name);
      }
      return TRUE;
    }
    if(readclimate(&climate->file_prec,climate->data.prec,0,climate->file_prec.scalar,grid,year,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->prec_filename);
        fprintf(stderr,"ERROR131: Cannot read precipitation of year %d from '%s'.\n",
                year,name);
        free(name);
      }
      return TRUE;
    }
    day=0;
    foreachmonth(month)
    {
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
        {
          rad=deg2rad(grid[cell].coord.lat);
          delta=deg2rad(-23.4*cos(2*M_PI*(midday[month]+10.0)/NDAYYEAR));
          acos_dl=(-tan(delta)*tan(rad));
          if (acos_dl< -1.0)
            acos_dl=-1.0;
          else if (acos_dl>1.0)
            acos_dl=1.0;
          dl=2*acos(acos_dl)/0.2618;
          if(isdaily(climate->file_temp))
          {
            temp=0;
            foreachdayofmonth(dayofmonth,month)
              temp+=climate->data.temp[cell*NDAYYEAR+day+dayofmonth];
            temp*=ndaymonth1[month];
          }
          else
            temp=(getcelltemp(climate,cell))[month];
          if(isdaily(climate->file_prec))
          {
            prec=0;
            foreachdayofmonth(dayofmonth,month)
              prec+=climate->data.prec[cell*NDAYYEAR+day+dayofmonth];
          }
          else
            prec=(getcellprec(climate,cell))[month];
          pet=((temp>0) ? 924*dl*0.611*exp(17.3*temp/(temp+237.3))/(temp+273.2):0);

          balance=prec-pet;
          if(balance< epsilon)
            balance=0.0;
#ifdef COUPLING_WITH_FMS
          grid[cell].laketemp+=temp/NMONTH/nsoilmeanyears;
#endif
          foreachstand(stand,s,grid[cell].standlist)
          {
            whcs_all=0.0;
            foreachsoillayer(l)
              whcs_all+=stand->soil.whcs[l];
            foreachsoillayer(l)
            {
              stand->soil.temp[l]+=temp/NMONTH/nsoilmeanyears;
              stand->soil.w[l]+=balance/nsoilmeanyears/stand->soil.whcs[l]*stand->soil.whcs[l]/whcs_all;
            }
          }
        }
      day+=ndaymonth[month];
    } /* of foreachmonth */
  }
  for(cell=0;cell<config->ngridcell;cell++)
    if(!grid[cell].skip)
      foreachstand(stand,s,grid[cell].standlist)
        foreachsoillayer(l)
        {
          if(stand->soil.w[l]>1)
            stand->soil.w[l]=1.0;
          if(stand->soil.temp[l]<initfrozen)
          {
            stand->soil.ice_depth[l]=stand->soil.whcs[l]*stand->soil.w[l];
            stand->soil.w[l]=0;
            stand->soil.freeze_depth[l]=soildepth[l];
            stand->soil.ice_pwp[l]=1;
          }
        }
  /* initialse the enthalpy vectors based on temperature */
  Soil_thermal_prop therm; /* thermal properties of soil for calculation of enthalpy */
  int i,gridpoint;
  for(cell=0;cell<config->ngridcell;cell++)
    if(!grid[cell].skip)
      foreachstand(stand,s,grid[cell].standlist)
      {
        calc_soil_thermal_props(UNKNOWN,&therm,&(stand->soil),NULL,NULL,config->johansen,FALSE); /* thermal properties of soil depend on water content */
        foreachsoillayer(l)
        {
          for(i=0; i<GPLHEAT; ++i)
          { /* iterate through gridpoints of the refined heatgrid */
            gridpoint = GPLHEAT*l+i;
            /* Get the enthalpy corresponding to the temperature.
               when $temp=0$, we only know that $enth in [0,latent_heat]$,
               hence there is some freedom in choosing inital enth */
            stand->soil.enth[gridpoint] =
               (stand->soil.temp[l]<0 ?
                stand->soil.temp[l]*therm.c_frozen[gridpoint] :
                stand->soil.temp[l]*therm.c_unfrozen[gridpoint]+therm.latent_heat[gridpoint]);
          }
          stand->soil.wi_abs_enth_adj[l]=allwater((&(stand->soil)),l)+allice((&(stand->soil)),l);
          stand->soil.sol_abs_enth_adj[l]=soildepth[l]-stand->soil.wsats[l];
        }
      }
  return FALSE;
} /* of 'initsoiltemp' */
