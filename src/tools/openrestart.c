/**************************************************************************************/
/**                                                                                \n**/
/**                 o  p  e  n  r  e  s  t  a  r  t  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens restart file and seeks to first grid cell as                \n**/
/**     specified in LPJ configuration                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#define readint(file,name,var) \
if(bstruct_readint(file,name,var))\
  {\
    if(isroot(*config))\
      fprintf(stderr,"ERROR245: Cannot read header in %s file '%s'.\n",type,filename);\
    bstruct_finish(file);\
    return NULL; \
  }

#define readbool(file,name,var) \
if(bstruct_readbool(file,name,var))\
  {\
    if(isroot(*config))\
      fprintf(stderr,"ERROR245: Cannot read header in %s file '%s'.\n",type,filename);\
    bstruct_finish(file);\
    return NULL; \
  }

Bstruct openrestart(const char *filename, /**< filename of restart file */
                    Config *config,       /**< LPJ configuration */
                    int npft,             /**< number of natural PFTs */
                    int ncft              /**< number of crop PFTs */
                   )                      /** \return pointer to restart file or NULL */
{
  Bstruct file;
  char *lpjversion;
  Real cellsize_lon,cellsize_lat;
  int offset,ncell,restart_npft,restart_ncft,firstcell,firstyear;
  Bool separate_harvests,river_routing;
  Type datatype;
  char *type;
  /* Open restart file */
  file=bstruct_open(filename,isroot(*config));
  if(file==NULL)
    return NULL;
  type=(config->ischeckpoint) ? "checkpoint" : "restart";
  /* read header */
  if(bstruct_readbeginstruct(file,"header"))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR245: No header found in %s file '%s'.\n",type,filename);
    bstruct_finish(file);
    return NULL;
  }
  lpjversion=bstruct_readstring(file,"version");
  if(lpjversion==NULL)
  {
    bstruct_finish(file);
    return NULL;
  }
  if(isroot(*config) && strcmp(lpjversion,getversion()))
  {
    fprintf(stderr,"WARNING041: LPJmL version %s of %s file '%s' is not %s.\n",
            lpjversion,type,filename,getversion());
  }
  free(lpjversion);
  readint(file,"year",&firstyear);
  readint(file,"firstcell",&firstcell);
  readint(file,"npft",&restart_npft);
  readint(file,"ncft",&restart_ncft);
  if(bstruct_readreal(file,"cellsize_lat",&cellsize_lat))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR245: Cannot read header in %s file '%s'.\n",type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(bstruct_readreal(file,"cellsize_lon",&cellsize_lon))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR245: Cannot read header in %s file '%s'.\n",type,filename);
    bstruct_finish(file);
    return NULL;
  }
  readint(file,"datatype",(int *)(&datatype));
  readbool(file,"landuse",&config->landuse_restart);
  readint(file,"sdate_option",&config->sdate_option_restart);
  readbool(file,"crop_phu_option",&config->crop_phu_option_restart);
  readbool(file,"river_routing",&river_routing);
  readbool(file,"separate_harvests",&separate_harvests);
  if(freadseed(file,"seed",config->seed))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR245: Cannot read header in %s file '%s'.\n",type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(bstruct_readendstruct(file,"header"))
  {
    bstruct_finish(file);
    return NULL;
  }
  if(bstruct_readbeginarray(file,"grid",&ncell))
  {
    bstruct_finish(file);
    return NULL;
  }
  /* enable error output for all tasks */
  bstruct_setout(file,TRUE);
  if(fabs(cellsize_lon-config->resolution.lon)/config->resolution.lon>1e-3)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Cell size longitude %g different from %g in %s file '%s'.\n",
              cellsize_lon,config->resolution.lon,type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(fabs(cellsize_lat-config->resolution.lat)/config->resolution.lat>1e-3)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR154: Cell size latitude %g different from %g in %s file '%s'.\n",
              cellsize_lat,config->resolution.lat,type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(config->landuse_restart)
  {
    if(config->withlanduse==NO_LANDUSE)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR180: Land-use setting false is different from true in %s file '%s'.\n",type,filename);
      bstruct_finish(file);
      return NULL;
    }
    if(separate_harvests!=config->separate_harvests)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR180: Separate harvest setting %s is different from %s in %s file '%s'.\n",
                bool2str(config->separate_harvests),bool2str(separate_harvests),type,filename);
      bstruct_finish(file);
      return NULL;
    }
  }
  if(river_routing!=config->river_routing)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR181: River-routing setting %s is different from %s in %s file '%s'.\n",
              bool2str(config->river_routing),bool2str(river_routing),type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(isroot(*config) && config->sdate_option_restart==NO_FIXED_SDATE && config->sdate_option>NO_FIXED_SDATE && config->firstyear-config->nspinup>config->sdate_fixyear)
    fprintf(stderr,"ERROR245: Sowing dates are missing in restart file, sowing date fixed in year %d, but simulation starts in %d.\n",
            config->sdate_fixyear,config->firstyear-config->nspinup);
  if((sizeof(Real)==sizeof(float)  && datatype!=LPJ_FLOAT) ||
     (sizeof(Real)==sizeof(double) && datatype!=LPJ_DOUBLE))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR182: Real datatype does not match in %s file '%s'.\n",type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(restart_npft!=npft)
  {
    if(isroot(*config))

      fprintf(stderr,
              "ERROR183: Number of natural PFTs=%d does not match %d present in %s file '%s'.\n",
              npft,restart_npft,type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(restart_ncft!=ncft)
  {
    if(isroot(*config))

      fprintf(stderr,
              "ERROR183: Number of crop PFTs=%d does not match %d present in %s file '%s'.\n",
              ncft,restart_ncft,type,filename);
    bstruct_finish(file);
    return NULL;
  }
  if(config->ischeckpoint)
  {
    config->checkpointyear=firstyear;
    if(config->checkpointyear<config->firstyear-config->nspinup
       || config->checkpointyear>config->lastyear)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR233: Year %d in checkpoint file '%s' outside simulation years.\n",
                config->checkpointyear,filename);
      bstruct_finish(file);
      return NULL;
    }
  }
  else if(config->nspinup==0 && firstyear!=config->firstyear-1 &&
     isroot(*config))
    fprintf(stderr,
            "WARNING005: Year of restart file=%d not equal start year=%d-1.\n",
            firstyear,config->firstyear);

  if(config->firstgrid<firstcell)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR155: First grid cell %d not in %s file '%s', starts at cell %d.\n",
              config->startgrid,type,filename,firstcell);
    bstruct_finish(file);
    return NULL;
  }
  if(config->firstgrid>firstcell+ncell-1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR155: First grid cell %d not in %s file '%s', starts at cell %d.\n",
              config->startgrid,type,filename,firstcell);
    bstruct_finish(file);
    return NULL;
  }
  if(config->nall>ncell)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR155: %s file '%s' is too short, has only %d cells, %d needed.\n",
              type,filename,ncell,config->nall);
    bstruct_finish(file);
    return NULL;
  }
  if(config->firstgrid+config->nall>firstcell+ncell)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR155: %s file '%s' has cells in [%d,%d], must be [%d,%d].\n",
              type,filename,firstcell,firstcell+ncell-1,
              config->firstgrid,config->firstgrid+config->nall-1);
    bstruct_finish(file);
    return NULL;
  }
  offset=config->startgrid-firstcell;
  /* skip to cell */
  if(bstruct_seekindexarray(file,offset,ncell))
  {
    fprintf(stderr,"ERROR156: Cannot seek to index %d in %s file '%s'.\n",offset,type,filename);
    bstruct_finish(file);
    return NULL;
  }
  return file;
} /* of 'openrestart' */
