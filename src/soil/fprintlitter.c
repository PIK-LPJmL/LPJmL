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

void fprintlitter(FILE *file,           /**< pointer to file */
                  const Litter *litter, /**< pointer to litter */
                  int with_nitrogen     /**< nitrogen cycle enabled */
                 )                      /** \returns void */
{
  int i,p;
  Stocks sum,total;
  Trait trait_sum,trait_sum_sub;
  fprintf(file,"\n\tAvg. fbd:\t");
  for(i=0;i<NFUELCLASS+1;i++)
    fprintf(file,"%g ",litter->avg_fbd[i]);
  fprintf(file,"\n\tLitter carbon (gC/m2)\n"
               "\tPFT                                      below   leaf   ");
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," wood(%d)",i);
  fprintf(file," leaf   ");
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," wood(%d)",i);
  fprintf(file,"\n");
  fprintf(file,"\t---------------------------------------- ------- -------");
  for(i=0;i<2*NFUELCLASS+1;i++)
    fputs(" -------",file);
  fputc('\n',file);
  sum.carbon=sum.nitrogen=trait_sum.leaf.carbon=trait_sum.leaf.nitrogen=total.carbon=total.nitrogen=trait_sum_sub.leaf.carbon=trait_sum_sub.leaf.nitrogen=0;
  for(i=0;i<NFUELCLASS;i++)
     trait_sum.wood[i].carbon=trait_sum.wood[i].nitrogen=trait_sum_sub.wood[i].carbon=trait_sum_sub.wood[i].nitrogen=0;
  for(p=0;p<litter->n;p++)
  {
    fprintf(file,"\t%-40s %7.2f %7.2f",litter->item[p].pft->name,
            litter->item[p].bg.carbon,litter->item[p].ag.leaf.carbon);
    sum.carbon+=litter->item[p].bg.carbon;
    trait_sum.leaf.carbon+=litter->item[p].ag.leaf.carbon;
    for(i=0;i<NFUELCLASS;i++)
    {
      fprintf(file," %7.2f",litter->item[p].ag.wood[i].carbon);
      trait_sum.wood[i].carbon+=litter->item[p].ag.wood[i].carbon;
    }
    fprintf(file," %7.2f",litter->item[p].agsub.leaf.carbon);
    trait_sum_sub.leaf.carbon+=litter->item[p].agsub.leaf.carbon;
    for(i=0;i<NFUELCLASS;i++)
    {
      fprintf(file," %7.2f",litter->item[p].agsub.wood[i].carbon);
      trait_sum_sub.wood[i].carbon+=litter->item[p].agsub.wood[i].carbon;
    }
    fputc('\n',file);
  }
  total.carbon+=sum.carbon+trait_sum.leaf.carbon+trait_sum_sub.leaf.carbon;
  fprintf(file,"\t---------------------------------------- ------- -------");
  for(i=0;i<NFUELCLASS;i++)
  {
    total.carbon+=trait_sum.wood[i].carbon;
    fprintf(file," -------");
  }
  fprintf(file," -------");
  for(i=0;i<NFUELCLASS;i++)
  {
    total.carbon+=trait_sum_sub.wood[i].carbon;
    fprintf(file," -------");
  }
  fprintf(file,"\n\tTOTAL                            %7.2f %7.2f %7.2f",total.carbon,sum.carbon,trait_sum.leaf.carbon);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %7.2f",trait_sum.wood[i].carbon);
  fprintf(file," %7.2f",trait_sum_sub.leaf.carbon);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %7.2f",trait_sum_sub.wood[i].carbon);
  fputc('\n',file);
  if(with_nitrogen)
  {
    fprintf(file,"\tLitter nitrogen (gN/m2)\n"
                 "\tPFT                                      below   leaf   ");
    for(i=0;i<NFUELCLASS;i++)
      fprintf(file," wood(%d)",i);
    fprintf(file," leaf   ");
    for(i=0;i<NFUELCLASS;i++)
      fprintf(file," wood(%d)",i);
    fprintf(file,"\n");
    fprintf(file,"\t---------------------------------------- ------- -------");
    for(i=0;i<2*NFUELCLASS+1;i++)
      fprintf(file," -------");
    fprintf(file,"\n");
    for(p=0;p<litter->n;p++)
    {
      fprintf(file,"\t%-40s %7.2f %7.2f",litter->item[p].pft->name,
              litter->item[p].bg.nitrogen,litter->item[p].ag.leaf.nitrogen);
      sum.nitrogen+=litter->item[p].bg.nitrogen;
      trait_sum.leaf.nitrogen+=litter->item[p].ag.leaf.nitrogen;
      for(i=0;i<NFUELCLASS;i++)
      {
        fprintf(file," %7.2f",litter->item[p].ag.wood[i].nitrogen);
        trait_sum.wood[i].nitrogen+=litter->item[p].ag.wood[i].nitrogen;
      }
      fprintf(file," %7.2f",litter->item[p].agsub.leaf.nitrogen);
      trait_sum_sub.leaf.nitrogen+=litter->item[p].agsub.leaf.nitrogen;
      for(i=0;i<NFUELCLASS;i++)
      {
        fprintf(file," %7.2f",litter->item[p].agsub.wood[i].nitrogen);
        trait_sum_sub.wood[i].nitrogen+=litter->item[p].agsub.wood[i].nitrogen;
      }
      fprintf(file,"\n");
    }
    total.nitrogen+=sum.nitrogen+trait_sum.leaf.nitrogen+trait_sum_sub.leaf.nitrogen;
    fprintf(file,"\t---------------------------------------- ------- -------");
    for(i=0;i<NFUELCLASS;i++)
    {
      total.nitrogen+=trait_sum.wood[i].nitrogen;
      fprintf(file," -------");
    }
    fprintf(file," -------");
    for(i=0;i<NFUELCLASS;i++)
    {
      total.nitrogen+=trait_sum_sub.wood[i].nitrogen;
      fprintf(file," -------");
    }
    fprintf(file,"\n\tTOTAL                            %7.2f %7.2f %7.2f",total.nitrogen,sum.nitrogen,trait_sum.leaf.nitrogen);
    for(i=0;i<NFUELCLASS;i++)
      fprintf(file," %7.2f",trait_sum.wood[i].nitrogen);
    fprintf(file," %7.2f",trait_sum_sub.leaf.nitrogen);
    for(i=0;i<NFUELCLASS;i++)
      fprintf(file," %7.2f",trait_sum_sub.wood[i].nitrogen);
    fprintf(file,"\nagtop_wcap:\t%g\n",litter->agtop_wcap);
    fprintf(file,"agtop_moist:\t%g\n",litter->agtop_moist);
    fprintf(file,"agtop_cover:\t%g\n",litter->agtop_cover);
    fprintf(file,"agtop_temp:\t%g (deg C)\n",litter->agtop_temp);
  }
} /* of 'fprintlitter' */
