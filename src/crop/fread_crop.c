/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  a  d  _  c  r  o  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads crop-specific variables from restart file                   \n**/
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

#define readreal(file,name,val) if(bstruct_readreal(file,#name,&val->name)) return TRUE
#define readint(file,name,val) if(bstruct_readint(file,#name,&val->name)) return TRUE
#define readstocks2(file,name,val) if(freadstocks(file,name,val)) return TRUE

static Bool freadcropphys2(Bstruct file,const char *name,Cropphys2 *crop)
{
  if(bstruct_readbeginstruct(file,name))
    return TRUE;
  readstocks2(file,"leaf",&crop->leaf);
  readstocks2(file,"root",&crop->root);
  readstocks2(file,"pool",&crop->pool);
  readstocks2(file,"so",&crop->so);
  return bstruct_readendstruct(file,name);
} /* of 'fread_sharvests' */

static Bool fread_sharvests(Bstruct file,const char *name,Separate_harvests *sh)
{
  if(bstruct_readbeginstruct(file,name))
      return TRUE;
  readreal(file,petsum,sh);
  readreal(file,evapsum,sh);
  readreal(file,transpsum,sh);
  readreal(file,intercsum,sh);
  readreal(file,precsum,sh);
  readreal(file,sradsum,sh);
  readreal(file,irrig_apply,sh);
  readreal(file,tempsum,sh);
  readreal(file,nirsum,sh);
  readreal(file,lgp,sh);
  readreal(file,runoffsum,sh);
  readreal(file,n2o_denitsum,sh);
  readreal(file,n2o_nitsum,sh);
  readreal(file,n2_emissum,sh);
  readreal(file,leachingsum,sh);
  readreal(file,c_emissum,sh);
  readreal(file,nfertsum,sh);
  readint(file,sdate,sh);
  readint(file,sowing_year,sh);
  return bstruct_readendstruct(file,name);
} /* of 'fread_sharvests' */

Bool fread_crop(Bstruct file,          /**< file pointer */
                Pft *pft,              /**< PFT data to be read */
                Bool separate_harvests /**< double harvest output enabled? */
               )                       /** \return TRUE on error */
{
  Pftcrop *crop;
  crop=new(Pftcrop);
  pft->data=crop;
  if(crop==NULL)
  {
    printallocerr("crop");
    return TRUE;
  }
  readreal(file,nlimit,pft);
  readint(file,wtype,crop);
  readint(file,growingdays,crop);
  readreal(file,pvd,crop);
  readreal(file,phu,crop);
  readreal(file,basetemp,crop);
  readint(file,senescence,crop);
  readint(file,senescence0,crop);
  readreal(file,husum,crop);
  readreal(file,vdsum,crop);
  readreal(file,fphu,crop);
  if(freadcropphys2(file,"ind",&crop->ind))
    return TRUE;
  readreal(file,flaimax,crop);
  readreal(file,lai,crop);
  readreal(file,lai000,crop);
  readreal(file,laimax_adjusted,crop);
  readreal(file,lai_nppdeficit,crop);
  readreal(file,demandsum,crop);
  readreal(file,ndemandsum,crop);
  readreal(file,nuptakesum,crop);
  readreal(file,nfertilizer,crop);
  readreal(file,nmanure,crop);
  readreal(file,vscal_sum,crop);
  if(pft->stand->type->landusetype==AGRICULTURE && separate_harvests)
  {
    crop->sh=new(Separate_harvests);
    if(crop->sh==NULL)
    {
      printallocerr("crop");
      free(crop);
      return TRUE;
    }
    if(fread_sharvests(file,"sh",crop->sh))
    {
      fprintf(stderr,"ERROR254: Cannot read separate harvests for '%s'.\n",
              pft->par->name);
      return TRUE;
    }
  }
  else
    crop->sh=NULL;
  return bstruct_readreal(file,"supplysum",&crop->supplysum);
} /* of 'fread_crop' */
