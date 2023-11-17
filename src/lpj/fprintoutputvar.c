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
                     int size,                /**< size of array */
                     int npft,                /**< number of natural PFTs */
                     int ncft,                /**< number of crop PFTs */
                     const Config *config     /**< LPJ configuration */
                    )
{
  int i,width,width_unit,width_var,index,width_standard_name;
  char *sc;
  Item *item;
  String s;
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
  width_standard_name=strlen("standard_name");
  for(i=0;i<size;i++)
  {
    width=max(width,strlen(output[i].name));
    width_unit=max(width_unit,strlen(output[i].unit));
    width_var=max(width_var,strlen(output[i].var));
    width_standard_name=max(width_standard_name,strlen(output[i].standard_name));
  }
  fprintf(file,"Output files available\n"
          "%-*s %-*s %-*s tstep nbd Type  Scale   Offset %-*s Long name\n",width,"Name",width_var,"Variable",width_unit,"Unit",width_standard_name,"Standard name");
  frepeatch(file,'-',width);
  fputc(' ',file);
  frepeatch(file,'-',width_var);
  fputc(' ',file);
  frepeatch(file,'-',width_unit);
  fputs(" ----- --- ----- ------- ------ ",file);
  frepeatch(file,'-',width_standard_name);
  fputc(' ',file);
  frepeatch(file,'-',77-width-width_unit-width_var-7-6-width_standard_name);
  putc('\n',file);
  for(i=0;i<size;i++)
  {
    index=(item==NULL) ? i : item[i].index;
    if(config->with_nitrogen || !isnitrogen_output(index))
    {
      switch(output[index].time)
      {
        case YEAR:
          sc="/y";
          break;
        case SECOND:
          sc="/s";
          break;
        case MONTH:
          sc="/m";
          break;
        case DAY:
          sc="/d";
          break;
        default:
          sc="";
      }
      fprintf(file,"%-*s %-*s %-*s %-5s %3d %5s %5g%-2s %6g %-*s %s\n",width,output[index].name,
              width_var,output[index].var,
              width_unit,strlen(output[index].unit)==0 ? "-" : output[index].unit,sprinttimestep(s,output[index].timestep),
              outputsize(index,npft,ncft,config),
              typenames[getoutputtype(index,config->grid_type)],output[index].scale,sc,output[index].offset,
              width_standard_name,output[index].standard_name,output[index].long_name);

    }
  }
  free(item);
  frepeatch(file,'-',width);
  fputc(' ',file);
  frepeatch(file,'-',width_var);
  fputc(' ',file);
  frepeatch(file,'-',width_unit);
  fputs(" ----- --- ----- ------- ------ ",file);
  frepeatch(file,'-',width_standard_name);
  fputc(' ',file);
  frepeatch(file,'-',77-width-width_unit-width_var-7-6-width_standard_name);
  putc('\n',file);
} /* of 'fprintoutputvar' */
