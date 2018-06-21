/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  l  i  t  t  e  r  .  c                        \n**/
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

void fprintlitter(FILE *file,          /**< pointer to file */
                  const Litter *litter /**< pointer to litter */
                  )                    /** \returns void */
{
  int i,p;
  Real sum=0;
  Real total;
  Trait trait_sum;
  fputs("\n\tAvg. fbd:\t",file);
  for(i=0;i<NFUELCLASS+1;i++)
    fprintf(file,"%g ",litter->avg_fbd[i]);
  trait_sum.leaf=0;
  for(i=0;i<NFUELCLASS;i++)
    trait_sum.wood[i]=0;
  fputs("\n\tLitter carbon (gC/m2)\n"
        "\tPFT                                      below   leaf   ",file);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," wood(%d)",i);
  fputs("\n\t---------------------------------------- ------- -------",file);
  for(i=0;i<NFUELCLASS;i++)
    fputs(" -------",file);
  fputc('\n',file);
  for(p=0;p<litter->n;p++)
  {
    fprintf(file,"\t%-40s %7.2f %7.2f",litter->ag[p].pft->name,
            litter->bg[p],litter->ag[p].trait.leaf);
    sum+=litter->bg[p];
    trait_sum.leaf+=litter->ag[p].trait.leaf;
    for(i=0;i<NFUELCLASS;i++)
    {
      fprintf(file," %7.2f",litter->ag[p].trait.wood[i]);
      trait_sum.wood[i]+=litter->ag[p].trait.wood[i];
    }
    fputc('\n',file);
  }
  fputs("\t---------------------------------------- ------- -------",file);
  total=sum+trait_sum.leaf;
  for(i=0;i<NFUELCLASS;i++)
  {
    fputs(" -------",file);
    total+=trait_sum.wood[i];
  }
  fprintf(file,"\n\tTOTAL                            %7.2f %7.2f %7.2f",
          total,sum,trait_sum.leaf);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %7.2f",trait_sum.wood[i]);
} /* of 'fprintlitter' */
