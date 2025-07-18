/**************************************************************************************/
/**                                                                                \n**/
/**        f  w  r  i  t  e  c  r  o  p  d  a  t  e  s  .  c                       \n**/
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

static Bool fwritecropdatesitem(Bstruct file,const Cropdates *cropdates)
{
  bstruct_writebeginstruct(file,NULL);
  bstruct_writeint(file,"fall_sdate20",cropdates->fall_sdate20);
  bstruct_writeint(file,"last_update_fall",cropdates->last_update_fall);
  bstruct_writeint(file,"spring_sdate20",cropdates->spring_sdate20);
  bstruct_writeint(file,"last_update_spring",cropdates->last_update_spring);
  bstruct_writeint(file,"vern_date20",cropdates->vern_date20);
  bstruct_writeint(file,"last_update_vern",cropdates->last_update_vern);
  return bstruct_writeendstruct(file);
} /* of 'fwritecropdatesitem' */

Bool fwritecropdates(Bstruct file, /**< pointer to binary file */
                     const char *name, /**< name of object */
                     const Cropdates *cropdates, /**< array of crop dates to write */
                     int ncft /**< number of crop dates */
                    )         /** \return TRUE on error */
{
  int cft;
  bstruct_writebeginarray(file,name,ncft);
  for(cft=0;cft<ncft;cft++)
    fwritecropdatesitem(file,cropdates+cft);
  return bstruct_writeendarray(file);
} /* of 'fwritecropdates' */
