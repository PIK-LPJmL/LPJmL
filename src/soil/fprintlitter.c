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
  Stocks sum,total;
  Trait trait_sum;
  fprintf(file,"\n\tAvg. fbd:\t");
  for(i=0;i<NFUELCLASS+1;i++)
    fprintf(file,"%g ",litter->avg_fbd[i]);
  fprintf(file,"\n\tLitter carbon (gC/m2)\n"
               "\tPFT                                      below   leaf   ");
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," wood(%d)",i);
  fprintf(file,"\n");
  fprintf(file,"\t---------------------------------------- ------- -------");
  for(i=0;i<NFUELCLASS;i++)
    fputs(" -------",file);
  fputc('\n',file);
  sum.carbon=sum.nitrogen=trait_sum.leaf.carbon=trait_sum.leaf.nitrogen=total.carbon=total.nitrogen=0;
  for(i=0;i<NFUELCLASS;i++)
     trait_sum.wood[i].carbon=trait_sum.wood[i].nitrogen=0; 
  for(p=0;p<litter->n;p++)
  {
    fprintf(file,"\t%-40s %7.2f %7.2f",litter->ag[p].pft->name,
            litter->bg[p].carbon,litter->ag[p].trait.leaf.carbon);
    sum.carbon+=litter->bg[p].carbon;
    trait_sum.leaf.carbon+=litter->ag[p].trait.leaf.carbon;
    for(i=0;i<NFUELCLASS;i++)
    {
      fprintf(file," %7.2f",litter->ag[p].trait.wood[i].carbon);
      trait_sum.wood[i].carbon+=litter->ag[p].trait.wood[i].carbon;
    }
    fputc('\n',file);
  }
  total.carbon+=sum.carbon+trait_sum.leaf.carbon;
  fprintf(file,"\t---------------------------------------- ------- -------");
  for(i=0;i<NFUELCLASS;i++)
  {
    total.carbon+=trait_sum.wood[i].carbon;
    fprintf(file," -------");
  }
  fprintf(file,"\n\tTOTAL                            %7.2f %7.2f %7.2f",total.carbon,sum.carbon,trait_sum.leaf.carbon);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %7.2f",trait_sum.wood[i].carbon);
  fprintf(file,"\n\tLitter nitrogen (gN/m2)\n"
               "\tPFT                                      below   leaf   ");
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," wood(%d)",i);
  fprintf(file,"\n");
  fprintf(file,"\t---------------------------------------- ------- -------");
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," -------");
  fprintf(file,"\n");
  for(p=0;p<litter->n;p++)
  {
    fprintf(file,"\t%-40s %7.2f %7.2f",litter->ag[p].pft->name,
            litter->bg[p].nitrogen,litter->ag[p].trait.leaf.nitrogen);
    sum.nitrogen+=litter->bg[p].nitrogen;
    trait_sum.leaf.nitrogen+=litter->ag[p].trait.leaf.nitrogen;
    for(i=0;i<NFUELCLASS;i++)
    {
      fprintf(file," %7.2f",litter->ag[p].trait.wood[i].nitrogen);
      trait_sum.wood[i].nitrogen+=litter->ag[p].trait.wood[i].nitrogen;
    }
    fprintf(file,"\n");
  }
  total.nitrogen+=sum.nitrogen+trait_sum.leaf.nitrogen;
  fprintf(file,"\t---------------------------------------- ------- -------");
  for(i=0;i<NFUELCLASS;i++)
  {
    total.nitrogen+=trait_sum.wood[i].nitrogen;
    fprintf(file," -------");
  }
  fprintf(file,"\n\tTOTAL                            %7.2f %7.2f %7.2f",total.nitrogen,sum.nitrogen,trait_sum.leaf.nitrogen);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %7.2f",trait_sum.wood[i].nitrogen);
  fputc('\n',file);
} /* of 'fprintlitter' */
