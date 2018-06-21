/**************************************************************************************/
/**                                                                                \n**/
/**                 l  a  n  d  f  r  a  c  .  c                                   \n**/
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

void newlandfrac(Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                 int ncft              /**< number of crop PFTs */
                )
{
 landfrac[0].crop=newvec(Real,ncft);
 check(landfrac[0].crop);
 landfrac[1].crop=newvec(Real,ncft);
 check(landfrac[1].crop);
} /* of 'newlandfrac' */

void initlandfrac(Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                  int ncft              /**< number of crop PFTs */
                 )
{
  int i,j;
  for(i=0;i<2;i++)
  {
    for(j=0;j<ncft;j++)
      landfrac[i].crop[j]=0;
    for(j=0;j<NGRASS;j++)
      landfrac[i].grass[j]=0;
    landfrac[i].biomass_grass=landfrac[i].biomass_tree=0;
  }
} /* of 'initlandfrac' */

void scalelandfrac(Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                   int ncft,             /**< number of crop PFTs */
                   Real scale            /**< scaling factor (0..11) */
                  )
{
  int i,j;
  for(i=0;i<2;i++)
  {
    for(j=0;j<ncft;j++)
      landfrac[i].crop[j]*=scale;
    for(j=0;j<NGRASS;j++)
      landfrac[i].grass[j]*=scale;
    landfrac[i].biomass_grass*=scale;
    landfrac[i].biomass_tree*=scale;
  }
} /* of 'scalelandfrac' */

void freelandfrac(Landfrac landfrac[2] /**< land fractions (non-irrig., irrig.) */
                 )
{
  free(landfrac[0].crop);
  free(landfrac[1].crop);
} /* of 'freelandfrac' */

Bool fwritelandfrac(FILE *file,                 /**< pointer to binary file */
                    const Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                    int ncft                    /**< number of crop PFTs */
                   )                            /** \return TRUE on error */
{
  int i;
  for(i=0;i<2;i++)
  {
    fwrite(landfrac[i].crop,sizeof(Real),ncft,file);
    fwrite(landfrac[i].grass,sizeof(Real),NGRASS,file);
    fwrite(&landfrac[i].biomass_grass,sizeof(Real),1,file);
    if(fwrite(&landfrac[i].biomass_tree,sizeof(Real),1,file)!=1)
      return TRUE;
  }
  return FALSE;
} /* of 'fwritelandfrac' */

Bool freadlandfrac(FILE *file,           /**< pointer to binary file */
                   Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                   int ncft,             /**< number of crop PFTs */
                   Bool swap             /**< byte order has to be swapped (TRUE/FALSE) */
                  )                      /** \return TRUE on error */
{
  int i;
  for(i=0;i<2;i++)
  {
    freadreal(landfrac[i].crop,ncft,swap,file);
    freadreal(landfrac[i].grass,NGRASS,swap,file);
    freadreal(&landfrac[i].biomass_grass,1,swap,file);
    if(freadreal(&landfrac[i].biomass_tree,1,swap,file)!=1)
      return TRUE;
  }
  return FALSE;
} /* of 'freadlandfrac' */

Real landfrac_sum(const Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                  int ncft,                   /**< number of crop PFTs */
                  Bool irrig                  /**< irrigated fraction? (TRUE/FALSE) */
                 )                            /** \return sum of non-irrigated/irrigated fraction */ 
{
  Real sum;
  int j;
  sum=0;
  for(j=0;j<ncft;j++)
    sum+=landfrac[irrig].crop[j];
  for(j=0;j<NGRASS;j++)
    sum+=landfrac[irrig].grass[j];
  sum+=landfrac[irrig].biomass_grass;
  sum+=landfrac[irrig].biomass_tree;

  return sum;
} /* of 'landfrac_sum' */
