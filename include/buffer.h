/**************************************************************************************/
/**                                                                                \n**/
/**                       b  u  f  f  e  r  .  h                                   \n**/
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

#ifndef BUFFER_H
#define BUFFER_H

/* Definitions of datatypes */

typedef struct buffer *Buffer;

/* Declaration of functions */

extern Buffer newbuffer(int);
extern void updatebuffer(Buffer,Real); 
extern Bool fwritebuffer(FILE *,const Buffer);
extern Buffer freadbuffer(FILE *,Bool);
extern void freebuffer(Buffer);
extern Real getbufferavg(const Buffer);
extern Bool isemptybuffer(const Buffer);

#endif
