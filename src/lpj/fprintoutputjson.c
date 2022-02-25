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

#define SUFFIX ".json" /* file suffix for JSON file */

Bool fprintoutputjson(int i,int year,const Config *config)
{
  FILE *file;
  String s;
  char *filename;
  char *json_filename;
  char **pftnames;
  int p,nbands;
  if(config->outputvars[i].oneyear)
  {
    snprintf(s,STRING_LEN,config->outputvars[i].filename.name,year);
    filename=s;
  }
  else
    filename=config->outputvars[i].filename.name;
  /* create filename for JSON metafile */
  json_filename=malloc(strlen(filename)+strlen(SUFFIX)+1);
  if(json_filename==NULL)
  {
    printallocerr("json_filename");
    return TRUE;
  }
  strcat(strcpy(json_filename,filename),SUFFIX);
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
  fprintf(file,"  \"firstcell\" : %d,\n",config->firstgrid);
  fprintf(file,"  \"ncell\" : %d,\n",(config->outputvars[i].id==ADISCHARGE) ? config->nall : config->total);
  fprintf(file,"  \"cellsize\" : [%f,%f],\n",config->resolution.lon,config->resolution.lat);
  fprintf(file,"  \"nstep\" : %d,\n",max(1,getnyear(config->outnames,config->outputvars[i].id)));
  nbands=outputsize(config->outputvars[i].id,
                    config->npft[GRASS]+config->npft[TREE],
                    config->npft[CROP],config);
  fprintf(file,"  \"nbands\" : %d,\n",config->outputvars[i].id==GRID ? 2 : nbands);
  if(nbands>1)
  {
   if(issoil(config->outputvars[i].id))
   {
     fprintf(file,"  \"layer\" : [0.0,");
     for(p=1;p<nbands;p++)
       fprintf(file,",%f",layerbound[p-1]);
     fprintf(file,"],\n");
   }
   else
   {
     pftnames=createpftnames(config->outputvars[i].id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
     if(pftnames==NULL)
     {
       printallocerr("pftnames");
       return TRUE;
     }
     fprintf(file,"  \"pft\" : [");
     for(p=0;p<nbands;p++)
     {
       if(p)
         fprintf(file,",");
       fprintf(file,"\"%s\"",pftnames[p]);
     }
     fprintf(file,"],\n");
     freepftnames(pftnames,config->outputvars[i].id,config->npft[GRASS]+config->npft[TREE],config->npft[CROP],config);
   }
  }
  fprintf(file,"  \"descr\" : \"%s\",\n",(config->outnames[config->outputvars[i].id].descr==NULL) ? "null" : config->outnames[config->outputvars[i].id].descr);
  fprintf(file,"  \"unit\" : \"%s\",\n",(config->outnames[config->outputvars[i].id].unit==NULL) ? "null" : config->outnames[config->outputvars[i].id].unit);
  fprintf(file,"  \"firstyear\" : %d,\n",config->outputvars[i].oneyear ? year : config->outputyear);
  fprintf(file,"  \"nyear\" : %d,\n",(config->outputvars[i].oneyear || config->outputvars[i].id==GRID || config->outputvars[i].id==COUNTRY || config->outputvars[i].id==REGION) ? 1 : config->lastyear-config->outputyear+1);
  if(config->outputvars[i].id==GRID)
  {
    fprintf(file,"  \"datatype\" : \"%s\",\n",typenames[config->float_grid ? LPJ_FLOAT : LPJ_SHORT]);
    fprintf(file,"  \"scaling\" : 0.01,\n");
    fprintf(file,"  \"order\" : \"cellyear\",\n");
  }
  else
  {
    fprintf(file,"  \"datatype\" : \"%s\",\n",typenames[getoutputtype(config->outputvars[i].id,FALSE)]);
    fprintf(file,"  \"scaling\" : 1.0,\n");
    fprintf(file,"  \"order\" : \"cellseq\",\n");
  }
  fprintf(file,"  \"bigendian\" : %s,\n",bool2str(bigendian()));
  fprintf(file,"  \"filename\" : \"%s\"\n",filename);
  fprintf(file,"}\n");
  fclose(file);
  free(json_filename);
  return FALSE;
} /* of 'fprintoutputjson' */
