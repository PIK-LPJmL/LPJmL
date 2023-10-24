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

static const Filename *findoutputfile(int index,const Config *config)
{
  int i;
  for(i=0;i<config->n_out;i++)
    if(config->outputvars[i].id==index)
      return &config->outputvars[i].filename;
  return NULL;
} /* of 'findoutputfile' */

static void fprintreffile(FILE *file,int index,int ref_index,const char *ref_name,const Config *config)
{
  const Filename *ref_filename;
  char *name;
  if(config->outputvars[index].id!=ref_index)
  {
    ref_filename=findoutputfile(ref_index,config);
    if(ref_filename!=NULL)
    {
      if(ref_filename->meta)
      {
         name=malloc(strlen(ref_filename->name)+strlen(config->json_suffix)+1);
         name=strcat(strcpy(name,ref_filename->name),config->json_suffix);
         fprintf(file,"  \"%s\" : {\"filename\" : \"%s\", \"format\" : \"meta\"},\n",ref_name,strippath(name));
         free(name);
      }
      else if(ref_filename->fmt==CLM)
        fprintf(file,"  \"%s\" : {\"filename\" : \"%s\", \"format\" : \"clm\", \"version\" : %d}\n,",
                 ref_name,strippath(ref_filename->name),ref_filename->version);
      else
      {
        fprintf(file,"  \"%s\" : {\"filename\" : \"%s\", \"format\" : \"%s\", \"scalar\" : %g,",
                ref_name,
                strippath(ref_filename->name),
                fmt[ref_filename->fmt],
                (ref_index!=GRID || config->float_grid || ref_filename->fmt==TXT || ref_filename->fmt==CDF) ? 1 : 0.01);
        fprintf(file," \"datatype\" : \"%s\"},\n",
                typenames[(ref_index!=GRID || config->float_grid || ref_filename->fmt==TXT || ref_filename->fmt==CDF) ? LPJ_FLOAT : LPJ_SHORT]);
      }
    }
  }
} /* of 'fprintreffile' */

Bool fprintoutputjson(int index,           /**< index in outputvars array */
                      int year,            /**< year one-year output is written */
                      const Config *config /**< LPJmL configuration */
                     )                     /** \return TRUE on error */
{
  FILE *file;
  char *filename;
  char *json_filename;
  char **pftnames;
  int p,nbands,len,count,id;
  id=config->outputvars[index].id;
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
  fprintf(file,"  \"history\" : \"%s\",\n",config->arglist);
  fprintf(file,"  \"variable\" : \"%s\",\n",config->outnames[id].name);
  fprintf(file,"  \"firstcell\" : %d,\n",config->firstgrid);
  fprintf(file,"  \"ncell\" : %d,\n",(id==ADISCHARGE) ? config->nall : config->total);
  fprintf(file,"  \"cellsize_lon\" : %f,\n",config->resolution.lon);
  fprintf(file,"  \"cellsize_lat\" : %f,\n",config->resolution.lat);
  fprintf(file,"  \"nstep\" : %d,\n",max(1,getnyear(config->outnames,id)));
  fprintf(file,"  \"timestep\" : %d,\n",max(1,config->outputvars[index].filename.timestep));
  nbands=outputsize(id,
                    config->npft[GRASS]+config->npft[TREE],
                    config->npft[CROP],config);
  fprintf(file,"  \"nbands\" : %d,\n",id==GRID ? 2 : nbands);
  if(nbands>1)
  {
   if(issoil(id))
   {
     fprintf(file,"  \"" BAND_NAMES "\" : [%f",layerbound[0]);
     for(p=1;p<nbands;p++)
       fprintf(file,",%f",layerbound[p]);
     fprintf(file,"],\n");
   }
   else
   {
     pftnames=createpftnames(id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
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
       freepftnames(pftnames,id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
     }
   }
  }
  fprintf(file,"  \"descr\" : ");
  if(config->outnames[id].descr==NULL)
    fprintf(file,"null,\n");
  else
    fprintf(file,"\"%s\",\n",config->outnames[id].descr);
  fprintf(file,"  \"unit\" : ");
  if(config->outnames[id].unit==NULL)
    fprintf(file,"null,\n");
  else
    fprintf(file,"\"%s\",\n",config->outnames[id].unit);
  if(getnyear(config->outnames,id)==0)
  {
    fprintf(file,"  \"firstyear\" : %d,\n",config->outputyear);
    fprintf(file,"  \"lastyear\" : %d,\n"
            "  \"nyear\" : 1,\n",config->outputyear);
  }
  else
  {
    fprintf(file,"  \"firstyear\" : %d,\n",config->outputvars[index].oneyear ? year : config->outputyear+max(1,config->outnames[id].timestep)-1);
    fprintf(file,"  \"lastyear\" : %d,\n",config->outputvars[index].oneyear ? year : config->outputyear+max(1,config->outnames[id].timestep)-1+((config->lastyear-config->outputyear+1)/max(1,config->outnames[id].timestep)-1)*max(1,config->outnames[id].timestep));
    fprintf(file,"  \"nyear\" : %d,\n",(config->outputvars[index].oneyear) ? 1 : (config->lastyear-config->outputyear+1)/max(1,config->outnames[id].timestep));
  }
  if(config->outputvars[index].id==GRID)
  {
    fprintf(file,"  \"datatype\" : \"%s\",\n",
            typenames[(config->float_grid || config->outputvars[index].filename.fmt==TXT || config->outputvars[index].filename.fmt==CDF) ? LPJ_FLOAT : LPJ_SHORT]);
    if(config->float_grid || config->outputvars[index].filename.fmt==TXT || config->outputvars[index].filename.fmt==CDF)
      fprintf(file,"  \"scalar\" : 1.0,\n");
    else
      fprintf(file,"  \"scalar\" : 0.01,\n");
    fprintf(file,"  \"order\" : \"cellyear\",\n");
  }
  else
  {
    fprintf(file,"  \"datatype\" : \"%s\",\n",typenames[getoutputtype(id,config->float_grid)]);
    fprintf(file,"  \"scalar\" : 1.0,\n");
    fprintf(file,"  \"order\" : \"cellseq\",\n");
  }
  fprintf(file,"  \"bigendian\" : %s,\n",bool2str(bigendian()));
  if(config->outputvars[index].filename.fmt==CLM)
    fprintf(file,"  \"offset\" : %zu,\n",id==GRID ? headersize(LPJGRID_HEADER,config->outputvars[index].filename.version) : headersize(LPJOUTPUT_HEADER,config->outputvars[index].filename.version));
  else if(config->outputvars[index].filename.fmt==TXT)
    fprintf(file,"  \"delimiter\" : \"%c\",\n",config->csv_delimit);
  fprintf(file,"  \"format\" : \"%s\",\n",fmt[config->outputvars[index].filename.fmt]);
  if(config->outputvars[index].filename.fmt==CLM)
    fprintf(file,"  \"version\" : %d,\n",config->outputvars[index].filename.version);
  fprintreffile(file,index,GRID,"grid",config);
  fprintreffile(file,index,TERR_AREA,"ref_area",config);
  fprintf(file,"  \"filename\" : \"%s\"\n",strippath(filename));
  fprintf(file,"}\n");
  fclose(file);
  if(config->outputvars[index].oneyear)
    free(filename);
  free(json_filename);
  return FALSE;
} /* of 'fprintoutputjson' */
