/**************************************************************************************/
/**                                                                                \n**/
/**            i  n  i  t  i  g  n  i  t  i  o  n  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJ                                                    \n**/
/**                                                                                \n**/
/**     Function reads human ignitions from file                                   \n**/ 
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool initignition(Cell grid[],         /* LPJ grid */
                  const Config *config /* LPJ configuration */
                 )                     /* returns TRUE on error */
{
  FILE *file;
  Header header;
  String headername;
  int version;
  Bool swap;
  int cell;
  size_t offset;
  Input_netcdf input_netcdf;
  if(config->human_ignition_filename.fmt==CDF)
  {
    input_netcdf=openinput_netcdf(config->human_ignition_filename.name,
                                  NULL,NULL,0,config);
    if(input_netcdf==NULL)
      return TRUE;
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(readinput_netcdf(input_netcdf,&grid[cell].ignition.human,
                          &grid[cell].coord))
      {
        closeinput_netcdf(input_netcdf);
        return TRUE;
      }
    }
    closeinput_netcdf(input_netcdf);
  }
  else
  {
    file=openinputfile(&header,&swap,&config->human_ignition_filename,
                       headername,&version,&offset,config);
    if(file==NULL)
    {
     printfopenerr(config->human_ignition_filename.name);
     return TRUE;
    }
    if(version<=1)
      header.scalar=0.001;
    if(config->human_ignition_filename.fmt!=RAW && header.nbands!=1)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in human ignition file '%s' is not 1.\n",
                header.nbands,config->human_ignition_filename.name);
      fclose(file);
      return TRUE;
    }
    if(fseek(file,(config->startgrid-header.firstcell)*typesizes[header.datatype]+offset,SEEK_CUR))
    {
      fprintf(stderr,"ERROR186: Cannot seek file for human ignition to position %d.\n",config->startgrid);
      fclose(file);
      return TRUE;
    }
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(readrealvec(file,&grid[cell].ignition.human,0,header.scalar,1,swap,header.datatype))
      {
        fputs("ERROR186: Cannot read human ignition.\n",stderr);
        fclose(file);
        return TRUE;
      }
      if(grid[cell].ignition.human<0)
        grid[cell].ignition.human=0;
      grid[cell].ignition.human/=365;
    }
    fclose(file);
  }
  return FALSE;
} /* of 'initignition' */
