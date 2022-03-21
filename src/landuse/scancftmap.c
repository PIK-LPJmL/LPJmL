/**************************************************************************************/
/**                                                                                \n**/
/**                    s  c  a  n  c  f  t  m  a  p  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads landuse mapping from LPJ configuration file                 \n**/
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

Bool findcftmap(const char *name,       /**< PFT name to find in landuse map */
                const Pftpar *pftpar,   /**< PFT parameter array */
                const int landusemap[], /**< landuse map array */
                int size                /**< size of landuse map araay */
               )                        /** \return whether  PFT name was found (TRUE/FALSE) */
{
  int cft;
  for(cft=0;cft<size;cft++)
    if(landusemap[cft]!=NOT_FOUND && !strcmp(name,pftpar[landusemap[cft]].name))
      return TRUE;
  return FALSE;
} /* of 'findcftmap' */

int findpftname(const char *name,     /**< PFT name to find in array */
                const Pftpar *pftpar, /**< PFT parameter array */
                int ncft              /**< size of PFT array */
               )                      /** \return index in PFT array or NOT_FOUND */
{
  int cft;
  for(cft=0;cft<ncft;cft++)
    if(!strcmp(name,pftpar[cft].name))
      return cft;
  return NOT_FOUND;
} /* of 'findpftname' */

int *scancftmap(LPJfile *file,       /**< pointer to LPJ config file */
                int *size,           /**< size of CFT map array */
                const char *name,    /**< name of map */
                Bool cftonly,        /**< scan only crop PFTs */
                int npft,            /**< number of natural PFTs */
                int ncft,            /**< number of crop PFTs */
                const Config *config /**< LPJ configuration */
               )                     /** \return CFT map array or NULL on error */
{
  Bool *undef;
  LPJfile *array,*item;
  int *cftmap;
  Verbosity verbose;
  const char *s;
  int cft;
  Bool first=TRUE;
  if(iskeydefined(file,name))
  {
    verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
    array=fscanarray(file,size,name,verbose);
    if(array==NULL)
      return NULL;
    cftmap=newvec(int,*size);
    if(cftmap==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    undef=newvec(Bool,cftonly ? ncft : getnirrig(ncft,config));
    if(undef==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    if(cftonly)
      for(cft=0;cft<ncft;cft++)
        undef[cft]=TRUE;
    else
      for(cft=0;cft<getnirrig(ncft,config);cft++)
        undef[cft]=TRUE;
    for(cft=0;cft<*size;cft++)
    {
      item=fscanarrayindex(array,cft);
      if(isnull(item))
      {
        cftmap[cft]=NOT_FOUND;
        continue;
      }
      if(!isstring(item,NULL))
      {
        if(verbose)
          fprintf(stderr,"ERROR226: Datatype of element %d in map '%s' is not of type string.\n",
                  cft+1,name);
        free(cftmap);
        return NULL;
      }
      s=fscanstring(item,NULL,NULL,verbose);
      if(s==NULL)
      {
        free(cftmap);
        return NULL;
      }
      cftmap[cft]=findpftname(s,config->pftpar+npft,ncft);
      if(cftmap[cft]!=NOT_FOUND)
      {
        undef[cftmap[cft]]=FALSE;
        continue;
      }
      if(!cftonly)
      {
        cftmap[cft]=findstr(s,grassland_names,NGRASS);
        if(cftmap[cft]!=NOT_FOUND)
        {
          cftmap[cft]+=ncft;
          undef[cftmap[cft]]=FALSE;
          continue;
        }
        cftmap[cft]=findstr(s,biomass_names,NBIOMASSTYPE);
        if(cftmap[cft]!=NOT_FOUND)
        {
          cftmap[cft]+=ncft+NGRASS;
          undef[cftmap[cft]]=FALSE;
          continue;
        }
        if(config->nwptype)
        {
          cftmap[cft]=findstr(s,woodplantation_names,config->nwptype);
          if(cftmap[cft]!=NOT_FOUND)
          {
            cftmap[cft]+=ncft+NGRASS+NBIOMASSTYPE;
            undef[cftmap[cft]]=FALSE;
            continue;
          }
        }
        if(config->nagtree)
        {
          cftmap[cft]=findpftname(s,config->pftpar+npft-config->nagtree,config->nagtree);
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
        if(verbose)
          fprintf(stderr,"WARNING011: Unknown CFT \"%s\" in map '%s', will be ignored.\n",s,name);
      }
    } /* of for(cft=0...) */
    if(isroot(*config))
    {
      for(cft=0;cft<ncft;cft++)
        if(undef[cft])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: Map '%s' not defined for",name);
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
              fprintf(stderr,"WARNING010: Map '%s' not defined for",name);
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
              fprintf(stderr,"WARNING010: Map '%s' not defined for",name);
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
              fprintf(stderr,"WARNING010: Map '%s' not defined for",name);
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
              fprintf(stderr,"WARNING010: Map '%s' not defined for",name);
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
  }
  else
  {
    /* no map defined, set default one */
    *size=(cftonly) ? ncft : getnirrig(ncft,config);
    cftmap=newvec(int,*size);
    if(cftmap==NULL)
    {
      printallocerr("cftmap");
      return NULL;
    }
    for(cft=0;cft<*size;cft++)
      cftmap[cft]=cft;
    if(isroot(*config))
    {
      fprintf(stderr,"WARNING011: Map '%s' not found, set to [",name);
      fprintpftnames(stderr,config->pftpar+npft,ncft);
      if(!cftonly)
      {
        for(cft=0;cft<NGRASS;cft++)
          fprintf(stderr,",\"%s\"",grassland_names[cft]);
        for(cft=0;cft<NBIOMASSTYPE;cft++)
          fprintf(stderr,",\"%s\"",biomass_names[cft]);
        for(cft=0;cft<config->nwft;cft++)
          fprintf(stderr,",\"%s\"",woodplantation_names[cft]);
        for(cft=0;cft<config->nagtree;cft++)
          fprintf(stderr,",\"%s\"",config->pftpar[npft-config->nagtree+cft].name);
      }
      fputs("].\n",stderr);
    }
  }
  return cftmap;
} /* of 'scancftmap' */
