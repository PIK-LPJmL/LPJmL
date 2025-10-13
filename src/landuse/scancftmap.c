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
                int *size,           /**< size of CFT map array or -1 on error */
                const char *name,    /**< name of map */
                Bool cftonly,        /**< scan only crop PFTs */
                Bool defaultmap,     /**< set default map if name not found */
                int npft,            /**< number of natural PFTs */
                int ncft,            /**< number of crop PFTs */
                const Config *config /**< LPJ configuration */
               )                     /** \return CFT map array or NULL */
{
  int *cftmap;
  Verbosity verbose;
  Map *map;
  if(iskeydefined(file,name))
  {
    verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
    map=fscanmap(file,name,verbose);
    if(map==NULL)
    {
      *size= -1;
      return NULL;
    }
    *size=getmapsize(map);
    cftmap=getcftmap(map,name,cftonly,npft,ncft,config);
    freemap(map);
  }
  else if(defaultmap)
    cftmap=defaultcftmap(size,name,cftonly,npft,ncft,config);
  else
  {
    *size=0;
    cftmap=NULL;
  }
  return cftmap;
} /* of 'scancftmap' */
