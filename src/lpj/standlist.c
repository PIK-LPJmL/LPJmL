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

Bool fwritestandlist(Bstruct file,              /**< pointer to restart file */
                     const char *key,           /**< name of object */
                     const Standlist standlist, /**< stand list */
                     int ntotpft                /**< total number of PFTs */
                    ) /** \return TRUE on error */
{
  const Stand *stand;
  int s;
  bstruct_writebeginarray(file,key,standlist->n);
  foreachstand(stand,s,standlist)
    if(fwritestand(file,NULL,stand,ntotpft))
      return TRUE;
  return bstruct_writeendarray(file);
} /* of 'fwritestandlist' */

void fprintstandlist(FILE *file,                /**< Pointer to text file */
                     const Standlist standlist, /**< Stand list */
                     const Pftpar *pftpar,      /**< PFT parameter array */
                     int ntotpft                /**< total number of PFTs */
                    )
{
  const Stand *stand;
  int s;
  fprintf(file,"Number of stands:\t%d\n",standlist->n);
  foreachstand(stand,s,standlist)
  {
    fprintf(file,"Stand:\t\t%d\n",s);
    fprintstand(file,stand,pftpar,ntotpft);
  }
} /* of 'fprintstandlist' */

Standlist freadstandlist(Bstruct file,          /**< pointer to restart file */
                         const char *name,      /**< name of object */
                         Cell *cell,            /**< Cell pointer */
                         const Pftpar pftpar[], /**< pft parameter array */
                         int ntotpft,           /**< total number of PFTs */
                         const Soilpar *soilpar,/**< soil parameter */
                         Standtype **standtype, /**< array of stand types */
                         int nstand,            /**< number of stand types */
                         Bool separate_harvests /**< separate harvests enabled (TRUE/FALSE) */
                        ) /** \return allocated stand list or NULL */
{
  /* Function reads stand list from file */
  int s,n;
  Standlist standlist;
  if(bstruct_readbeginarray(file,name,&n))
    return NULL;
  /* Read number of stands */
  standlist=newlist(n);
  if(standlist==NULL)
  {
    printallocerr("standlist");
    return NULL;
  }
  /* Read all stand data */
  for(s=0;s<standlist->n;s++)
    if((getlistitem(standlist,s)=freadstand(file,NULL,cell,pftpar,ntotpft,soilpar,
                                            standtype,nstand,separate_harvests))==NULL)
    {
      fprintf(stderr,"ERROR254: Cannot read stand item %d of %s.\n",s,name);
      return NULL;
    }
  if(bstruct_readendarray(file,name))
  {
    return NULL;
  }
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
   if(stand->type->dailyfire!=NULL && stand->type->max_ndayfire>0)
   {
     stand->fires=newqueue(sizeof(Fire)/sizeof(Real),stand->type->max_ndayfire);
     check(stand->fires);
   }
   else
     stand->fires=NULL;
   stand->type->newstand(stand);
   return getlistlen(cell->standlist);
} /* of 'addstand' */

void initstand(Stand *stand /**< Pointer to stand */
              )
{
  /* Function initializes stand */
  stand->fire_sum=0.0;
  stand->growing_days=0;
  stand->Hag_Beta=stand->cell->Hag_beta;
  stand->prescribe_landcover=NO_LANDCOVER;
  stand->afire_frac=0;
} /* of 'initstand' */

void freestand(Stand *stand /**< Pointer to stand */
              )
{
  freepftlist(&stand->pftlist); /* free list of established PFTs */
  freesoil(&stand->soil);
  freequeue(stand->fires);
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

Real standfracsum(const Standlist standlist /**< stand list */
                 )                          /** \return sum of all stand fractions */
{
  Stand *stand;
  Real frac_sum=0.0;
  int s;
  foreachstand(stand,s,standlist)
    frac_sum+=stand->frac;
  return frac_sum;
} /* of 'standfracsum' */

