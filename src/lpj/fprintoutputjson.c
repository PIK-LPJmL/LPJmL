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

Bool fprintoutputjson(int index,           /**< index in outputvars array */
                      int year,            /**< year one-year output is written */
                      const Config *config /**< LPJmL configuration */
                     )                     /** \return TRUE on error */
{
  FILE *file;
  String s;
  char *filename;
  char *json_filename;
  char **pftnames;
  int p,nbands,len;
  if(config->outputvars[index].oneyear)
  {
    snprintf(s,STRING_LEN,config->outputvars[index].filename.name,year);
    filename=s;
  }
  else
    filename=config->outputvars[index].filename.name;
  /* create filename for JSON metafile */
  json_filename=malloc(strlen(filename)+strlen(config->json_suffix)+1);
  if(json_filename==NULL)
  {
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
    free(json_filename);
    return TRUE;
  }
  fprintf(file,"{\n");
  fprintf(file,"  \"sim_name\" : \"%s\",\n",config->sim_name);
  fprintf(file,"  \"source\" : \"LPJmL C Version " LPJ_VERSION"\",\n");
  fprintf(file,"  \"history\" : \"%s\",\n",config->arglist);
  fprintf(file,"  \"firstcell\" : %d,\n",config->firstgrid);
  fprintf(file,"  \"ncell\" : %d,\n",(config->outputvars[index].id==ADISCHARGE) ? config->nall : config->total);
  fprintf(file,"  \"cellsize\" : [%f,%f],\n",config->resolution.lon,config->resolution.lat);
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
     fprintf(file,"  \"layer\" : [0.0,");
     for(p=1;p<nbands;p++)
       fprintf(file,",%f",layerbound[p-1]);
     fprintf(file,"],\n");
   }
   else
   {
     pftnames=createpftnames(config->outputvars[index].id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
     if(pftnames==NULL)
       printallocerr("pftnames");
     else
     {
       len=fprintf(file,"  \"pft\" : [");
       for(p=0;p<nbands;p++)
       {
         if(p)
           len+=fprintf(file,",");
         if(len>LINE_LEN)
         {
           fputs("\n           ",file);
           len=11;
         }
         len+=fprintf(file,"\"%s\"",pftnames[p]);
       }
       fprintf(file,"],\n");
       freepftnames(pftnames,config->outputvars[index].id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
     }
   }
  }
  fprintf(file,"  \"descr\" : ");
  if(config->outnames[config->outputvars[index].id].descr==NULL)
    fprintf(file,"null,\n");
  else
    fprintf(file,"\"%s\",\n",config->outnames[config->outputvars[index].id].descr);
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
    fprintf(file,"  \"firstyear\" : %d,\n",config->outputvars[index].oneyear ? year : config->outputyear+max(1,config->outnames[config->outputvars[index].id].timestep)/2);
    fprintf(file,"  \"lastyear\" : %d,\n",config->outputvars[index].oneyear ? year : config->lastyear);
  }
  fprintf(file,"  \"nyear\" : %d,\n",(config->outputvars[index].oneyear || config->outputvars[index].id==GRID || config->outputvars[index].id==COUNTRY || config->outputvars[index].id==REGION) ? 1 : (config->lastyear-config->outputyear+1)/max(1,config->outnames[config->outputvars[index].id].timestep));
  fprintf(file,"  \"datatype\" : \"%s\",\n",typenames[getoutputtype(config->outputvars[index].id,config->float_grid)]);
  if(config->outputvars[index].id==GRID)
  {
    if(config->float_grid)
      fprintf(file,"  \"scaling\" : 1.0,\n");
    else
      fprintf(file,"  \"scaling\" : 0.01,\n");
    fprintf(file,"  \"order\" : \"cellyear\",\n");
  }
  else
  {
    fprintf(file,"  \"scaling\" : 1.0,\n");
    fprintf(file,"  \"order\" : \"cellseq\",\n");
  }
  fprintf(file,"  \"bigendian\" : %s,\n",bool2str(bigendian()));
  if(config->outputvars[index].filename.fmt==CLM)
    fprintf(file,"  \"offset\" : %zu,\n",config->outputvars[index].id==GRID ? headersize(LPJGRID_HEADER,LPJGRID_VERSION) : headersize(LPJOUTPUT_HEADER,LPJOUTPUT_VERSION));
  else if(config->outputvars[index].filename.fmt==TXT)
    fprintf(file,"  \"delimiter\" : \"%c\",\n",config->csv_delimit);
  fprintf(file,"  \"format\" : \"%s\",\n",fmt[config->outputvars[index].filename.fmt]);
  if(config->outputvars[index].filename.fmt==CLM)
    fprintf(file,"  \"version\" : %d,\n",config->outputvars[index].id==GRID ? LPJGRID_VERSION : LPJOUTPUT_VERSION);
  fprintf(file,"  \"filename\" : \"%s\"\n",strippath(filename));
  fprintf(file,"}\n");
  fclose(file);
  free(json_filename);
  return FALSE;
} /* of 'fprintoutputjson' */
