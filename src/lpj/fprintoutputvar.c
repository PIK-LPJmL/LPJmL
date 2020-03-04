/**************************************************************************************/
/**                                                                                \n**/
/**            f  p  r  i  n  t  o  u  t  p  u  t  v  a  r  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints name, description and units of all output variables        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

typedef struct
{
  char *name;
  int index;
} Item;

static int compare(const Item *a,const Item *b)
{
  return strcmp(a->name,b->name);
} /* of 'compare' */

void fprintoutputvar(FILE *file,              /**< pointer to text file */
                     const Variable output[], /**< array of output variables */
                     int size                 /**< size of array */
                    )
{
  int i,width,width_unit,width_var,index;
  Item *item;
  /* sort output alphabetically by name */
  item=newvec(Item,size);
  if(item!=NULL)
  {
    for(i=0;i<size;i++)
    {
      item[i].index=i;
      item[i].name=output[i].name;
    }
    qsort(item,size,sizeof(Item),(int (*)(const void *,const void *))compare);
  }
  width=strlen("Name");
  width_unit=strlen("Unit");
  width_var=strlen("Variable");
  for(i=0;i<size;i++)
  {
    width=max(width,strlen(output[i].name));
    width_unit=max(width_unit,strlen(output[i].unit));
    width_var=max(width_var,strlen(output[i].var));
  }
  fprintf(file,"Output files available\n"
          "%-*s %-*s %-*s Type  Scale Offset Description\n",width,"Name",width_var,"Variable",width_unit,"Unit");
  frepeatch(file,'-',width);
  fputc(' ',file);
  frepeatch(file,'-',width_var);
  fputc(' ',file);
  frepeatch(file,'-',width_unit);
  fputs(" ----- ----- ------ ",file);
  frepeatch(file,'-',77-width-width_unit-width_var-7);
  putc('\n',file);
  for(i=0;i<size;i++)
  {
   index=(item==NULL) ? i : item[i].index;
   fprintf(file,"%-*s %-*s %-*s %5s %5g %6g %s\n",width,output[index].name,
           width_var,output[index].var,
           width_unit,strlen(output[index].unit)==0 ? "-" : output[index].unit,
           typenames[getoutputtype(index)],output[index].scale,output[index].offset,output[index].descr);
  }
  free(item);
  frepeatch(file,'-',width);
  fputc(' ',file);
  frepeatch(file,'-',width_var);
  fputc(' ',file);
  frepeatch(file,'-',width_unit);
  fputs(" ----- ",file);
  frepeatch(file,'-',77-width-width_unit-width_var-7);
  putc('\n',file);
} /* of 'fprintoutputvar' */
