/**************************************************************************************/
/**                                                                                \n**/
/**                 i n i t p r o d u c t i n i t . c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens file with initial product pool data                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef IMAGE

Productinit *initproductinit(const Config *config)
{
  Productinit *productinit;
  Header header;
  int version;
  String headername;
  size_t offset;
  if(config->prodpool_init_filename.fmt==CDF)
  {
    if(isroot(*config))
      fputs("ERROR211: NetCDF input currently not supported for product pools.\n",stderr);
    return NULL;
  }
  productinit=new(Productinit);
  if(productinit==NULL)
  {
    printallocerr("productinit");
    return NULL;
  } 
  if((productinit->file=openinputfile(&header,&productinit->swap,
                                      &config->prodpool_init_filename,
                                      headername,
                                      &version,&offset,config))==NULL)
  {
    free(productinit);
    return NULL;
  }
  productinit->firstyear=header.firstyear;
  productinit->nband=header.nbands;
  if(version<3)
    productinit->datatype=LPJ_FLOAT;
  else
    productinit->datatype=header.datatype;
  productinit->size=header.ncell*typesizes[productinit->datatype]*2;
  if(config->prodpool_init_filename.fmt==RAW)
    productinit->offset=sizeof(float)*config->startgrid*2;
  else
  {
    if(header.nbands!=2)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR218: Number of bands=%d in product pool file '%s' is not 2.\n",
                header.nbands,config->prodpool_init_filename.name);
      fclose(productinit->file);
      free(productinit);
      return NULL;
    }
    productinit->offset=headersize(headername,version)+typesizes[productinit->datatype]*(config->startgrid-header.firstcell)*2+offset;
  }
  productinit->scalar=header.scalar;
  return productinit;
} /* of 'initproductinit' */

void freeproductinit(Productinit *productinit)
{
  if(productinit!=NULL)
  {
    fclose(productinit->file);
    free(productinit);
  }
} /* of 'freeproductinit' */

#endif
