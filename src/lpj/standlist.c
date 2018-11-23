/**************************************************************************************/
/**                                                                                \n**/
/**                     s  t  a  n  d  l  i  s  t  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Implementation of list of stands                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "natural.h"

int fwritestandlist(FILE *file,                /**< pointer to binary file */
                    const Standlist standlist, /**< stand list */
                    int ntotpft                /**< total number of PFTs */
                   ) /** \return number of stands written */
{
  const Stand *stand;
  int s;
  fwrite(&standlist->n,sizeof(int),1,file);
  foreachstand(stand,s,standlist)
    if(fwritestand(file,stand,ntotpft))
      return s;
  return s;
} /* of 'fwritestandlist' */

void fprintstandlist(FILE *file,               /**< Pointer to text file */
                     const Standlist standlist /**< Stand list */
                    )
{
  const Stand *stand;
  int s;
  fprintf(file,"Number of stands:\t%d\n",standlist->n);
  foreachstand(stand,s,standlist)
  {
    fprintf(file,"Stand:\t\t%d\n",s);
    fprintstand(file,stand);
  }
} /* of 'fprintstandlist' */

Standlist freadstandlist(FILE *file,            /**< File pointer to binary file */
                         Cell *cell,            /**< Cell pointer */
                         const Pftpar pftpar[], /**< pft parameter array */
                         int ntotpft,           /**< total number of PFTs */
                         const Soilpar *soilpar,/**< soil parameter */
                         const Standtype standtype[],
                         int nstand,            /**< number of stand types */
                         Bool swap              /**< Byte order has to be changed */
                        ) /** \return allocated stand list or NULL */
{
  /* Function reads stand list from file */
  int s;
  Standlist standlist;
  /* Read number of stands */
  standlist=new(List);
  if(standlist==NULL)
  {
    printallocerr("standlist");
    return NULL;
  }
  if(freadint1(&standlist->n,swap,file)!=1)
  {
    free(standlist);
    return NULL;
  }
  standlist->data=newvec(void *,standlist->n);
  if(standlist->data==NULL)
  {
    printallocerr("standlist");
    free(standlist);
    return NULL;
  }
  /* Read all stand data */
  for(s=0;s<standlist->n;s++)
    if((standlist->data[s]=freadstand(file,cell,pftpar,ntotpft,soilpar,
                                      standtype,nstand,swap))==NULL)
      return NULL;
  return standlist;
} /* of 'freadstandlist' */

int addstand(const Standtype *type, /**< stand type */
             Cell *cell             /**< cell pointer */
            )                       /** \return new number of stands */
{
   /* Function adds stand to list */
   Stand *stand;
   stand=new(Stand);
   check(stand);
   addlistitem(cell->standlist,stand);
   stand->cell=cell;
   newpftlist(&stand->pftlist);
   newsoil(&stand->soil);
   stand->type=type;
   /* call stand-specific allocation function */
   initstand(stand);
   stand->type->newstand(stand);
   return getlistlen(cell->standlist);
} /* of 'addstand' */

void initstand(Stand *stand /**< Pointer to stand */
              )
{
  /* Function initializes stand */
  stand->fire_sum=0.0;
  stand->growing_time=stand->growing_days=stand->age=0;
  stand->prescribe_landcover=NO_LANDCOVER;
  stand->landcover=NULL;
} /* of 'initstand' */

void freestand(Stand *stand /**< Pointer to stand */
              )
{
  freepftlist(&stand->pftlist); /* free list of established PFTs */
  freesoil(&stand->soil);
  /* call stand-specific free function */
  stand->type->freestand(stand);
  free(stand);
} /* of 'freestand'  */
 
int delstand(Standlist list, /**< stand list */
             int index       /**< index of stand to be deleted */
            )                /** \return new number of stands */
{
  /* Function deletes stand from list */
  Stand *stand;
  stand=getstand(list,index);
  freestand(stand);
  return dellistitem(list,index);
} /* of 'delstand' */

void freestandlist(Standlist standlist /**< stand list */
                  )
{
  int s;
  Stand *stand;
  foreachstand(stand,s,standlist)
    freestand(stand);
  freelist(standlist);
} /* of 'freestandlist' */
