/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  h  e  a  d  e  r  .  c                         \n**/
/**                                                                                \n**/
/**     Writing file header for LPJ related files.                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwriteheader(FILE *file,             /**< file pointer of binary file */
                  const Header *header,   /**< file header */
                  const char *headername, /**< header string */
                  int version             /**< header version */
                 )                        /** \return TRUE on error */
{
  if(fwrite(headername,strlen(headername),1,file)!=1)
    return TRUE;
  if(fwrite(&version,sizeof(version),1,file)!=1)
    return TRUE;
  switch(version)
  {
    case 1:
      return (fwrite(header,sizeof(Header_old),1,file)!=1);
    case 2:
      return (fwrite(header,sizeof(Header2),1,file)!=1);
    default:
      return (fwrite(header,sizeof(Header),1,file)!=1);
  }
} /* of 'fwriteheader' */
