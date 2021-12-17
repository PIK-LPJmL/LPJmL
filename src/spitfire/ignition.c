/**************************************************************************************/
/**                                                                                \n**/
/**                     i  g  n  i  t  i  o  n  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of datatype Human_ignition                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

struct human_ignition
{
  Climatefile file;
}; /* Definition of opaque datatype Human_ignition */

Human_ignition inithumanignition(const Config *config /**< LPJ configuration */
                                 )                     /** \return pointer to human ignition
                                                            struct or NULL */
{
  Header header;
  Human_ignition ignition;
  String headername;
  int version;
  size_t offset,filesize;

  if(config->human_ignition_filename.name==NULL)
    return NULL;
  ignition=new(struct human_ignition);
  if(ignition==NULL)
    return NULL;
  ignition->file.fmt=config->human_ignition_filename.fmt;
  if(config->human_ignition_filename.fmt==CDF)
  {
    if(opendata_netcdf(&ignition->file,&config->human_ignition_filename,"yr-1",config))
    {
      free(ignition);
      return NULL;
    }
  }
  else if(config->human_ignition_filename.fmt==SOCK)
  {
    if(isroot(*config))
    {
     send_token_copan(GET_DATA_SIZE,HUMAN_IGNITION_DATA,config);
     readint_socket(config->socket,&header.nbands,1);
    }
#ifdef USE_MPI
    MPI_Bcast(&header.nbands,1,MPI_INT,0,config->comm);
#endif
    ignition->file.var_len=header.nbands;
  }
  else
  {
    if((ignition->file.file=openinputfile(&header,&ignition->file.swap,
                                         &config->human_ignition_filename,
                                         headername,
                                         &version,&offset,TRUE,config))==NULL)
    {
      free(ignition);
      return NULL;
    }

    ignition->file.firstyear=header.firstyear;
    ignition->file.size=header.ncell*typesizes[header.datatype];
    ignition->file.scalar=header.scalar;
    ignition->file.datatype=header.datatype;
    ignition->file.nyear=header.nyear;
    ignition->file.var_len=header.nbands;
    if(config->human_ignition_filename.fmt==RAW)
      ignition->file.offset=config->startgrid*sizeof(short);
    else
    {
      ignition->file.offset=(config->startgrid-header.firstcell)*typesizes[header.datatype]+headersize(headername,version)+offset;
      if(isroot(*config) && config->human_ignition_filename.fmt!=META)
      {
         filesize=getfilesizep(ignition->file.file)-headersize(headername,version)-offset;
         if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
           fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->human_ignition_filename.name);
      }
    }
  }
  if(ignition->file.var_len>1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR218: Number of bands=%zu in population density file '%s' is not 1.\n",
              ignition->file.var_len,config->human_ignition_filename.name);
    closeclimatefile(&ignition->file,isroot(*config));
    free(ignition);
    return NULL;
  }
  if(isroot(*config) && config->human_ignition_filename.fmt!=SOCK && config->lastyear>ignition->file.firstyear+ignition->file.nyear-1)
    fprintf(stderr,"WARNING024: Last year in ignition data file=%d is less than last simulation year %d, data from last year used.\n",
            ignition->file.firstyear+ignition->file.nyear-1,config->lastyear);

  ignition->file.n=config->ngridcell;
  return ignition;
} /* of 'inithumanignition' */

/*
- called in lpj()
- opens the ignition input file (see also building file for the ignition Input (like cfts26_lu2clm.c)
- sets the ignition variables (see also manage.h)
*/

Bool gethumanignition(Human_ignition ignition, /**< pointer to population data */
                      int year,                /**< year (AD) */
                      Cell grid[],             /**< LPJ grid */
                      const Config *config     /**< LPJ configuration */
                     )                         /** \return TRUE on error */
{
  Real *data;
  char *name;
  int cell;
  year-=ignition->file.firstyear;
  if(year<0)
    year=0;
  if(year>=ignition->file.nyear)
    year=ignition->file.nyear-1;
  if((data=newvec(Real,ignition->file.n))==NULL)
  {
    printallocerr("data");
    return TRUE;
  } 
  if(ignition->file.fmt==CDF)
  {
    if(readdata_netcdf(&ignition->file,data,grid,year,config))
    {
      fprintf(stderr,"ERROR185: Cannot read ignition density of year %d from '%s'.\n",
              year+ignition->file.firstyear,config->human_ignition_filename.name);
      free(data);
      return TRUE;
    }
    return FALSE;
  }
  if(ignition->file.fmt==SOCK)
  {
    if(receive_real_copan(POPDENS_DATA,data,1,year,config))
    {
      fprintf(stderr,"ERROR149: Cannot receive ignition density of year %d.\n",year);
      free(data);
      return TRUE;
    }
    return FALSE;
  }
  if(fseek(ignition->file.file,year*ignition->file.size+ignition->file.offset,SEEK_SET))
  {
    name=getrealfilename(&config->human_ignition_filename);
    fprintf(stderr,"ERROR184: Cannot seek to ignition density of year %d in '%s'.\n",
            year+ignition->file.firstyear,name);
    free(name);
    free(data);
    return TRUE;
  }
  if(readrealvec(ignition->file.file,data,0,ignition->file.scalar,ignition->file.n,ignition->file.swap,ignition->file.datatype))
  {
    name=getrealfilename(&config->human_ignition_filename);
    fprintf(stderr,"ERROR185: Cannot read ignition density of year %d from '%s'.\n",
            year+ignition->file.firstyear,name);
    free(data);
    free(name);
    return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
   grid[cell].ignition.human=data[cell];
   if(grid[cell].ignition.human<0)
     grid[cell].ignition.human=0;
   grid[cell].ignition.human/=365;
  }
  free(data);
  return FALSE;
} /* of 'gethumanignition' */

void freehumanignition(Human_ignition ignition,Bool isroot)
{
  if(ignition!=NULL)
  {
    closeclimatefile(&ignition->file,isroot);
    free(ignition);
  }
} /* of 'freehumanignition' */
