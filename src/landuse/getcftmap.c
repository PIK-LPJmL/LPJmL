/**************************************************************************************/
/**                                                                                \n**/
/**                    g  e  t  c  f  t  m  a  p  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets landuse mapping from string array                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

int *getcftmap(const Map *map,      /**< pointer to string array  */
               const char *name,    /**< name or filename for map */
               Bool cftonly,        /**< scan only crop PFTs */
               int npft,            /**< number of natural PFTs */
               int ncft,            /**< number of crop PFTs */
               const Config *config /**< LPJ configuration */
              )                     /** \return CFT map array or NULL on error */
{
  Bool *undef;
  int *cftmap;
  int cft;
  Bool first=TRUE;
  cftmap=newvec(int,getmapsize(map));
  if(cftmap==NULL)
  {
    printallocerr("cftmap");
    return NULL;
  }
  undef=newvec(Bool,cftonly ? ncft : getnirrig(ncft,config));
  if(undef==NULL)
  {
    printallocerr("undef");
    return NULL;
  }
  if(cftonly)
    for(cft=0;cft<ncft;cft++)
      undef[cft]=TRUE;
  else
    for(cft=0;cft<getnirrig(ncft,config);cft++)
      undef[cft]=TRUE;
  for(cft=0;cft<getmapsize(map);cft++)
  {
    if(getmapitem(map,cft)==NULL)
    {
      cftmap[cft]=NOT_FOUND;
      continue;
    }
    cftmap[cft]=findpftname(getmapitem(map,cft),config->pftpar+npft,ncft);
    if(cftmap[cft]!=NOT_FOUND)
    {
      undef[cftmap[cft]]=FALSE;
      continue;
    }
    if(!cftonly)
    {
      cftmap[cft]=findstr(getmapitem(map,cft),grassland_names,NGRASS);
      if(cftmap[cft]!=NOT_FOUND)
      {
        cftmap[cft]+=ncft;
        undef[cftmap[cft]]=FALSE;
        continue;
      }
      cftmap[cft]=findstr(getmapitem(map,cft),biomass_names,NBIOMASSTYPE);
      if(cftmap[cft]!=NOT_FOUND)
      {
        cftmap[cft]+=ncft+NGRASS;
        undef[cftmap[cft]]=FALSE;
        continue;
      }
      if(config->nwptype)
      {
        cftmap[cft]=findstr(getmapitem(map,cft),woodplantation_names,config->nwptype);
        if(cftmap[cft]!=NOT_FOUND)
        {
          cftmap[cft]+=ncft+NGRASS+NBIOMASSTYPE;
          undef[cftmap[cft]]=FALSE;
          continue;
        }
      }
      if(config->nagtree)
      {
        cftmap[cft]=findpftname(getmapitem(map,cft),config->pftpar+npft-config->nagtree,config->nagtree);
        if(cftmap[cft]!=NOT_FOUND)
        {
        cftmap[cft]+=ncft+NGRASS+NBIOMASSTYPE+config->nwptype;
          undef[cftmap[cft]]=FALSE;
          continue;
        }
      }
    }
    if(cftmap[cft]==NOT_FOUND)
    {
      if(isroot(*config))
        fprintf(stderr,"WARNING011: Unknown CFT \"%s\" in map '%s', will be ignored.\n",(char *)getmapitem(map,cft),name);
    }
  } /* of for(cft=0...) */
  if(isroot(*config))
  {
    for(cft=0;cft<ncft;cft++)
      if(undef[cft])
      {
        if(first)
        {
          fprintf(stderr,"WARNING010: Map in '%s' not defined for",name);
          first=FALSE;
        }
        else
          fputc(',',stderr);
        fprintf(stderr," \"%s\"",config->pftpar[npft+cft].name);
      }
    if(!cftonly)
    {
      for(cft=0;cft<NGRASS;cft++)
        if(undef[cft+ncft])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: Map in '%s' not defined for",name);
            first=FALSE;
          }
          else
            fputc(',',stderr);
          fprintf(stderr," \"%s\"",grassland_names[cft]);
        }
      for(cft=0;cft<NBIOMASSTYPE;cft++)
        if(undef[cft+ncft+NGRASS])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: Map in '%s' not defined for",name);
            first=FALSE;
          }
          else
            fputc(',',stderr);
          fprintf(stderr," \"%s\"",biomass_names[cft]);
        }
      for(cft=0;cft<config->nwptype;cft++)
        if(undef[ncft+NGRASS+NBIOMASSTYPE+cft])
        {
          if(first && isroot(*config))
          {
            fprintf(stderr,"WARNING010: Map in '%s' not defined for",name);
            first=FALSE;
          }
          else
            fputc(',',stderr);
          fprintf(stderr," \"%s\"",woodplantation_names[cft]);
        }
      for(cft=0;cft<config->nagtree;cft++)
        if(undef[cft+ncft+NGRASS+NBIOMASSTYPE+config->nwptype])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: Map in '%s' not defined for",name);
            first=FALSE;
          }
          else
            fputc(',',stderr);
          fprintf(stderr," \"%s\"",config->pftpar[npft-config->nagtree+cft].name);
        }
    }
    if(!first)
      fprintf(stderr,", set to zero.\n");
  } /* of isroot(*config) */
  free(undef);
  return cftmap;
} /* of 'getcftmap' */
