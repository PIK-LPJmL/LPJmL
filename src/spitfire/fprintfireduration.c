/**************************************************************************************/
/**                                                                                \n**/
/**           f  p  r  i  n  t  f  i  r  e  d  u  r  a  t  i  o  n  .  c           \n**/
/**                                                                                \n**/
/**     Function prints stand-specific maximum fire duration                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintfireduration(FILE *file,             /**< pointer to text file */
                        Standtype **standtypes, /**< pointer to stand type array */
                        int nstand              /**< number of stand types */
                       )
{
  int i;
  Bool first=TRUE;
  for(i=0;i<nstand;i++)
    if(standtypes[i]->fireduration[0]!=param.fireduration[0] ||
      (standtypes[i]->fireduration[1]!=param.fireduration[1]))
    {
      if(first)
      {
        fprintf(file,"fire durations:");
        first=FALSE;
      }
      else
        fputc(',',file);
      fprintf(file," '%s' = [%g,%g] (min)",
              standtypes[i]->name,standtypes[i]->fireduration[0],standtypes[i]->fireduration[1]);
    }
  fputc('\n',file);
  first=TRUE;
  for(i=0;i<nstand;i++)
    if(standtypes[i]->max_ndayfire!=param.max_ndayfire)
    {
      if(first)
      {
        fprintf(file,"Maximum fire days:");
        first=FALSE;
      }
      else
        fputc(',',file);
      fprintf(file," '%s' = %d (days)",
              standtypes[i]->name,standtypes[i]->max_ndayfire);
    }
  fputc('\n',file);
} /* of 'fprintfireduration' */
