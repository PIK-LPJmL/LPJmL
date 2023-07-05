/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  o  u  t  p  u  t  j  s  o  n  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints JSON metafile for binary output files                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define LINE_LEN 80  /* line length of JSON file */

static const Filename *findgridfile(const Config *config)
{
  int i;
  for(i=0;i<config->n_out;i++)
    if(config->outputvars[i].id==GRID)
      return &config->outputvars[i].filename;
  return NULL;
} /* of 'findgridfile' */

Bool fprintoutputjson(int index,           /**< index in outputvars array */
                      int year,            /**< year one-year output is written */
                      const Config *config /**< LPJmL configuration */
                     )                     /** \return TRUE on error */
{
  FILE *file;
  char *filename;
  char *json_filename;
  const Filename *grid_filename;
  char **pftnames;
  time_t t;
  int p,nbands,len,count;
  if(config->outputvars[index].oneyear)
  {
    count=snprintf(NULL,0,config->outputvars[index].filename.name,year);
    if(count==-1)
      return TRUE;
    filename=malloc(count+1);
    if(filename==NULL)
    {
      printallocerr("filename");
      return TRUE;
    }
    snprintf(filename,count+1,config->outputvars[index].filename.name,year);
  }
  else
    filename=config->outputvars[index].filename.name;
  /* create filename for JSON metafile */
  json_filename=malloc(strlen(filename)+strlen(config->json_suffix)+1);
  if(json_filename==NULL)
  {
    if(config->outputvars[index].oneyear)
      free(filename);
    printallocerr("json_filename");
    return TRUE;
  }
  strcat(strcpy(json_filename,filename),config->json_suffix);
  /* add absolute path to binary file if missing */
  /* create metafile */
  file=fopen(json_filename,"w");
  if(file==NULL)
  {
    printfcreateerr(json_filename);
    if(config->outputvars[index].oneyear)
      free(filename);
    free(json_filename);
    return TRUE;
  }
  fprintf(file,"{\n");
  fprintf(file,"  \"sim_name\" : \"%s\",\n",config->sim_name);
  fprintf(file,"  \"source\" : \"LPJmL C Version " LPJ_VERSION"\",\n");
  time(&t);
  fprintf(file,"  \"history\" : \"%s: %s\",\n",strdate(&t),config->arglist);
  if(config->n_global)
  {
    fprintf(file,"  \"global_attrs\" : {");
    for(p=0;p<config->n_global;p++)
    {
      fprintf(file,"\"%s\" : \"%s\"",config->global_attrs[p].name,config->global_attrs[p].value);
      if(p<config->n_global-1)
        fprintf(file,", ");
    }
    fprintf(file,"},\n");
  }
  fprintf(file,"  \"variable\" : \"%s\",\n",config->outnames[config->outputvars[index].id].var);
  fprintf(file,"  \"firstcell\" : %d,\n",config->firstgrid);
  fprintf(file,"  \"ncell\" : %d,\n",(config->outputvars[index].id==ADISCHARGE) ? config->nall : config->total);
  fprintf(file,"  \"cellsize_lon\" : %f,\n",config->resolution.lon);
  fprintf(file,"  \"cellsize_lat\" : %f,\n",config->resolution.lat);
  fprintf(file,"  \"nstep\" : %d,\n",max(1,getnyear(config->outnames,config->outputvars[index].id)));
  fprintf(file,"  \"timestep\" : %d,\n",max(1,config->outputvars[index].filename.timestep));
  nbands=outputsize(config->outputvars[index].id,
                    config->npft[GRASS]+config->npft[TREE],
                    config->npft[CROP],config);
  fprintf(file,"  \"nbands\" : %d,\n",config->outputvars[index].id==GRID ? 2 : nbands);
  if(nbands>1)
  {
   if(issoil(config->outputvars[index].id))
   {
     fprintf(file,"  \"" BAND_NAMES "\" : [%f",layerbound[0]);
     for(p=1;p<nbands;p++)
       fprintf(file,",%f",layerbound[p]);
     fprintf(file,"],\n");
   }
   else
   {
     pftnames=createpftnames(config->outputvars[index].id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
     if(pftnames==NULL)
       printallocerr(BAND_NAMES);
     else
     {
       len=fprintf(file,"  \"" BAND_NAMES "\" : [");
       for(p=0;p<nbands;p++)
       {
         if(p)
           len+=fprintf(file,",");
         if(len>LINE_LEN)
         {
           fputs("\n                  ",file);
           len=11;
         }
         len+=fprintf(file,"\"%s\"",pftnames[p]);
       }
       fprintf(file,"],\n");
       freepftnames(pftnames,config->outputvars[index].id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
     }
   }
  }
  fprintf(file,"  \"standard_name\" : ");
  if(config->outnames[config->outputvars[index].id].standard_name==NULL)
    fprintf(file,"\"%s\",\n",config->outnames[config->outputvars[index].id].name);
  else
    fprintf(file,"\"%s\",\n",config->outnames[config->outputvars[index].id].standard_name);
  fprintf(file,"  \"long_name\" : ");
  if(config->outnames[config->outputvars[index].id].long_name==NULL)
    fprintf(file,"null,\n");
  else
    fprintf(file,"\"%s\",\n",config->outnames[config->outputvars[index].id].long_name);
  fprintf(file,"  \"unit\" : ");
  if(config->outnames[config->outputvars[index].id].unit==NULL)
    fprintf(file,"null,\n");
  else
    fprintf(file,"\"%s\",\n",config->outnames[config->outputvars[index].id].unit);
  if(config->outputvars[index].id==GRID || config->outputvars[index].id==COUNTRY || config->outputvars[index].id==REGION)
  {
    fprintf(file,"  \"firstyear\" : %d,\n",config->outputyear);
    fprintf(file,"  \"lastyear\" : %d,\n",config->outputyear);
  }
  else
  {
    fprintf(file,"  \"firstyear\" : %d,\n",config->outputvars[index].oneyear ? year : config->outputyear+max(1,config->outnames[config->outputvars[index].id].timestep)-1);
    fprintf(file,"  \"lastyear\" : %d,\n",config->outputvars[index].oneyear ? year : config->outputyear+max(1,config->outnames[config->outputvars[index].id].timestep)-1+((config->lastyear-config->outputyear+1)/max(1,config->outnames[config->outputvars[index].id].timestep)-1)*max(1,config->outnames[config->outputvars[index].id].timestep));
  }
  fprintf(file,"  \"nyear\" : %d,\n",(config->outputvars[index].oneyear || config->outputvars[index].id==GRID || config->outputvars[index].id==COUNTRY || config->outputvars[index].id==REGION) ? 1 : (config->lastyear-config->outputyear+1)/max(1,config->outnames[config->outputvars[index].id].timestep));
  fprintf(file,"  \"datatype\" : \"%s\",\n",typenames[getoutputtype(config->outputvars[index].id,config->grid_type)]);
  if(config->outputvars[index].id==GRID)
  {
    if(config->grid_type==LPJ_SHORT)
      fprintf(file,"  \"scalar\" : 0.01,\n");
    else
      fprintf(file,"  \"scalar\" : 1.0,\n");
    fprintf(file,"  \"order\" : \"cellyear\",\n");
  }
  else
  {
    fprintf(file,"  \"scalar\" : 1.0,\n");
    fprintf(file,"  \"order\" : \"cellseq\",\n");
  }
  fprintf(file,"  \"bigendian\" : %s,\n",bool2str(bigendian()));
  if(config->outputvars[index].filename.fmt==CLM)
    fprintf(file,"  \"offset\" : %zu,\n",config->outputvars[index].id==GRID ? headersize(LPJGRID_HEADER,config->outputvars[index].filename.version) : headersize(LPJOUTPUT_HEADER,config->outputvars[index].filename.version));
  else if(config->outputvars[index].filename.fmt==TXT)
    fprintf(file,"  \"delimiter\" : \"%c\",\n",config->csv_delimit);
  fprintf(file,"  \"format\" : \"%s\",\n",fmt[config->outputvars[index].filename.fmt]);
  if(config->outputvars[index].filename.fmt==CLM)
    fprintf(file,"  \"version\" : %d,\n",config->outputvars[index].filename.version);
  if(config->outputvars[index].id!=GRID)
  {
    grid_filename=findgridfile(config);
    if(grid_filename!=NULL)
      fprintf(file,"  \"grid\" : {\"filename\" : \"%s\", \"format\" : \"%s\", \"datatype\" : \"%s\"},\n",
              strippath(grid_filename->name),
              fmt[grid_filename->fmt],
              typenames[config->grid_type]);
  }
  fprintf(file,"  \"filename\" : \"%s\"\n",strippath(filename));
  fprintf(file,"}\n");
  fclose(file);
  if(config->outputvars[index].oneyear)
    free(filename);
  free(json_filename);
  return FALSE;
} /* of 'fprintoutputjson' */
