/**************************************************************************************/
/**                                                                                \n**/
/**                           l  a  n  d  u  s  e  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes landuse datatype                                      \n**/
/**     - opens the landuse input file (see also cfts26_lu2clm.c)                  \n**/
/**     - sets the landuse variables (see also landuse.h)                          \n**/
/**     - order of landuse input data:                                             \n**/
/**        0-10   CFT                                                              \n**/
/**          11   OTHERS                                                           \n**/
/**          12   PASTURES                                                         \n**/
/**          13   BIOMASS_GRASS                                                    \n**/
/**          14   BIOMASS_TREE                                                     \n**/
/**       15-25   CFT_irr                                                          \n**/
/**          26   others_irr                                                       \n**/
/**          27   PASTURE_irr                                                      \n**/
/**          28   BIOMASS_GRASS IRR                                                \n**/
/**          29   BIOMASS_TREE IRR                                                 \n**/
/**     - called in iterate()                                                      \n**/
/**     - reads every year the fractions of the bands for all cells from           \n**/
/**       the input file                                                           \n**/
/**     - checks if sum of fraction is not greater 1                               \n**/
/**       -> if sum of fraction is greater 1: subtraction from fraction            \n**/
/**          of managed grass if possible                                          \n**/
/**       -> else fail incorrect input file                                        \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/* define a tiny fraction for allcrops that is always at least 10x epsilon */

Real tinyfrac=max(epsilon*10,1e-6);

struct landuse
{
  Bool intercrop;               /**< intercropping possible (TRUE/FALSE) */
  Climatefile landuse;          /**< file pointer */
  Climatefile fertilizer_nr;    /**< file pointer to nitrogen fertilizer file */
  Climatefile manure_nr;        /**< file pointer to manure fertilizer file */
  Climatefile with_tillage;     /**< file pointer to tillage file */
  Climatefile residue_on_field; /**< file pointer to residue extraction file */
  Climatefile sdate;            /**< file pointer to prescribed sdates */
  Climatefile crop_phu;         /**< file pointer to prescribed crop phus */
};                              /**< definition of opaque datatype Landuse */

static void checkyear(const char *name,const Climatefile *file,const Config *config)
{
  int lastyear;
  if(isroot(*config))
  {
    if(config->withlanduse==LANDUSE)
    {
      lastyear=(config->fix_landuse) ? config->fix_climate_year : config->lastyear;
      if(file->firstyear+file->nyear-1<lastyear)
        fprintf(stderr,"WARNING024: Last year in %s data file=%d is less than last simulation year %d, data from last year used.\n",
                name,file->firstyear+file->nyear-1,lastyear);
    }
    else if(file->firstyear+file->nyear-1<config->landuse_year_const)
      fprintf(stderr,"WARNING024: Last year in %s data file=%d is less than const. landuse year %d, data from last year used.\n",
              name,file->firstyear+file->nyear-1,config->landuse_year_const);

  }
} /* of 'checkyear' */

Landuse initlanduse(const Config *config /**< LPJ configuration */
                   )                     /** \return allocated landuse or NULL */
{
  Header header;
  String headername;
  int version;
  Landuse landuse;
  size_t offset,filesize;
  landuse=new(struct landuse);
  if(landuse==NULL)
  {
    printallocerr("landuse");
    return NULL;
  }
  landuse->landuse.fmt=config->landuse_filename.fmt;
  if(config->landuse_filename.fmt==CDF)
  {
    if (opendata_netcdf(&landuse->landuse, &config->landuse_filename, "1", config))
    {
      free(landuse);
      return NULL;
    }
  }
  else
  {
    if((landuse->landuse.file=openinputfile(&header, &landuse->landuse.swap,
                                            &config->landuse_filename,
                                            headername,
                                            &version, &offset,TRUE, config)) == NULL)
    {
      free(landuse);
      return NULL;
    }
    if(config->landuse_filename.fmt==RAW)
    {
      header.nbands=2*config->landusemap_size;
      landuse->landuse.datatype=LPJ_SHORT;
      landuse->landuse.offset=config->startgrid*header.nbands*sizeof(short);
    }
    else
    {
      landuse->landuse.datatype=header.datatype;
      landuse->landuse.offset=(config->startgrid-header.firstcell)*header.nbands*typesizes[header.datatype]+headersize(headername,version)+offset;
      if(isroot(*config) && config->landuse_filename.fmt!=META)
      {
        filesize=getfilesizep(landuse->landuse.file)-headersize(headername,version)-offset;
        if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
          fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->landuse_filename.name);
      }
    }
    landuse->landuse.firstyear=header.firstyear;
    landuse->landuse.nyear=header.nyear;
    landuse->landuse.size=header.ncell*header.nbands*typesizes[landuse->landuse.datatype];
    landuse->landuse.n=config->ngridcell*header.nbands;
    landuse->landuse.var_len=header.nbands;
    landuse->landuse.scalar=(version==1) ? 0.001 : header.scalar;
    if(header.nstep!=1)
    {
      closeclimatefile(&landuse->landuse,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Invalid number of steps=%d in landuse data file, must be 1.\n",
                header.nstep);
      free(landuse);
      return NULL;
    }
    if(header.timestep!=1)
    {
      closeclimatefile(&landuse->landuse,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,"ERROR147: Invalid time step=%d in landuse data file, must be 1.\n",
                header.timestep);
      free(landuse);
      return NULL;
    }
  }
  if(landuse->landuse.var_len!=2*config->landusemap_size && landuse->landuse.var_len!=4*config->landusemap_size)
  {
    closeclimatefile(&landuse->landuse,isroot(*config));
    if(isroot(*config))
      fprintf(stderr,
              "ERROR147: Invalid number of bands=%zu in landuse data file,must be %d or %d.\n",
              landuse->landuse.var_len,2*config->landusemap_size,4*config->landusemap_size);
    free(landuse);
    return NULL;
  }
  if(isroot(*config) && landuse->landuse.var_len!=4*config->landusemap_size)
    fputs("WARNING024: Land-use input does not include irrigation systems, suboptimal country values are used.\n",stderr);
  checkyear("land-use",&landuse->landuse,config);
  if(config->sdate_option==PRESCRIBED_SDATE)
  {
    /* read sdate input metadata */
    landuse->sdate.fmt=config->sdate_filename.fmt;
    if(config->sdate_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->sdate,&config->sdate_filename,NULL,config))
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    else
    {
      if((landuse->sdate.file=openinputfile(&header,&landuse->sdate.swap,
                                            &config->sdate_filename,headername,
                                            &version,&offset,TRUE,config))==NULL)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(config->sdate_filename.fmt==RAW)
      {
        landuse->sdate.var_len=2*config->cftmap_size;
        landuse->sdate.datatype=LPJ_SHORT;
        landuse->sdate.offset=config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        landuse->sdate.var_len=header.nbands;
        landuse->sdate.datatype=header.datatype;
        landuse->sdate.offset=((long long)config->startgrid-(long long)header.firstcell)*header.nbands*typesizes[landuse->sdate.datatype]+headersize(headername,version)+offset;
        if(isroot(*config) && config->sdate_filename.fmt!=META)
        {
          filesize=getfilesizep(landuse->sdate.file)-headersize(headername,version)-offset;
          if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
            fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->sdate_filename.name);
        }
      }
      landuse->sdate.firstyear=header.firstyear;
      landuse->sdate.nyear=header.nyear;
      landuse->sdate.size=(long long)header.ncell*(long long)header.nbands*typesizes[landuse->sdate.datatype];
      landuse->sdate.n=config->ngridcell*header.nbands;
      landuse->sdate.scalar=header.scalar;
      if(header.nstep!=1)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        closeclimatefile(&landuse->sdate,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid number of steps=%d in sowing date file, must be 1.\n",
                  header.nstep);
        free(landuse);
        return NULL;
      }
      if(header.timestep!=1)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        closeclimatefile(&landuse->sdate,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid time step=%d in sowing date file, must be 1.\n",
                  header.timestep);
        free(landuse);
        return NULL;
      }
    }
    if(landuse->sdate.var_len!=2*config->cftmap_size)
    {
      closeclimatefile(&landuse->landuse,isroot(*config));
      closeclimatefile(&landuse->sdate,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,
                "ERROR147: Invalid number of bands=%zu in sowing date file, must be %d.\n",
                landuse->sdate.var_len,2*config->cftmap_size);
      free(landuse);
      return(NULL);
    }
  }
  else
  {
    landuse->sdate.file=NULL;
  } /* End sdate */

    /* Multiple-years PRESCRIBED_CROP_PHU */
  if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
  {
    /* read sdate input metadata */
    landuse->crop_phu.fmt=config->crop_phu_filename.fmt;
    if(config->crop_phu_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->crop_phu,&config->crop_phu_filename,NULL,config))
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    else
    {
      if((landuse->crop_phu.file=openinputfile(&header,&landuse->crop_phu.swap,
                                               &config->crop_phu_filename,headername,
                                               &version,&offset,TRUE,config))==NULL)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(config->crop_phu_filename.fmt==RAW)
      {
        header.nbands=2*config->cftmap_size;
        header.datatype=LPJ_SHORT;
        landuse->crop_phu.offset=(long long)config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        landuse->crop_phu.offset=((long long)config->startgrid-(long long)header.firstcell)*header.nbands*typesizes[header.datatype]+headersize(headername,version)+offset;
        if(isroot(*config) && config->crop_phu_filename.fmt!=META)
        {
          filesize=getfilesizep(landuse->crop_phu.file)-headersize(headername,version)-offset;
          if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
            fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->crop_phu_filename.name);
        }
      }
      landuse->crop_phu.datatype=header.datatype;
      landuse->crop_phu.firstyear=header.firstyear;
      landuse->crop_phu.nyear=header.nyear;
      landuse->crop_phu.size=(long long)header.ncell*(long long)header.nbands*typesizes[landuse->crop_phu.datatype];
      landuse->crop_phu.n=config->ngridcell*header.nbands;
      landuse->crop_phu.var_len=header.nbands;
      landuse->crop_phu.scalar=header.scalar;
      if(header.nstep!=1)
      {
        closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid number of steps=%d in crop phu data file, must be 1.\n",
                  header.nstep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(header.timestep!=1)
      {
        closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid time step=%d in crop phu data file, must be 1.\n",
                  header.timestep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    if(landuse->crop_phu.var_len!=2*config->cftmap_size)
    {
      closeclimatefile(&landuse->crop_phu,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,
                "ERROR147: Invalid number of bands=%zu in crop phu data file, must be %d.\n",
                landuse->crop_phu.var_len,2*config->cftmap_size);
      closeclimatefile(&landuse->landuse,isroot(*config));
      if(config->sdate_option==PRESCRIBED_SDATE)
        closeclimatefile(&landuse->sdate,isroot(*config));
      free(landuse);
      return NULL;
    }
    checkyear("crop phu",&landuse->crop_phu,config);
  } /* End crop_phu */

  if(config->fertilizer_input==FERTILIZER)
  {
    /* read fertilizer data */
    landuse->fertilizer_nr.fmt=config->fertilizer_nr_filename.fmt;
    if(config->fertilizer_nr_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->fertilizer_nr,&config->fertilizer_nr_filename,"g/m2",config))
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    else
    {
      if((landuse->fertilizer_nr.file=openinputfile(&header,&landuse->fertilizer_nr.swap,
                                                    &config->fertilizer_nr_filename,headername,
                                                    &version,&offset,TRUE,config))==NULL)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(config->fertilizer_nr_filename.fmt==RAW)
      {
        header.nbands=2*config->fertilizermap_size;
        header.datatype=LPJ_SHORT;
        landuse->fertilizer_nr.offset=config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        landuse->fertilizer_nr.offset=(config->startgrid-header.firstcell)*header.nbands*typesizes[header.datatype]+headersize(headername,version);
        if(isroot(*config) && config->fertilizer_nr_filename.fmt!=META)
        {
          filesize=getfilesizep(landuse->fertilizer_nr.file)-headersize(headername,version)-offset;
          if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
            fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->fertilizer_nr_filename.name);
        }
      }
      landuse->fertilizer_nr.datatype=header.datatype;
      landuse->fertilizer_nr.firstyear=header.firstyear;
      landuse->fertilizer_nr.nyear=header.nyear;
      landuse->fertilizer_nr.size=header.ncell*header.nbands*typesizes[header.datatype];
      landuse->fertilizer_nr.n=config->ngridcell*header.nbands;
      landuse->fertilizer_nr.var_len=header.nbands;
      landuse->fertilizer_nr.scalar=header.scalar;
      if(header.nstep!=1)
      {
        closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid number of steps=%d in fertilizer data file, must be 1.\n",
                  header.nstep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(header.timestep!=1)
      {
        closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid time step=%d in fertilizer data file, must be 1.\n",
                  header.timestep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    if(landuse->fertilizer_nr.var_len!=2*config->fertilizermap_size)
    {
      closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,
                "ERROR147: Invalid number of bands=%zu in fertilizer data file, must be %d.\n",
                landuse->fertilizer_nr.var_len,2*config->fertilizermap_size);
      closeclimatefile(&landuse->landuse,isroot(*config));
      if(config->sdate_option==PRESCRIBED_SDATE)
        closeclimatefile(&landuse->sdate,isroot(*config));
      if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
        closeclimatefile(&landuse->crop_phu,isroot(*config));
      free(landuse);
      return(NULL);
    }
    checkyear("fertilizer",&landuse->fertilizer_nr,config);
  }

  if(config->manure_input)
  {
    /* read manure fertilizer data */
    landuse->manure_nr.fmt=config->manure_nr_filename.fmt;
    if(config->manure_nr_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->manure_nr,&config->manure_nr_filename,NULL,config))
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    else
    {
      if((landuse->manure_nr.file=openinputfile(&header,&landuse->manure_nr.swap,
                                                &config->manure_nr_filename,headername,
                                                &version,&offset,TRUE,config))==NULL)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(config->manure_nr_filename.fmt==RAW)
      {
        header.nbands=2*config->fertilizermap_size;
        header.datatype=LPJ_SHORT;
        landuse->manure_nr.offset=config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        landuse->manure_nr.offset=(config->startgrid-header.firstcell)*header.nbands*typesizes[header.datatype]+headersize(headername,version);
        if(isroot(*config) && config->manure_nr_filename.fmt!=META)
        {
          filesize=getfilesizep(landuse->manure_nr.file)-headersize(headername,version)-offset;
          if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
            fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->manure_nr_filename.name);
        }
      }
      landuse->manure_nr.datatype=header.datatype;
      landuse->manure_nr.firstyear=header.firstyear;
      landuse->manure_nr.nyear=header.nyear;
      landuse->manure_nr.size=header.ncell*header.nbands*typesizes[header.datatype];
      landuse->manure_nr.n=config->ngridcell*header.nbands;
      landuse->manure_nr.var_len=header.nbands;
      landuse->manure_nr.scalar=header.scalar;
      if(header.nstep!=1)
      {
        closeclimatefile(&landuse->manure_nr,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid number of steps=%d in manure data file, must be 1.\n",
                  header.nstep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(header.timestep!=1)
      {
        closeclimatefile(&landuse->manure_nr,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid time step=%d in manure data file, must be 1.\n",
                  header.timestep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        free(landuse);
      }
    }
    if(landuse->manure_nr.var_len!=2*config->fertilizermap_size)
    {
      closeclimatefile(&landuse->manure_nr,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,
                "ERROR147: Invalid number of bands=%zu in manure data file. must be %d.\n",
                landuse->manure_nr.var_len,2*config->fertilizermap_size);
      closeclimatefile(&landuse->landuse,isroot(*config));
      if(config->sdate_option==PRESCRIBED_SDATE)
        closeclimatefile(&landuse->sdate,isroot(*config));
      if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
         closeclimatefile(&landuse->crop_phu,isroot(*config));
      if(config->fertilizer_input==FERTILIZER)
        closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
      free(landuse);
      return NULL;
    }
    checkyear("manure",&landuse->manure_nr,config);
  }

  if(config->tillage_type==READ_TILLAGE)
  {
    landuse->with_tillage.fmt=config->with_tillage_filename.fmt;
    if(config->with_tillage_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->with_tillage,&config->with_tillage_filename,NULL,config))
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    else
    {
      if((landuse->with_tillage.file=openinputfile(&header,&landuse->with_tillage.swap,
                                                   &config->with_tillage_filename,headername,
                                                   &version,&offset,TRUE,config))==NULL)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(config->with_tillage_filename.fmt==RAW)
      {
        header.nbands=1;
        header.datatype=LPJ_SHORT;
        landuse->with_tillage.offset=config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        landuse->with_tillage.offset=(config->startgrid-header.firstcell)*header.nbands*sizeof(short)+headersize(headername,version);
        if(isroot(*config) && config->with_tillage_filename.fmt!=META)
        {
          filesize=getfilesizep(landuse->with_tillage.file)-headersize(headername,version)-offset;
          if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
            fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->with_tillage_filename.name);
        }
      }
      landuse->with_tillage.datatype=header.datatype;
      landuse->with_tillage.firstyear=header.firstyear;
      landuse->with_tillage.nyear=header.nyear;
      landuse->with_tillage.size=header.ncell*header.nbands*sizeof(short);
      landuse->with_tillage.n=config->ngridcell*header.nbands;
      landuse->with_tillage.var_len=header.nbands;
      landuse->with_tillage.scalar=header.scalar;
      if(header.nstep!=1)
      {
        closeclimatefile(&landuse->with_tillage,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid number of steps=%d in tillage type file, must be 1.\n",
                  header.nstep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(header.timestep!=1)
      {
        closeclimatefile(&landuse->with_tillage,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid time step=%d in tillage type file, must be 1.\n",
                  header.timestep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    if(landuse->with_tillage.var_len!=1)
    {
      closeclimatefile(&landuse->landuse,isroot(*config));
      if(config->sdate_option==PRESCRIBED_SDATE)
        closeclimatefile(&landuse->sdate,isroot(*config));
      if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
        closeclimatefile(&landuse->crop_phu,isroot(*config));
      if(config->fertilizer_input==FERTILIZER)
        closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
      if(config->manure_input)
        closeclimatefile(&landuse->manure_nr,isroot(*config));
      closeclimatefile(&landuse->with_tillage,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,
                "ERROR147: Invalid number of bands=%zu in tillage type file, must be 1.\n",
                landuse->with_tillage.var_len);
      free(landuse);
      return(NULL);
    }
    checkyear("tillage",&landuse->with_tillage,config);
  }

  if(config->residue_treatment==READ_RESIDUE_DATA)
  {
    /* read residue data */
    landuse->residue_on_field.fmt=config->residue_data_filename.fmt;
    if(config->residue_data_filename.fmt==CDF)
    {
      if(opendata_netcdf(&landuse->residue_on_field,&config->residue_data_filename,NULL,config))
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        if(config->tillage_type==READ_TILLAGE)
          closeclimatefile(&landuse->with_tillage,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    else
    {
      if((landuse->residue_on_field.file=openinputfile(&header,&landuse->residue_on_field.swap,
                                                       &config->residue_data_filename,headername,
                                                       &version,&offset,TRUE,config))==NULL)
      {
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        if(config->tillage_type==READ_TILLAGE)
          closeclimatefile(&landuse->with_tillage,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(config->residue_data_filename.fmt==RAW)
      {
        header.nbands=config->fertilizermap_size;
        header.datatype=LPJ_SHORT;
        landuse->residue_on_field.offset=config->startgrid*header.nbands*sizeof(short);
      }
      else
      {
        landuse->residue_on_field.offset=(config->startgrid-header.firstcell)*header.nbands*typesizes[header.datatype]+headersize(headername,version);
        if(isroot(*config) && config->residue_data_filename.fmt!=META)
        {
          filesize=getfilesizep(landuse->residue_on_field.file)-headersize(headername,version)-offset;
          if(filesize!=typesizes[header.datatype]*header.nyear*header.nbands*header.ncell)
            fprintf(stderr,"WARNING032: File size of '%s' does not match nyear*ncell*nbands.\n",config->residue_data_filename.name);
        }
      }
      landuse->residue_on_field.datatype=header.datatype;
      landuse->residue_on_field.firstyear=header.firstyear;
      landuse->residue_on_field.nyear=header.nyear;
      landuse->residue_on_field.size=header.ncell*header.nbands*typesizes[header.datatype];
      landuse->residue_on_field.n=config->ngridcell*header.nbands;
      landuse->residue_on_field.var_len=header.nbands;
      landuse->residue_on_field.scalar=header.scalar;
      if(header.nstep!=1)
      {
        closeclimatefile(&landuse->residue_on_field,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid number of steps=%d in residue extraction data file, must be 1.\n",
                  header.nstep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        if(config->tillage_type==READ_TILLAGE)
          closeclimatefile(&landuse->with_tillage,isroot(*config));
        free(landuse);
        return NULL;
      }
      if(header.timestep!=1)
      {
        closeclimatefile(&landuse->residue_on_field,isroot(*config));
        if(isroot(*config))
          fprintf(stderr,"ERROR147: Invalid time step=%d in residue extraction data file, must be 1.\n",
                  header.timestep);
        closeclimatefile(&landuse->landuse,isroot(*config));
        if(config->sdate_option==PRESCRIBED_SDATE)
          closeclimatefile(&landuse->sdate,isroot(*config));
        if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
          closeclimatefile(&landuse->crop_phu,isroot(*config));
        if(config->fertilizer_input==FERTILIZER)
          closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
        if(config->manure_input)
          closeclimatefile(&landuse->manure_nr,isroot(*config));
        if(config->tillage_type==READ_TILLAGE)
          closeclimatefile(&landuse->with_tillage,isroot(*config));
        free(landuse);
        return NULL;
      }
    }
    if(landuse->residue_on_field.var_len!=config->fertilizermap_size)
    {
      closeclimatefile(&landuse->residue_on_field,isroot(*config));
      if(isroot(*config))
        fprintf(stderr,
                "ERROR147: Invalid number of bands=%zu in residue extraction data file, must be %d.\n",
                landuse->residue_on_field.var_len,config->fertilizermap_size);
      closeclimatefile(&landuse->landuse,isroot(*config));
      if(config->sdate_option==PRESCRIBED_SDATE)
        closeclimatefile(&landuse->sdate,isroot(*config));
      if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
        closeclimatefile(&landuse->crop_phu,isroot(*config));
      if(config->fertilizer_input==FERTILIZER)
        closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
      if(config->manure_input)
        closeclimatefile(&landuse->manure_nr,isroot(*config));
      if(config->tillage_type==READ_TILLAGE)
        closeclimatefile(&landuse->with_tillage,isroot(*config));
      free(landuse);
      return NULL;
    }
    checkyear("residue",&landuse->residue_on_field,config);
  }
  landuse->intercrop=config->intercrop;
  return landuse;
} /* of 'initlanduse' */


/**************************************************************************************/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads landuse data for a specific year                            \n**/
/**                                                                                \n**/
/**     - order of landuse input data:                                             \n**/
/**        0-10   CFT                                                              \n**/
/**          11   OTHERS                                                           \n**/
/**          12   PASTURES                                                         \n**/
/**          13   BIOMASS_GRASS                                                    \n**/
/**          14   BIOMASS_TREE                                                     \n**/
/**       15-25   CFT_irr                                                          \n**/
/**          26   others_irr                                                       \n**/
/**          27   PASTURE_irr                                                      \n**/
/**          28   BIOMASS_GRASS IRR                                                \n**/
/**          29   BIOMASS_TREE IRR                                                 \n**/
/**     - called in iterate()                                                      \n**/
/**     - reads every year the fractions of the bands for all cells from           \n**/
/**       the input file                                                           \n**/
/**     - checks if sum of fraction is not greater 1                               \n**/
/**       -> if sum of fraction is greater 1: subtraction from fraction            \n**/
/**          of managed grass if possible                                          \n**/
/**       -> else fail incorrect input file                                        \n**/
/**                                                                                \n**/
/**************************************************************************************/

static Real reducelanduse(Cell *cell,Real sum,int ncft,int nagtree)
{
  int i,j;
  if(cell->ml.landfrac[0].grass[1]>sum)
  {
    cell->ml.landfrac[0].grass[1]-=sum;
    return 0.0;
  }
  if(cell->ml.landfrac[1].grass[1]>sum)
  {
    cell->ml.landfrac[1].grass[1]-=sum;
    return 0.0;
  }
  for(j=0; j<2; j++)
  {
    for(i=0; i<ncft; i++)
      if(cell->ml.landfrac[j].crop[i]>sum)
      {
        cell->ml.landfrac[j].crop[i]-=sum;
        return 0;
      }
    for(i=0; i<nagtree; i++)
      if(cell->ml.landfrac[j].ag_tree[i]>sum)
      {
        cell->ml.landfrac[j].ag_tree[i]-=sum;
        return 0;
      }
    for(i=0; i<NGRASS; i++)
      if(cell->ml.landfrac[j].grass[i]>sum)
      {
        cell->ml.landfrac[j].grass[i]-=sum;
        return 0;
      }
    if(cell->ml.landfrac[j].biomass_tree>sum)
    {
      cell->ml.landfrac[j].biomass_tree-=sum;
      return 0;
    }
    if(cell->ml.landfrac[j].biomass_grass>sum)
    {
      cell->ml.landfrac[j].biomass_grass-=sum;
      return 0;
    }
    if(cell->ml.landfrac[j].woodplantation>sum)
    {
      cell->ml.landfrac[j].woodplantation-=sum;
      return 0;
    }
  }
  return sum;
} /* of 'reducelanduse' */

Bool getlanduse(Landuse landuse,     /**< Pointer to landuse data */
                Cell grid[],         /**< LPJ cell array */
                int year,            /**< year (AD) */
                int actual_year,     /**< year (AD) but not the static in case of CONST_LANDUSE */
                int ncft,            /**< number of crop PFTs */
                const Config *config /**< LPJ configuration */
               )                     /** \return TRUE on error */
{
  int i,j,count,cell;
  IrrigationType p;
  Real sum,*data;
  int *dates;
  int yearsdate=actual_year;     /*sdate year*/
  int yearphu=actual_year;       /*crop phu year*/
  int yearf=year;
  int yearm=year;
  int yeart=year;
  int yearr=year;
  /* for testing soil type to avoid all crops on ROCK and ICE cells */
  Stand *stand;
  int soiltype=-1;
  int yearl=year;
  String line;

  /* Initialize yearly prescribed sdate */
  if(config->sdate_option==PRESCRIBED_SDATE)
  {
    yearsdate-=landuse->sdate.firstyear;
    if(yearsdate>=landuse->sdate.nyear)
      yearsdate=landuse->sdate.nyear-1; /* use last year sdate */
    else if(yearsdate<0)
      yearsdate=0;                        /* use first year sdate */

    dates=newvec(int,config->ngridcell*landuse->sdate.var_len);
    if(dates==NULL)
    {
      printallocerr("dates");
      return TRUE;
    }
    if(landuse->sdate.fmt==CDF)
    {
      if(readintdata_netcdf(&landuse->sdate,dates,grid,yearsdate,config))
      {
        fprintf(stderr,
                "ERROR149: Cannot read sowing dates of year %d in getlanduse().\n",
                yearsdate+landuse->sdate.firstyear);
        fflush(stderr);
        free(dates);
        return TRUE;
      }
    }
    else
    {
      if(fseek(landuse->sdate.file,(long long)yearsdate*landuse->sdate.size+landuse->sdate.offset,SEEK_SET))
      {
        fprintf(stderr,
                "ERROR148: Cannot seek sowing dates to year %d in getlanduse().\n",
                yearsdate+landuse->sdate.firstyear);
        free(dates);
        return TRUE;
      }
      if(readintvec(landuse->sdate.file,dates,landuse->sdate.n,landuse->sdate.swap,landuse->sdate.datatype))
      {
        fprintf(stderr,
                "ERROR149: Cannot read sowing dates of year %d in getlanduse().\n",
                yearsdate+landuse->sdate.firstyear);
        free(dates);
        return TRUE;
      }
    }
    count=0;
    for(cell=0;cell<config->ngridcell;cell++)
      if(!grid[cell].skip)
      {
        for(j=0;j<2*ncft;j++)
          grid[cell].ml.sdate_fixed[j]=0;
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.sdate_fixed[config->cftmap[j]]=dates[count++];
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.sdate_fixed[config->cftmap[j]+ncft]=dates[count++];
      }
      else
        count+=2*ncft;
    free(dates);
  }
  if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
  {
    /* assigning crop phus data */
    yearphu-=landuse->crop_phu.firstyear;
    if(yearphu>=landuse->crop_phu.nyear)
      yearphu=landuse->crop_phu.nyear-1; /* use last year sdate */
    else if(yearphu<0)
      yearphu=0;                        /* use first year sdate */

    data=newvec(Real,config->ngridcell*landuse->crop_phu.var_len);
    if(data==NULL)
    {
      printallocerr("data");
      return TRUE;
     }
    if(landuse->crop_phu.fmt==CDF)
    {
      if(readdata_netcdf(&landuse->crop_phu,data,grid,yearphu,config))
      {
        fprintf(stderr,
                "ERROR149: Cannot read crop phus of year %d in getlanduse().\n",
                yearphu+landuse->crop_phu.firstyear);
        free(data);
        fflush(stderr);
        return TRUE;
      }
    }
    else
    {
      if(fseek(landuse->crop_phu.file,(long long)yearphu*landuse->crop_phu.size+landuse->crop_phu.offset,SEEK_SET))
      {
        fprintf(stderr,
                "ERROR148: Cannot seek crop phus to year %d in getlanduse().\n",
                yearphu+landuse->crop_phu.firstyear);
        free(data);
        return TRUE;
      }
      if(readrealvec(landuse->crop_phu.file,data,0,landuse->crop_phu.scalar,landuse->crop_phu.n,landuse->crop_phu.swap,landuse->crop_phu.datatype))
      {
        fprintf(stderr,
                "ERROR149: Cannot read crop phus of year %d in getlanduse().\n",
                yearphu+landuse->crop_phu.firstyear);
        free(data);
        return TRUE;
      }
    }
    count=0;
    for(cell=0; cell<config->ngridcell; cell++)
      if(!grid[cell].skip)
      {
        for(j=0; j<2*ncft; j++)
          grid[cell].ml.crop_phu_fixed[j]=0;
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.crop_phu_fixed[config->cftmap[j]]=data[count++];
        for(j=0;j<config->cftmap_size;j++)
          if(config->cftmap[j]==NOT_FOUND)
            count++; /* ignore data */
          else
            grid[cell].ml.crop_phu_fixed[config->cftmap[j]+ncft]=data[count++];
      }
      else
        count+=2*ncft;
    free(data);
  } /* end crop_phu*/

  yearl-=landuse->landuse.firstyear;
  if(yearl>=landuse->landuse.nyear)
    yearl=landuse->landuse.nyear-1;
  else if(yearl<0)
    yearl=0;
  data=newvec(Real,config->ngridcell*landuse->landuse.var_len);
  if(data==NULL)
  {
    printallocerr("data");
    return TRUE;
  }
  if(landuse->landuse.fmt==CDF)
  {
    if(readdata_netcdf(&landuse->landuse,data,grid,yearl,config))
    {
      fprintf(stderr,
              "ERROR149: Cannot read landuse of year %d in getlanduse().\n",
              yearl+landuse->landuse.firstyear);
      fflush(stderr);
      free(data);
      return TRUE;
    }
  }
  else
  {
    if(fseek(landuse->landuse.file,(long long)yearl*landuse->landuse.size+landuse->landuse.offset,SEEK_SET))
    {
      fprintf(stderr,
              "ERROR148: Cannot seek landuse to year %d in getlanduse().\n",
              yearl + landuse->landuse.firstyear);
      fflush(stderr);
      free(data);
      return TRUE;
    }
    if(readrealvec(landuse->landuse.file,data,0,landuse->landuse.scalar,landuse->landuse.n,landuse->landuse.swap,landuse->landuse.datatype))
    {
      fprintf(stderr,
              "ERROR149: Cannot read landuse of year %d in getlanduse().\n",
              yearl + landuse->landuse.firstyear);
      fflush(stderr);
      free(data);
      return TRUE;
    }
  }
  count=0;

  for(cell=0;cell<config->ngridcell;cell++)
  {
    /* get soiltype of first stand (not stored in cell structure) */
    if(!grid[cell].skip && grid[cell].standlist->n>0)
    {
      stand=getstand(grid[cell].standlist,0);
      soiltype=stand->soil.par->type;
    }
    else
      soiltype=-1;
    for(i=0;i<WIRRIG;i++)
    {
      initlandfracitem(grid[cell].ml.landfrac+i,ncft,config->nagtree);
      /* read cropfrac from 32 bands or rain-fed cropfrac from 64 bands input */
      if(landuse->landuse.var_len!=4*config->landusemap_size || i<1)
      {
        if(i>0 && !grid[cell].skip)
        {
          for(j=0;j<ncft;j++)
            grid[cell].ml.irrig_system->crop[j]=grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
          for(j=0;j<NGRASS;j++)
            grid[cell].ml.irrig_system->grass[j]=grid[cell].ml.manage.par->default_irrig_system;
          for(j=0;j<config->nagtree;j++)
            grid[cell].ml.irrig_system->ag_tree[j]=grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.irrig_system->woodplantation = grid[cell].ml.manage.par->default_irrig_system;
        }
        if(readlandfracmap(grid[cell].ml.landfrac+i,config->landusemap,
                        config->landusemap_size,data,&count,ncft,config->nwptype))
        {
          fprintf(stderr,"ERROR149: Land-use input=%g less than zero for cell %d (%s) in year %d.\n",
                  data[count],cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearl+landuse->landuse.firstyear);
          return TRUE;
        }
      }
      else /* read irrigated cropfrac and irrigation systems from 64 bands input */
      {
        for(p=SURF;p<=DRIP;p++) /* irrigation system loop; 1: surface, 2: sprinkler, 3: drip */
        {
          for(j=0;j<config->landusemap_size;j++)
          {
            if(data[count]>0)
            {
              if(config->landusemap[j]==NOT_FOUND)
                count++; /* ignore data */
              else if(config->landusemap[j]<ncft)
              {
                grid[cell].ml.landfrac[i].crop[config->landusemap[j]]+=data[count++];
                grid[cell].ml.irrig_system->crop[config->landusemap[j]]=p;
              }
              else if(config->landusemap[j]<ncft+NGRASS)
              {
                grid[cell].ml.landfrac[i].grass[config->landusemap[j]-ncft]+=data[count++];
                grid[cell].ml.irrig_system->grass[config->landusemap[j]-ncft]=p;
              }
              else if(config->landusemap[j]==ncft+NGRASS)
              {
                grid[cell].ml.landfrac[i].biomass_grass+=data[count++];
                grid[cell].ml.irrig_system->biomass_grass=p;
              }
              else if(config->landusemap[j]==ncft+NGRASS+1)
              {
                grid[cell].ml.landfrac[i].biomass_tree+=data[count++];
                grid[cell].ml.irrig_system->biomass_tree=p;
              }
              else if(config->nwptype && config->landusemap[j]==ncft+NGRASS+NBIOMASSTYPE)
              {
                grid[cell].ml.landfrac[i].woodplantation+=data[count++];
                grid[cell].ml.irrig_system->woodplantation=p;
              }
              else
              {
                grid[cell].ml.landfrac[i].ag_tree[config->landusemap[j]-ncft-NGRASS-NBIOMASSTYPE-config->nwptype]+=data[count++];
                grid[cell].ml.irrig_system[i].ag_tree[config->landusemap[j]-ncft-NGRASS-NBIOMASSTYPE-config->nwptype]=p;
              }
            }
            else
              count++;
          }
        } /* of for(p=SURF;p<=DRIP;p++) */
      }
    } /* of  for(i=0;i<WIRRIG;i++) */
    switch (config->irrig_scenario)
    {
      case NO_IRRIGATION:
        for (j = 0; j < ncft; j++)
        {
          grid[cell].ml.landfrac[0].crop[j] += grid[cell].ml.landfrac[1].crop[j];
          grid[cell].ml.landfrac[1].crop[j] = 0;
          grid[cell].ml.irrig_system->crop[j] = NOIRRIG;
        }
        for (j = 0; j < NGRASS; j++)
        {
          grid[cell].ml.landfrac[0].grass[j] += grid[cell].ml.landfrac[1].grass[j];
          grid[cell].ml.landfrac[1].grass[j] = 0;
          grid[cell].ml.irrig_system->grass[j] = NOIRRIG;
        }
        grid[cell].ml.landfrac[0].biomass_grass += grid[cell].ml.landfrac[1].biomass_grass;
        grid[cell].ml.landfrac[1].biomass_grass = 0;
        grid[cell].ml.irrig_system->biomass_grass = NOIRRIG;
        grid[cell].ml.landfrac[0].biomass_tree += grid[cell].ml.landfrac[1].biomass_tree;
        grid[cell].ml.landfrac[1].biomass_tree = 0;
        grid[cell].ml.irrig_system->biomass_tree = NOIRRIG;
        for (j = 0; j < config->nagtree; j++)
        {
          grid[cell].ml.landfrac[0].ag_tree[j] += grid[cell].ml.landfrac[1].ag_tree[j];
          grid[cell].ml.landfrac[1].ag_tree[j] = 0;
          grid[cell].ml.irrig_system->ag_tree[j] = NOIRRIG;
        }
        grid[cell].ml.landfrac[0].woodplantation += grid[cell].ml.landfrac[1].woodplantation;
        grid[cell].ml.landfrac[1].woodplantation=0;
        grid[cell].ml.irrig_system->woodplantation = NOIRRIG;
        break;
      case ALL_IRRIGATION:
        for (j = 0; j < ncft; j++)
        {
          grid[cell].ml.landfrac[1].crop[j] += grid[cell].ml.landfrac[0].crop[j];
          grid[cell].ml.landfrac[0].crop[j] = 0;
          if (!grid[cell].skip)
            grid[cell].ml.irrig_system->crop[j] = grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
        }
        for (j = 0; j < NGRASS; j++)
        {
          grid[cell].ml.landfrac[1].grass[j] += grid[cell].ml.landfrac[0].grass[j];
          grid[cell].ml.landfrac[0].grass[j] = 0;
          if (!grid[cell].skip)
            grid[cell].ml.irrig_system->grass[j] = grid[cell].ml.manage.par->default_irrig_system;
        }
        grid[cell].ml.landfrac[1].biomass_grass += grid[cell].ml.landfrac[0].biomass_grass;
        grid[cell].ml.landfrac[0].biomass_grass = 0;
        if (!grid[cell].skip)
          grid[cell].ml.irrig_system->biomass_grass = grid[cell].ml.manage.par->default_irrig_system;
        grid[cell].ml.landfrac[1].biomass_tree += grid[cell].ml.landfrac[0].biomass_tree;
        grid[cell].ml.landfrac[0].biomass_tree = 0;
        grid[cell].ml.irrig_system->biomass_tree = grid[cell].ml.manage.par->default_irrig_system;
        for (j = 0; j < config->nagtree; j++)
        {
          grid[cell].ml.landfrac[1].ag_tree[j] += grid[cell].ml.landfrac[0].ag_tree[j];
          grid[cell].ml.landfrac[0].ag_tree[j] = 0;
          if (!grid[cell].skip)
            grid[cell].ml.irrig_system->ag_tree[j] = grid[cell].ml.manage.par->default_irrig_system; /*default national irrigation system (Rohwer & Gerten 2007)*/
        }
        grid[cell].ml.landfrac[1].woodplantation += grid[cell].ml.landfrac[0].woodplantation;
        grid[cell].ml.landfrac[0].woodplantation = 0;
        if (!grid[cell].skip)
          grid[cell].ml.irrig_system->woodplantation = grid[cell].ml.manage.par->default_irrig_system;
        break;
    } /* of switch(...) */

    /* DEBUG: here you can set land-use fractions manually, it overwrites the land-use input, in all cells */
    /* the irrigation system is set to the default country value, but you can set a number from 1-3 manually */
    /* 1: surface, 2: sprinkler, 3: drip irrigation */

/*
      sum=landfrac_sum(grid[cell].ml.landfrac,ncft,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,TRUE);

      for(j=0;j<ncft;j++)
      {
        grid[cell].ml.landfrac[1].crop[j]=0.0;
        grid[cell].ml.landfrac[0].crop[j]=0.0;
        grid[cell].ml.irrig_system->crop[j]=grid[cell].ml.manage.par->default_irrig_system;
      }
      for(j=0;j<NGRASS;j++)
      {
        grid[cell].ml.landfrac[1].grass[j]=0.0;
        grid[cell].ml.landfrac[0].grass[j]=0.0;
        grid[cell].ml.irrig_system->grass[j]=grid[cell].ml.manage.par->default_irrig_system;
      }

      grid[cell].ml.landfrac[1].biomass_grass=0.0;
      grid[cell].ml.landfrac[0].biomass_grass=0.0;
      grid[cell].ml.irrig_system->biomass_grass=grid[cell].ml.manage.par->default_irrig_system;
      grid[cell].ml.landfrac[1].biomass_tree=0.0;
      grid[cell].ml.landfrac[0].biomass_tree=0.0;
      grid[cell].ml.irrig_system->biomass_tree=grid[cell].ml.manage.par->default_irrig_system;

      grid[cell].ml.landfrac[1].grass[1]=0.0;
      grid[cell].ml.landfrac[0].grass[0]=1.0;
      //if (sum>1.00001) grid[cell].ml.landfrac[0].grass[0]=1.0;
*/
/* END DEBUG */

    if(config->others_to_crop)
    {
      if(grid[cell].coord.lat>30||grid[cell].coord.lat<-30)
      {
        grid[cell].ml.landfrac[0].crop[config->cft_temp]+=grid[cell].ml.landfrac[0].grass[0];
        grid[cell].ml.landfrac[1].crop[config->cft_temp]+=grid[cell].ml.landfrac[1].grass[0];
        grid[cell].ml.landfrac[0].grass[0]=grid[cell].ml.landfrac[1].grass[0]=0;
      }
      else
      {
        grid[cell].ml.landfrac[0].crop[config->cft_tropic]+=grid[cell].ml.landfrac[0].grass[0];
        grid[cell].ml.landfrac[1].crop[config->cft_tropic]+=grid[cell].ml.landfrac[1].grass[0];
        grid[cell].ml.landfrac[0].grass[0]=grid[cell].ml.landfrac[1].grass[0]=0;
      }
    }
    if(config->grassonly)
    {
      for(j=0; j<ncft; j++)
        grid[cell].ml.landfrac[0].crop[j]=grid[cell].ml.landfrac[1].crop[j]=0;
      for(j=0; j<config->nagtree; j++)
        grid[cell].ml.landfrac[0].ag_tree[j]=grid[cell].ml.landfrac[1].ag_tree[j]=0;
      grid[cell].ml.landfrac[0].grass[0]=grid[cell].ml.landfrac[1].grass[0]=0;
      grid[cell].ml.landfrac[0].biomass_grass=grid[cell].ml.landfrac[1].biomass_grass=
      grid[cell].ml.landfrac[0].biomass_tree=grid[cell].ml.landfrac[1].biomass_tree=0;
      grid[cell].ml.landfrac[0].woodplantation=grid[cell].ml.landfrac[1].woodplantation=0;
    }
    
    /* force tinyfrac for all crops only on pixels with valid soil */
    if (config->withlanduse==ALL_CROPS && !grid[cell].skip && soiltype!=ROCK && soiltype!=ICE && soiltype >= 0)
    {
      for(j=0; j<ncft; j++)
      {
        if (grid[cell].ml.landfrac[1].crop[j] < tinyfrac) 
        {
          grid[cell].ml.irrig_system->crop[j] = grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.landfrac[1].crop[j] = tinyfrac;
        }
        if (grid[cell].ml.landfrac[0].crop[j] < tinyfrac) grid[cell].ml.landfrac[0].crop[j] = tinyfrac;
      }
      for(j=0; j<config->nagtree; j++)
      {
        if (grid[cell].ml.landfrac[1].ag_tree[j] < tinyfrac) 
        {
          grid[cell].ml.irrig_system->ag_tree[j] = grid[cell].ml.manage.par->default_irrig_system;
          grid[cell].ml.landfrac[1].ag_tree[j] = tinyfrac;
        }
        if (grid[cell].ml.landfrac[0].ag_tree[j] < tinyfrac) grid[cell].ml.landfrac[0].ag_tree[j] = tinyfrac;
      }
      for(j=0; j<NGRASS; j++)
      {
        if (grid[cell].ml.landfrac[0].grass[j] < tinyfrac) grid[cell].ml.landfrac[0].grass[j] = tinyfrac;
        if (grid[cell].ml.landfrac[1].grass[j] < tinyfrac) 
        {
          grid[cell].ml.landfrac[1].grass[j] = tinyfrac;
          grid[cell].ml.irrig_system->grass[j] = grid[cell].ml.manage.par->default_irrig_system;
        }
      }
      if (grid[cell].ml.landfrac[1].biomass_tree < tinyfrac) 
      {
        grid[cell].ml.landfrac[1].biomass_tree = tinyfrac;
        grid[cell].ml.irrig_system->biomass_tree = grid[cell].ml.manage.par->default_irrig_system;
      }
      if (grid[cell].ml.landfrac[0].biomass_tree < tinyfrac) grid[cell].ml.landfrac[0].biomass_tree = tinyfrac;
      if (grid[cell].ml.landfrac[1].biomass_grass < tinyfrac) 
      {
        grid[cell].ml.landfrac[1].biomass_grass = tinyfrac;
        grid[cell].ml.irrig_system->biomass_grass = grid[cell].ml.manage.par->default_irrig_system;
      }
      if (grid[cell].ml.landfrac[0].biomass_grass < tinyfrac) grid[cell].ml.landfrac[0].biomass_grass = tinyfrac;
      if(config->nwptype)
      {
        if (grid[cell].ml.landfrac[1].woodplantation < tinyfrac)
        {
          grid[cell].ml.landfrac[1].woodplantation = tinyfrac;
          grid[cell].ml.irrig_system->woodplantation = grid[cell].ml.manage.par->default_irrig_system;
        }
        if (grid[cell].ml.landfrac[0].woodplantation < tinyfrac) grid[cell].ml.landfrac[0].woodplantation = tinyfrac;
      }

    }
    sum=landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,TRUE);

    /* set landuse to zero if no valid soil */
    if ((grid[cell].skip || soiltype==ROCK || soiltype==ICE || soiltype < 0) && sum>0)
    {
      //fprintf(stderr,"WARNING!! setting LU (sum:%g) to zero, because of invalid soil type %d (%g/%g) in cell %d at year %d\n",
      //        sum,soiltype,grid[cell].coord.lon,grid[cell].coord.lat, cell+config->startgrid,yearl+landuse->landuse.firstyear);
      for(j=0; j<ncft; j++)
      {
        grid[cell].ml.landfrac[0].crop[j]=0;
        grid[cell].ml.landfrac[1].crop[j]=0;
      }
      for(j=0; j<config->nagtree; j++)
      {
        grid[cell].ml.landfrac[0].ag_tree[j]=0;
        grid[cell].ml.landfrac[1].ag_tree[j]=0;
      }
      for(j=0; j<NGRASS; j++)
      {
        grid[cell].ml.landfrac[0].grass[j]=0;
        grid[cell].ml.landfrac[1].grass[j]=0;
      }
      grid[cell].ml.landfrac[0].biomass_grass=0;
      grid[cell].ml.landfrac[1].biomass_grass=0;
      grid[cell].ml.landfrac[0].biomass_tree=0;
      grid[cell].ml.landfrac[1].biomass_tree=0;
      grid[cell].ml.landfrac[0].woodplantation=0;
      grid[cell].ml.landfrac[1].woodplantation=0;
    }
    /* recommpute sum after scaling fraction */
    sum=landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,FALSE)+landfrac_sum(grid[cell].ml.landfrac,ncft,config->nagtree,TRUE);

    if(sum>1.00001)
    {
      if(yearl>0&&sum>1.01)
      {
        fprintf(stderr,"WARNING013: in cell %d at year %d: sum of crop fractions greater 1: %f\n",
                cell+config->startgrid,yearl+landuse->landuse.firstyear,sum);
        fflush(stderr);
      }
      sum=reducelanduse(grid+cell,sum-1,ncft,config->nagtree);
      if(sum>0.00001)
        fail(CROP_FRACTION_ERR,FALSE,
             "crop fraction greater 1: %f cell: %d, managed grass is 0",
             sum+1,cell+config->startgrid);
    }
    if (config->withlanduse==ONLY_CROPS)
    {
      sum = 0;
      for (j = 0; j < ncft; j++)
      {
        sum += grid[cell].ml.landfrac[0].crop[j];
        sum += grid[cell].ml.landfrac[1].crop[j];
      }
      if (sum < 1 && sum > epsilon)
      {
        for (j = 0; j < ncft; j++)
        {
          grid[cell].ml.landfrac[0].crop[j] /= sum;
          grid[cell].ml.landfrac[1].crop[j] /= sum;
        }
        for (j = 0; j < NGRASS; j++)
        {
          grid[cell].ml.landfrac[0].grass[j] = 0;
          grid[cell].ml.landfrac[1].grass[j] = 0;
        }
        for (j = 0; j < config->nagtree; j++)
        {
          grid[cell].ml.landfrac[0].ag_tree[j] = 0;
          grid[cell].ml.landfrac[1].ag_tree[j] = 0;
        }
        grid[cell].ml.landfrac[0].biomass_grass = 0;
        grid[cell].ml.landfrac[1].biomass_grass = 0;
        grid[cell].ml.landfrac[0].biomass_tree = 0;
        grid[cell].ml.landfrac[1].biomass_tree = 0;
        grid[cell].ml.landfrac[0].woodplantation=0;
        grid[cell].ml.landfrac[1].woodplantation=0;
      }
    }
/** temporary set everything to irrigated maize */
/*        for(j=0; j<ncft; j++)
        {
          grid[cell].ml.landfrac[0].crop[j]=0;
          grid[cell].ml.landfrac[1].crop[j]=0;
        }
        for(j=0; j<NGRASS; j++)
        {
          grid[cell].ml.landfrac[0].grass[j]=0;
          grid[cell].ml.landfrac[1].grass[j]=0;
        }
        grid[cell].ml.landfrac[0].biomass_grass=0;
        grid[cell].ml.landfrac[1].biomass_grass=0;
        grid[cell].ml.landfrac[0].biomass_tree=0;
        grid[cell].ml.landfrac[1].biomass_tree=0;
      grid[cell].ml.landfrac[1].crop[0]=0.5;
      grid[cell].ml.landfrac[1].crop[2]=0.5;
      sum=1;*/
  } /* for(cell=0;...) */
  free(data);
  if(config->with_nitrogen)
  {
    for(cell=0; cell<config->ngridcell; cell++)
    {
      initlandfrac(grid[cell].ml.fertilizer_nr,ncft,config->nagtree);
      initlandfrac(grid[cell].ml.manure_nr,ncft,config->nagtree);
    }

    if(config->fertilizer_input==FERTILIZER)
    {
      /* assigning fertilizer Nr data */
      yearf-=landuse->fertilizer_nr.firstyear;
      if(yearf>=landuse->fertilizer_nr.nyear)
        yearf=landuse->fertilizer_nr.nyear-1;
      else if(yearf<0)
        yearf=0;
      data=newvec(Real,config->ngridcell*landuse->fertilizer_nr.var_len);
      if(data==NULL)
      {
        printallocerr("data");
        return TRUE;
      }
      if(landuse->fertilizer_nr.fmt==CDF)
      {
        if(readdata_netcdf(&landuse->fertilizer_nr,data,grid,yearf,config))
        {
          fprintf(stderr,
                  "ERROR149: Cannot read fertilizer of year %d in getlanduse().\n",
                  yearf+landuse->fertilizer_nr.firstyear);
          fflush(stderr);
          free(data);
          return TRUE;
        }
      }
      else
      {
        if(fseek(landuse->fertilizer_nr.file,(long long)yearf*landuse->fertilizer_nr.size+landuse->fertilizer_nr.offset,SEEK_SET))
        {
          fprintf(stderr,
                  "ERROR148: Cannot seek fertilizer Nr to year %d in getlanduse().\n",
                  yearf+landuse->fertilizer_nr.firstyear);
          fflush(stderr);
          free(data);
          return TRUE;
        }
        if(readrealvec(landuse->fertilizer_nr.file,data,0,landuse->fertilizer_nr.scalar,landuse->fertilizer_nr.n,
                       landuse->fertilizer_nr.swap,landuse->fertilizer_nr.datatype))
        {
          fprintf(stderr,
                  "ERROR149: Cannot read fertilizer Nr of year %d in getlanduse().\n",
                  yearf+landuse->fertilizer_nr.firstyear);
          fflush(stderr);
          free(data);
          return TRUE;
        }
      }
      count=0;

      /* do changes here for the fertilization*/
      for(cell=0; cell<config->ngridcell; cell++)
      {
        for(i=0; i<WIRRIG; i++)
        {
          if(readlandfracmap(grid[cell].ml.fertilizer_nr+i,config->fertilizermap,
                             config->fertilizermap_size,data,&count,ncft,config->nwptype))
          {
            fprintf(stderr,"ERROR149: Fertilizer input=%g for band %d less than zero for cell %d (%s) in year %d.\n",
                    data[count],count % config->fertilizermap_size+i*config->fertilizermap_size,
                    cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearf+landuse->fertilizer_nr.firstyear);
            return TRUE;
          }
        }
      } /* for(cell=0;...) */
      free(data);
    }

    if(config->manure_input)
    {
      /* assigning manure fertilizer nr data */
      yearm-=landuse->manure_nr.firstyear;
      if(yearm>=landuse->manure_nr.nyear)
        yearm=landuse->manure_nr.nyear-1;
      else if(yearm<0)
        yearm=0;
      data=newvec(Real,config->ngridcell*landuse->manure_nr.var_len);
      if(data==NULL)
      {
        printallocerr("data");
        return TRUE;
      }
      if(landuse->manure_nr.fmt==CDF)
      {
        if(readdata_netcdf(&landuse->manure_nr,data,grid,yearm,config))
        {
          fprintf(stderr,
            "ERROR149: Cannot read manure fertilizer of year %d in getlanduse().\n",
            yearm+landuse->manure_nr.firstyear);
          fflush(stderr);
          free(data);
          return TRUE;
        }
      }
      else
      {
        if(fseek(landuse->manure_nr.file,(long long)yearm*landuse->manure_nr.size+landuse->manure_nr.offset,SEEK_SET))
        {
          fprintf(stderr,
                  "ERROR148: Cannot seek manure fertilizer to year %d in getlanduse().\n",
                  yearm+landuse->manure_nr.firstyear);
          fflush(stderr);
          free(data);
          return TRUE;
        }
        if(readrealvec(landuse->manure_nr.file,data,0,landuse->manure_nr.scalar,landuse->manure_nr.n,
                       landuse->manure_nr.swap,landuse->manure_nr.datatype))
        {
          fprintf(stderr,
                  "ERROR149: Cannot read manure fertilizer of year %d in getlanduse().\n",
                  yearm+landuse->manure_nr.firstyear);
          fflush(stderr);
          free(data);
          return TRUE;
        }
      }
      count=0;

      /* do changes here for the manure*/
      for(cell=0; cell<config->ngridcell; cell++)
      {
        for(i=0; i<WIRRIG; i++)
        {
          if(readlandfracmap(grid[cell].ml.manure_nr+i,config->fertilizermap,
                             config->fertilizermap_size,data,&count,ncft,config->nwptype))
          {
            fprintf(stderr,"ERROR149: Manure input=%g for band %d less than zero for cell %d (%s) in year %d.\n",
                    data[count],count % config->fertilizermap_size+i*config->fertilizermap_size,
                    cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearm+landuse->manure_nr.firstyear);
            return TRUE;
          }
        }
      } /* for(cell=0;...) */
      free(data);
    }

    if(config->fix_fertilization)
    {
      for(cell=0; cell<config->ngridcell; cell++)
      {
        for(i=0; i<WIRRIG; i++)
        {
          for(j=0; j<ncft; j++)
         {
            grid[cell].ml.fertilizer_nr[i].crop[j]=param.fertilizer_rate;
            grid[cell].ml.manure_nr[i].crop[j]=param.manure_rate;
          }
          for(j=0; j<NGRASS; j++)
          {
            grid[cell].ml.fertilizer_nr[i].grass[j]=param.fertilizer_rate;
            grid[cell].ml.manure_nr[i].grass[j]=param.manure_rate;
          }
          for(j=0; j<config->nagtree; j++)
          {
            grid[cell].ml.fertilizer_nr[i].ag_tree[j]=param.fertilizer_rate;
            grid[cell].ml.manure_nr[i].ag_tree[j]=param.manure_rate;
          }
          grid[cell].ml.fertilizer_nr[i].biomass_grass=param.fertilizer_rate;
          grid[cell].ml.fertilizer_nr[i].biomass_tree=param.fertilizer_rate;
          grid[cell].ml.manure_nr[i].biomass_grass=param.manure_rate;
          grid[cell].ml.manure_nr[i].biomass_tree=param.manure_rate;
          grid[cell].ml.fertilizer_nr[i].woodplantation=param.fertilizer_rate;
          grid[cell].ml.manure_nr[i].woodplantation=param.manure_rate;
        }
      }
    }

  }

  if(config->tillage_type==READ_TILLAGE)
  {
    /* read in tillage data */
    yeart-=landuse->with_tillage.firstyear;
    if(yeart>=landuse->with_tillage.nyear)
      yeart=landuse->with_tillage.nyear-1;
    else if(yeart<0)
      yeart=0;
    dates=newvec(int,config->ngridcell*landuse->with_tillage.var_len);
    if(dates==NULL)
    {
      printallocerr("dates");
      return TRUE;
    }
    if(landuse->with_tillage.fmt==CDF)
    {
      if(readintdata_netcdf(&landuse->with_tillage,dates,grid,yeart,config))
      {
        fprintf(stderr,
                "ERROR149: Cannot read tillage types of year %d in getlanduse().\n",
                yeart+landuse->with_tillage.firstyear);
        free(dates);
        fflush(stderr);
        return TRUE;
      }
    }
    else
    {
      if(fseek(landuse->with_tillage.file,(long long)yeart*landuse->with_tillage.size+landuse->with_tillage.offset,SEEK_SET))
      {
        fprintf(stderr,
                "ERROR148: Cannot seek tillage types to year %d in getlanduse().\n",
                yeart+landuse->with_tillage.firstyear);
        free(dates);
        return TRUE;
      }
      if(readintvec(landuse->with_tillage.file,dates,landuse->with_tillage.n,landuse->with_tillage.swap,landuse->with_tillage.datatype))
      {
        fprintf(stderr,
                "ERROR149: Cannot read tillage types of year %d in getlanduse().\n",
                yeart+landuse->with_tillage.firstyear);
        free(dates);
        return TRUE;
      }
    }
    count=0;
    for(cell=0; cell<config->ngridcell; cell++)
      if(!grid[cell].skip)
        grid[cell].ml.with_tillage=dates[count++];
      else
        count++;
    free(dates);
  }
  else
  {
    for(cell=0; cell<config->ngridcell; cell++)
      grid[cell].ml.with_tillage=config->tillage_type!=NO_TILLAGE;
  }

  if(config->residue_treatment==READ_RESIDUE_DATA)
  {
    /* assigning residue extraction data */
    yearr-=landuse->residue_on_field.firstyear;
    if(yearr>=landuse->residue_on_field.nyear)
      yearr=landuse->residue_on_field.nyear-1;
    else if(yearr<0)
      yearr=0;
    data=newvec(Real,config->ngridcell*landuse->residue_on_field.var_len);
    if(data==NULL)
    {
      printallocerr("data");
      return TRUE;
    }
    if(landuse->residue_on_field.fmt==CDF)
    {
      if(readdata_netcdf(&landuse->residue_on_field,data,grid,yearr,config))
      {
        fprintf(stderr,
                "ERROR149: Cannot read residue extraction of year %d in getlanduse().\n",
                yearr+landuse->residue_on_field.firstyear);
        fflush(stderr);
        free(data);
        return TRUE;
      }
    }
    else
    {
      if(fseek(landuse->residue_on_field.file,(long long)yearr*landuse->residue_on_field.size+landuse->residue_on_field.offset,SEEK_SET))
      {
        fprintf(stderr,
                "ERROR148: Cannot seek residue extraction to year %d in getlanduse().\n",
                yearr+landuse->residue_on_field.firstyear);
        fflush(stderr);
        free(data);
        return TRUE;
      }
      if(readrealvec(landuse->residue_on_field.file,data,0,landuse->residue_on_field.scalar,landuse->residue_on_field.n,
                     landuse->residue_on_field.swap,landuse->residue_on_field.datatype))
      {
        fprintf(stderr,
                "ERROR149: Cannot read residue extraction of year %d in getlanduse().\n",
                yearr+landuse->residue_on_field.firstyear);
        fflush(stderr);
        free(data);
        return TRUE;
      }
    }
    count=0;

    /* do changes for residue rate left on field*/
    for(cell=0; cell<config->ngridcell; cell++)
    {
      initlandfrac(grid[cell].ml.residue_on_field,ncft,config->nagtree);
      if(readlandfracmap(grid[cell].ml.residue_on_field,config->fertilizermap,
                         config->fertilizermap_size,data,&count,ncft,config->nwptype))
      {
        fprintf(stderr,"ERROR149: Residue rate input=%g for band %d less than zero for cell %d (%s) in year %d.\n",
                data[count],count % config->fertilizermap_size,
                cell+config->startgrid,sprintcoord(line,&grid[cell].coord),yearr+landuse->residue_on_field.firstyear);
        return TRUE;
      }
      count-=config->fertilizermap_size;
      readlandfracmap(grid[cell].ml.residue_on_field+1,config->fertilizermap,
                      config->fertilizermap_size,data,&count,ncft,config->nwptype);
    }
    free(data);
  }
  return FALSE;
} /* of 'getlanduse' */

Bool getintercrop(const Landuse landuse /**< pointer to landuse data */
                 )                      /** \return intercropping enabled? (TRUE/FALSE) */
{
  return (landuse==NULL) ? FALSE : landuse->intercrop;
} /* of 'getintercrop' */

void freelanduse(Landuse landuse,     /**< pointer to landuse data */
                 const Config *config /**< LPJmL configuration */
)
{
  if(landuse!=NULL)
  {
    closeclimatefile(&landuse->landuse,isroot(*config));
    if(config->sdate_option==PRESCRIBED_SDATE)
      closeclimatefile(&landuse->sdate,isroot(*config));
    if(config->fertilizer_input==FERTILIZER)
      closeclimatefile(&landuse->fertilizer_nr,isroot(*config));
    if(config->crop_phu_option==PRESCRIBED_CROP_PHU)
       closeclimatefile(&landuse->crop_phu,isroot(*config));
    if(config->manure_input)
       closeclimatefile(&landuse->manure_nr,isroot(*config));
    if(config->tillage_type==READ_TILLAGE)
       closeclimatefile(&landuse->with_tillage,isroot(*config));
    if(config->residue_treatment==READ_RESIDUE_DATA)
       closeclimatefile(&landuse->residue_on_field,isroot(*config));
    free(landuse);
  }
} /* of 'freelanduse' */
