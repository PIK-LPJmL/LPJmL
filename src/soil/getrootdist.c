/**************************************************************************************/
/**                                                                                \n**/
/**                g  e  t  r  o  o  t  d  i  s  t  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void getrootdist(Real rootdist_n[],const Real rootdist[],Real mean_maxthaw)
{
  Real root_u,freeze_depth,layer,root_nu,thaw_depth;
  int i,l;
  root_u=layer=0;
  forrootsoillayer(l)
    rootdist_n[l]=rootdist[l];
  /*adjust root layer*/
  if(layerbound[BOTTOMLAYER]>mean_maxthaw &&
                mean_maxthaw>epsilon)
  {
    forrootsoillayer(l)
    {
      layer+=soildepth[l];
      root_u+=rootdist[l];
      freeze_depth=layer-mean_maxthaw;
      if (freeze_depth>0)
      {
        thaw_depth=soildepth[l]-freeze_depth;
        rootdist_n[l]=thaw_depth/soildepth[l]*rootdist[l];
        root_nu=rootdist[l]-rootdist_n[l];
        root_u-= root_nu;
        l++;
        break;
      }
      else 
      {
        thaw_depth=soildepth[l];
      }
    }
    for(i=l;i<BOTTOMLAYER;i++)
      rootdist_n[i]=0;
    layer=thaw_depth;
    for(i=l-2;i>=0;--i)
      layer+=soildepth[i];
    rootdist_n[l-1]+=rootdist_n[l-1]+(1-root_u)*thaw_depth/layer;
    /* rescale sum of rootdist_n to one */
    for(i=l-2;i>=0;--i)
      rootdist_n[i]+=(1-root_u)*soildepth[i]/layer;
  }
} /* of 'getrootdist' */
