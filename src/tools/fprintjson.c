/**************************************************************************************/
/**                                                                                \n**/
/**               f  p  r  i  n  t  j  s  o  n  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions prints JSON file from clm header                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define LINE_LEN 80

void fprintjson(FILE *file,           /**< pointer to text file */
                const char *filename, /**< filename of clm file */
                const char *source,   /**< source string or NULL */
                const char *history,  /**< history string or NULL */
                const char *arglist,  /**< argument string or NULL */
                const Header *header, /**< file header */
                Map *map,             /**< pointer to map array or NULL */
                const char *map_name, /**< Name of string array or NULL */
                const Attr *attrs,    /**< array of attributes */
                int n_attr,           /**< size of array of attributes */
                const char *variable, /**< name of variable of NULL */
                const char *unit,     /**< unit of variable or NULL */
                const char *standard_name,    /**< description of variable or NULL */
                const char *long_name,    /**< description of variable or NULL */
                const Filename *gridfile, /**< filename of grid file or NULL */
                Type grid_type,       /**< datatype of grid */
                int format,           /**< file format (RAW/CLM) */
                const char *id,       /**< Id of clm file */
                Bool swap,            /**< byte order has to be swapped (TRUE/FALSE) */
                int version           /**< version of clm file */
               )

{
  int i,len;
  time_t t;
  fprintf(file,"{\n"
          "  \"filename\" : \"%s\",\n",strippath(filename));
  if(source!=NULL)
    fprintf(file,"  \"source\" : \"%s\",\n",source);
  if(history==NULL)
  {
    if(arglist!=NULL)
    {
      time(&t);
      fprintf(file,"  \"history\" : \"%s: %s\",\n",strdate(&t),arglist);
    }
  }
  else
  {
    if(arglist!=NULL)
    {
      time(&t);
      fprintf(file,"  \"history\" : \"");
      fputprintable(file,history);
      fprintf(file,"\\n%s: %s\",\n",strdate(&t),arglist);
    }
    else
    {
      fprintf(file,"  \"history\" : \"");
      fputprintable(file,history);
      fprintf(file,"\",\n");
    }
  }
  if(n_attr)
  {
    fprintf(file,"  \"global_attrs\" : {");
    for(i=0;i<n_attr;i++)
    {
      fprintf(file,"\"%s\" : \"%s\"",attrs[i].name,attrs[i].value);
      if(i<n_attr-1)
        fprintf(file,", ");
    }
    fprintf(file,"},\n");
  }
  if(variable!=NULL)
    fprintf(file,"  \"variable\" : \"%s\",\n",variable);
  fprintf(file,"  \"firstcell\" : %d,\n",header->firstcell);
  fprintf(file,"  \"ncell\" : %d,\n",header->ncell);
  fprintf(file,"  \"cellsize_lon\" : %f,\n",header->cellsize_lon);
  fprintf(file,"  \"cellsize_lat\" : %f,\n",header->cellsize_lat);
  fprintf(file,"  \"firstyear\" : %d,\n",header->firstyear);
  fprintf(file,"  \"lastyear\" : %d,\n",header->firstyear+(header->nyear-1)*header->timestep);
  fprintf(file,"  \"nyear\" : %d,\n",header->nyear);
  fprintf(file,"  \"nstep\" : %d,\n",header->nstep);
  fprintf(file,"  \"timestep\" : %d,\n",header->timestep);
  fprintf(file,"  \"nbands\" : %d,\n",header->nbands);
  fprintf(file,"  \"scalar\" : %f,\n",header->scalar);
  fprintf(file,"  \"datatype\" : \"%s\",\n",typenames[header->datatype]);
  if(map!=NULL)
  {
    len=fprintf(file,"  \"%s\" : [",map_name);
    for(i=0;i<getmapsize(map);i++)
    {
      if(i)
        len+=fprintf(file,",");
      if(len>LINE_LEN)
        len=fprintf(file,"\n    ")-1;
      if(getmapitem(map,i)==NULL)
        len+=fprintf(file,"null");
      else if(map->isfloat)
        len+=fprintf(file,"%g",*((double *)getmapitem(map,i)));
      else
        len+=fprintf(file,"\"%s\"",(char *)getmapitem(map,i));
    }
    fputs("],\n",file);
  }
  if(unit!=NULL)
    fprintf(file,"  \"unit\" : \"%s\",\n",unit);
  if(standard_name!=NULL)
    fprintf(file,"  \"standard_name\" : \"%s\",\n",standard_name);
  if(long_name!=NULL)
    fprintf(file,"  \"long_name\" : \"%s\",\n",long_name);
  if(format>=0 && format<N_FMT)
    fprintf(file,"  \"format\" : \"%s\",\n",fmt[format]);
  fprintf(file,"  \"order\" : \"%s\",\n",ordernames[max(0,header->order-1)]);
  if(format==CLM || format==CLM2)
  {
    fprintf(file,"  \"version\" : %d,\n",version);
    fprintf(file,"  \"offset\" : %zu,\n",headersize(id,version));
  }
  if(gridfile!=NULL)
  {
    if(gridfile->fmt==META)
      fprintf(file,"  \"grid\" : {\"filename\" : \"%s\", \"format\" : \"%s\"},\n",
              strippath(gridfile->name),
              fmt[gridfile->fmt]);
    else
      fprintf(file,"  \"grid\" : {\"filename\" : \"%s\", \"format\" : \"%s\", \"datatype\" : \"%s\"},\n",
              strippath(gridfile->name),
              fmt[gridfile->fmt],
              typenames[grid_type]);
  }
  fprintf(file,"  \"bigendian\" : %s\n",bool2str((!swap && bigendian()) || (swap && !bigendian())));
  fprintf(file,"}\n");
} /* of 'fprintjson' */
