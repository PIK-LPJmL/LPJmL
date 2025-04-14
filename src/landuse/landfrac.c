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

Bool fwritelandfrac(Bstruct file,               /**< pointer to restart file */
                    const char *name,           /**< name of object */
                    const Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                    int ncft,                   /**< number of crop PFTs */
                    int nagtree                 /**< number of agriculture tree PFTs */
                   )                            /** \return TRUE on error */
{
  int i;
  bstruct_writearray(file,name,2);
  for(i=0;i<2;i++)
  {
    bstruct_writestruct(file,NULL);
    bstruct_writerealarray(file,"crop",landfrac[i].crop,ncft);
    bstruct_writerealarray(file,"ag_tree",landfrac[i].ag_tree,nagtree);
    bstruct_writerealarray(file,"grass",landfrac[i].grass,NGRASS);
    bstruct_writereal(file,"woodplantation",landfrac[i].woodplantation);
    bstruct_writereal(file,"biomass_grass",landfrac[i].biomass_grass);
    bstruct_writereal(file,"biomass_tree",landfrac[i].biomass_tree);
    bstruct_writeendstruct(file);
  }
  return bstruct_writeendarray(file);
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

Bool freadlandfrac(Bstruct file,         /**< pointer to restart file */
                   const char *name,     /**< name of object */
                   Landfrac landfrac[2], /**< land fractions (non-irrig., irrig.) */
                   int ncft,             /**< number of crop PFTs */
                   int nagtree           /**< number of agriculture tree PFTs */
                  )                      /** \return TRUE on error */
{
  int size,i;
  if(bstruct_readarray(file,name,&size))
    return TRUE;
  if(size!=2)
  {
    fprintf(stderr,"ERROR227: Size of %s=%d is not 2.\n",
            name,size);
    return TRUE;
  }
  for(i=0;i<2;i++)
  {
    if(bstruct_readstruct(file,NULL))
      return TRUE;
    if(bstruct_readrealarray(file,"crop",landfrac[i].crop,ncft))
      return TRUE;
    if(bstruct_readrealarray(file,"ag_tree",landfrac[i].ag_tree,nagtree))
      return TRUE;
    if(bstruct_readrealarray(file,"grass",landfrac[i].grass,NGRASS))
      return TRUE;
    if(bstruct_readreal(file,"woodplantation",&landfrac[i].woodplantation))
      return TRUE;
    if(bstruct_readreal(file,"biomass_grass",&landfrac[i].biomass_grass))
      return TRUE;
    if(bstruct_readreal(file,"biomass_tree",&landfrac[i].biomass_tree))
      return TRUE;
    if(bstruct_readendstruct(file))
      return TRUE;
  }
  return bstruct_readendarray(file);
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
