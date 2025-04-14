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

#define readreal2(file,name,val) if(bstruct_readreal(file,#name,&val->name)) return TRUE
#define readint2(file,name,val) if(bstruct_readint(file,#name,&val->name)) return TRUE
#define readstocks2(file,name,val) if(freadstocks(file,name,val)) return TRUE

static Bool freadcropphys2(Bstruct file,const char *name,Cropphys2 *crop)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  readstocks2(file,"leaf",&crop->leaf);
  readstocks2(file,"root",&crop->root);
  readstocks2(file,"pool",&crop->pool);
  readstocks2(file,"so",&crop->so);
  return bstruct_readendstruct(file);
}

Bool fread_crop(Bstruct file,          /**< file pointer */
                Pft *pft,            /**< PFT data to be read */
                Bool separate_harvests /**< double harvest output enabled? */
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
  readreal2(file,nlimit,pft);
  readint2(file,wtype,crop);
  readint2(file,growingdays,crop);
  readreal2(file,pvd,crop);
  readreal2(file,phu,crop);
  readreal2(file,basetemp,crop);
  readint2(file,senescence,crop);
  readint2(file,senescence0,crop);
  readreal2(file,husum,crop);
  readreal2(file,vdsum,crop);
  readreal2(file,fphu,crop);
  if(freadcropphys2(file,"ind",&crop->ind))
    return TRUE;
  readreal2(file,flaimax,crop);
  readreal2(file,lai,crop);
  readreal2(file,lai000,crop);
  readreal2(file,laimax_adjusted,crop);
  readreal2(file,lai_nppdeficit,crop);
  readreal2(file,demandsum,crop);
  readreal2(file,ndemandsum,crop);
  readreal2(file,nuptakesum,crop);
  readreal2(file,nfertilizer,crop);
  readreal2(file,nmanure,crop);
  readreal2(file,vscal_sum,crop);
  if(pft->stand->type->landusetype==AGRICULTURE && separate_harvests)
  {
    crop->sh=new(Separate_harvests);
    if(crop->sh==NULL)
    {
      printallocerr("crop");
      free(crop);
      return TRUE;
    }
    if(bstruct_readstruct(file,"sh"))
      return TRUE;
    readreal2(file,petsum,crop->sh);
    readreal2(file,evapsum,crop->sh);
    readreal2(file,transpsum,crop->sh);
    readreal2(file,intercsum,crop->sh);
    readreal2(file,precsum,crop->sh);
    readreal2(file,sradsum,crop->sh);
    readreal2(file,irrig_apply,crop->sh);
    readreal2(file,tempsum,crop->sh);
    readreal2(file,nirsum,crop->sh);
    readreal2(file,lgp,crop->sh);
    readreal2(file,runoffsum,crop->sh);
    readreal2(file,n2o_denitsum,crop->sh);
    readreal2(file,n2o_nitsum,crop->sh);
    readreal2(file,n2_emissum,crop->sh);
    readreal2(file,leachingsum,crop->sh);
    readreal2(file,c_emissum,crop->sh);
    readreal2(file,nfertsum,crop->sh);
    readint2(file,sdate,crop->sh);
    readint2(file,sowing_year,crop->sh);
    if(bstruct_readendstruct(file))
      return TRUE;
  }
  else
    crop->sh=NULL;
  return bstruct_readreal(file,"supplysum",&crop->supplysum);
} /* of 'fread_crop' */
