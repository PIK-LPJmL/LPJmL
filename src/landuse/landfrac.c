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

/* define a tiny fraction for allcrops that is always at least 10x epsilon */

Landfrac *newlandfrac(int ncft,   /**< number of crop PFTs */
                      int nagtree /**< number of agriculture tree PFTs */
                     )            /** \return allocated landfrac or NULL */
{
 int i;
 Landfrac *landfrac;
 landfrac=newvec(Landfrac,2);
 if(landfrac==NULL)
   return NULL;
 for(i=0;i<2;i++)
 {
   landfrac[i].crop=newvec(Real,ncft);
   if(landfrac[i].crop==NULL)
   {
     free(landfrac);
     return NULL;
   }
   landfrac[i].ag_tree=newvec(Real,nagtree);
   if(landfrac[i].ag_tree==NULL)
   {
     free(landfrac);
     return NULL;
   }
 }
 return landfrac;
} /* of 'newlandfrac' */

void initlandfracitem(Landfrac *landfrac, /**< land fractions */
                      int ncft,           /**< number of crop PFTs */
                      int nagtree         /**< number of agriculture tree PFTs */
                     )
{
  int j;
  for(j=0;j<ncft;j++)
    landfrac->crop[j]=0;
  for(j=0;j<nagtree;j++)
    landfrac->ag_tree[j]=0;
  for(j=0;j<NGRASS;j++)
    landfrac->grass[j]=0;
  landfrac->biomass_grass=landfrac->biomass_tree=0;
  landfrac->woodplantation=0;
} /* of 'initlandfracitem' */

void initlandfrac(Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                  int ncft,             /**< number of crop PFTs */
                  int nagtree           /**< number of agriculture tree PFTs */
                 )
{
  int i;
  for(i=0;i<2;i++)
    initlandfracitem(landfrac+i,ncft,nagtree);
} /* of 'initlandfrac' */

void scalelandfrac(Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                   int ncft,             /**< number of crop PFTs */
                   int nagtree,          /**< number of agriculture tree PFTs */
                   Real scale            /**< scaling factor (0..11) */
                  )
{
  int i,j;
  for(i=0;i<2;i++)
  {
    for(j=0;j<ncft;j++)
      landfrac[i].crop[j]*=scale;
    for(j=0;j<nagtree;j++)
      landfrac[i].ag_tree[j]*=scale;
    for(j=0;j<NGRASS;j++)
      landfrac[i].grass[j]*=scale;
    landfrac[i].biomass_grass*=scale;
    landfrac[i].biomass_tree*=scale;
    landfrac[i].woodplantation*=scale;
  }
} /* of 'scalelandfrac' */

void freelandfrac(Landfrac *landfrac /**< land fractions (non-irrig., irrig.) */
                 )
{
  if(landfrac!=NULL)
  {
    free(landfrac[0].crop);
    free(landfrac[1].crop);
    free(landfrac[0].ag_tree);
    free(landfrac[1].ag_tree);
    free(landfrac);
  }
} /* of 'freelandfrac' */

Bool fwritelandfrac(FILE *file,                 /**< pointer to binary file */
                    const Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                    int ncft,                   /**< number of crop PFTs */
                    int nagtree                 /**< number of agriculture tree PFTs */
                   )                            /** \return TRUE on error */
{
  int i;
  for(i=0;i<2;i++)
  {
    fwrite(landfrac[i].crop,sizeof(Real),ncft,file);
    fwrite(landfrac[i].ag_tree,sizeof(Real),nagtree,file);
    fwrite(landfrac[i].grass,sizeof(Real),NGRASS,file);
    fwrite(&landfrac[i].woodplantation,sizeof(Real),1,file);
    fwrite(&landfrac[i].biomass_grass,sizeof(Real),1,file);
    if(fwrite(&landfrac[i].biomass_tree,sizeof(Real),1,file)!=1)
      return TRUE;

  }
  return FALSE;
} /* of 'fwritelandfrac' */

void fprintlandfrac(FILE *file,               /**< pointer to text file */
                    const Landfrac *landfrac, /**< land fractions */
                    int ncft,                 /**< number of crop PFTs */
                    int nagtree               /**< number of agriculture tree PFTs */
                   )
{
  int i;
  for(i=0;i<ncft;i++)
    fprintf(file," %g",landfrac->crop[i]);
  for(i=0;i<nagtree;i++)
    fprintf(file," %g",landfrac->ag_tree[i]);
  for(i=0;i<NGRASS;i++)
    fprintf(file," %g",landfrac->grass[i]);
  fprintf(file," %g",landfrac->woodplantation);
  fprintf(file," %g",landfrac->biomass_grass);
  fprintf(file," %g",landfrac->biomass_tree);
} /* of 'fprintlandfrac' */

Bool freadlandfrac(FILE *file,           /**< pointer to binary file */
                   Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                   int ncft,             /**< number of crop PFTs */
                   int nagtree,          /**< number of agriculture tree PFTs */
                   Bool swap             /**< byte order has to be swapped (TRUE/FALSE) */
                  )                      /** \return TRUE on error */
{
  int i;
  for(i=0;i<2;i++)
  {
    freadreal(landfrac[i].crop,ncft,swap,file);
    freadreal(landfrac[i].ag_tree,nagtree,swap,file);
    freadreal(landfrac[i].grass,NGRASS,swap,file);
    freadreal(&landfrac[i].woodplantation,1,swap,file);
    freadreal(&landfrac[i].biomass_grass,1,swap,file);
    if(freadreal(&landfrac[i].biomass_tree,1,swap,file)!=1)
      return TRUE;
  }
  return FALSE;
} /* of 'freadlandfrac' */

Real landfrac_sum(const Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                  int ncft,                   /**< number of crop PFTs */
                  int nagtree,                /**< number of agriculture tree PFTs */
                  Bool irrig                  /**< irrigated fraction? (TRUE/FALSE) */
                 )                            /** \return sum of non-irrigated/irrigated fraction */
{
  Real sum;
  int j;
  sum=0;
  for(j=0;j<ncft;j++)
    sum+=landfrac[irrig].crop[j];
  for(j=0;j<nagtree;j++)
    sum+=landfrac[irrig].ag_tree[j];
  for(j=0;j<NGRASS;j++)
    sum+=landfrac[irrig].grass[j];
  sum+=landfrac[irrig].biomass_grass;
  sum+=landfrac[irrig].biomass_tree;
  sum+=landfrac[irrig].woodplantation;
  return sum;
} /* of 'landfrac_sum' */

Bool readlandfracmap(Landfrac *landfrac, /**< land fractions */
                    const int map[],     /**< map from bands to CFTs */
                    int size,            /**< size of map */
                    const Real data[],   /**< data from land-use file */
                    int *count,          /**< index in data array */
                    int ncft,            /**< number of crop PFTs */
                    int nwpt             /**< number of woodplantations */
                   )                     /** \return TRUE on error */
{
  int i;
  for(i=0;i<size;i++)
  {
    if(data[*count]<0)
      return TRUE;
    if(map[i]==NOT_FOUND)
      (*count)++; /* ignore data */
    else if(map[i]<ncft)
      landfrac->crop[map[i]]+=data[(*count)++];
    else if(map[i]<ncft+NGRASS)
      landfrac->grass[map[i]-ncft]+=data[(*count)++];
    else if(map[i]==ncft+NGRASS)
      landfrac->biomass_grass+=data[(*count)++];
    else if(map[i]==ncft+NGRASS+1)
      landfrac->biomass_tree+=data[(*count)++];
    else if(nwpt && map[i]==ncft+NGRASS+NBIOMASSTYPE)
      landfrac->woodplantation+=data[(*count)++];
    else
      landfrac->ag_tree[map[i]-ncft-NGRASS-NBIOMASSTYPE-nwpt]+=data[(*count)++];
  }
  return FALSE;
} /* of 'readlandfracmap*/
