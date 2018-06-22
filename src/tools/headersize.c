/**************************************************************************************/
/**                                                                                \n**/
/**                     h  e  a  d  e  r  s  i  z  e  .  c                         \n**/
/**                                                                                \n**/
/**     Function gets header size                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

size_t typesizes[]={1,sizeof(short),sizeof(int),sizeof(float),sizeof(double)};
char *typenames[]={"byte","short","int","float","double"};

size_t headersize(const char *headername, /**< header string in CLM file */
                  int version             /**< version of CLM file */
                 )                        /** \return header size in bytes */
{
  switch(version)
  {
    case 0: case 4:
      return 0; /* version=0 -> no header */
    case 1:
      return sizeof(int)+strlen(headername)+sizeof(Header_old);
    case 2:
      return sizeof(int)+strlen(headername)+sizeof(Header2);
    default:
      return sizeof(int)+strlen(headername)+sizeof(Header);
  }
} /* of 'headersize' */
