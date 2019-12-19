/**************************************************************************************/
/**                                                                                \n**/
/**            f  p  r  i  n  t  o  u  t  p  u  t  v  a  r  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints name, description and units of output variables            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintoutputvar(FILE *file,const Variable output[],int size)
{
  int i,width,width_unit,width_var;
  width=strlen("Name");
  width_unit=strlen("Unit");
  width_var=strlen("Variable");
  for(i=0;i<size;i++)
  {
    width=max(width,strlen(output[i].name));
    width_unit=max(width_unit,strlen(output[i].unit));
    width_var=max(width_var,strlen(output[i].var));
  }
  fprintf(file,"Output files\n"
          "%*s %*s %*s Type  Description\n",width,"Name",width_var,"Variable",width_unit,"Unit");
  frepeatch(file,'-',width);
  fputc(' ',file);
  frepeatch(file,'-',width_var);
  fputc(' ',file);
  frepeatch(file,'-',width_unit);
  fputs(" ----- ",file);
  frepeatch(file,'-',77-width-width_unit-width_var-7);
  putc('\n',file);
  for(i=0;i<size;i++)
   fprintf(file,"%*s %*s %*s %5s %s\n",width,output[i].name,width_var,output[i].var,
              width_unit,strlen(output[i].unit)==0 ? "-" : output[i].unit,
              typenames[getoutputtype(i)],output[i].descr);
  frepeatch(file,'-',width);
  fputc(' ',file);
  frepeatch(file,'-',width_var);
  fputc(' ',file);
  frepeatch(file,'-',width_unit);
  fputs(" ----- ",file);
  frepeatch(file,'-',77-width-width_unit-width_var-7);
  putc('\n',file);
} /* of 'fprintoutputvar' */
