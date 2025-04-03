/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  a  d  _  c  r  o  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads crop-specific variables from binary file                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

#define readreal2(file,name,val,swap) if(readreal(file,#name,&val->name,swap)) return TRUE
#define readint2(file,name,val,swap) if(readint(file,#name,&val->name,swap)) return TRUE
#define readstocks2(file,name,val,swap) if(readstocks(file,name,val,swap)) return TRUE

static Bool freadcropphys2(FILE *file,const char *name,Cropphys2 *crop,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  readstocks2(file,"leaf",&crop->leaf,swap);
  readstocks2(file,"root",&crop->root,swap);
  readstocks2(file,"pool",&crop->pool,swap);
  readstocks2(file,"so",&crop->so,swap);
  return readendstruct(file);
}

Bool fread_crop(FILE *file,          /**< file pointer */
                Pft *pft,            /**< PFT data to be read */
                Bool separate_harvests, /**< double harvest output enabled? */
                Bool swap            /**< if true data is in different byte order */
               )                     /** \return TRUE on error */
{
  Pftcrop *crop;
  crop=new(Pftcrop);
  pft->data=crop;
  if(crop==NULL)
  {
    printallocerr("crop");
    return TRUE;
  }
  readreal2(file,nlimit,pft,swap);
  readint2(file,wtype,crop,swap);
  readint2(file,growingdays,crop,swap);
  readreal2(file,pvd,crop,swap);
  readreal2(file,phu,crop,swap);
  readreal2(file,basetemp,crop,swap);
  readint2(file,senescence,crop,swap);
  readint2(file,senescence0,crop,swap);
  readreal2(file,husum,crop,swap);
  readreal2(file,vdsum,crop,swap);
  readreal2(file,fphu,crop,swap);
  if(freadcropphys2(file,"ind",&crop->ind,swap))
    return TRUE;
  readreal2(file,flaimax,crop,swap);
  readreal2(file,lai,crop,swap);
  readreal2(file,lai000,crop,swap);
  readreal2(file,laimax_adjusted,crop,swap);
  readreal2(file,lai_nppdeficit,crop,swap);
  readreal2(file,demandsum,crop,swap);
  readreal2(file,ndemandsum,crop,swap);
  readreal2(file,nuptakesum,crop,swap);
  readreal2(file,nfertilizer,crop,swap);
  readreal2(file,nmanure,crop,swap);
  readreal2(file,vscal_sum,crop,swap);
  if(pft->stand->type->landusetype==AGRICULTURE && separate_harvests)
  {
    crop->sh=new(Separate_harvests);
    if(crop->sh==NULL)
    {
      printallocerr("crop");
      free(crop);
      return TRUE;
    }
    if(readstruct(file,"sh",swap))
      return TRUE;
    readreal2(file,petsum,crop->sh,swap);
    readreal2(file,evapsum,crop->sh,swap);
    readreal2(file,transpsum,crop->sh,swap);
    readreal2(file,intercsum,crop->sh,swap);
    readreal2(file,precsum,crop->sh,swap);
    readreal2(file,sradsum,crop->sh,swap);
    readreal2(file,irrig_apply,crop->sh,swap);
    readreal2(file,tempsum,crop->sh,swap);
    readreal2(file,nirsum,crop->sh,swap);
    readreal2(file,lgp,crop->sh,swap);
    readreal2(file,runoffsum,crop->sh,swap);
    readreal2(file,n2o_denitsum,crop->sh,swap);
    readreal2(file,n2o_nitsum,crop->sh,swap);
    readreal2(file,n2_emissum,crop->sh,swap);
    readreal2(file,leachingsum,crop->sh,swap);
    readreal2(file,c_emissum,crop->sh,swap);
    readreal2(file,nfertsum,crop->sh,swap);
    readint2(file,sdate,crop->sh,swap);
    readint2(file,sowing_year,crop->sh,swap);
    if(readendstruct(file))
      return TRUE;
  }
  else
    crop->sh=NULL;
  return readreal(file,"supplysum",&crop->supplysum,swap);
} /* of 'fread_crop' */
