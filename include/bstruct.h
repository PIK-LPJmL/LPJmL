/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  .  h                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions for reading/writing JSON-like objects from binary file           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef BSTRUCT_H

/* Definition of datatypes */

typedef enum {BSTRUCT_BYTE,BSTRUCT_SHORT,BSTRUCT_INT,BSTRUCT_FLOAT,BSTRUCT_DOUBLE,
              BSTRUCT_FALSE,BSTRUCT_TRUE,BSTRUCT_USHORT,BSTRUCT_ZERO,BSTRUCT_FZERO,
              BSTRUCT_STRING,BSTRUCT_STRING1,BSTRUCT_BEGINARRAY,BSTRUCT_BEGINARRAY1,
              BSTRUCT_BEGINSTRUCT, BSTRUCT_INDEXARRAY,BSTRUCT_ENDSTRUCT,BSTRUCT_ENDARRAY,
              BSTRUCT_END} Bstruct_token;

typedef struct
{
  Byte token;   /**< type of object */
  int size;     /**< size of array */
  char *name;   /**< name of data object */
  union
  {
    char *string;
    long long *index;
    double d;
    float f;
    short s;
    unsigned short us;
    int i;
    Byte b;
  } data;       /**< data content */
} Bstruct_data;

typedef struct bstruct *Bstruct;

extern char *bstruct_typenames[];

/* Declaration of functions */

extern Bstruct bstruct_create(const char *);
extern Bstruct bstruct_open(const char *,Bool);
extern Bstruct bstruct_append(const char *,Bool);
extern int bstruct_getmiss(const Bstruct);
extern FILE *bstruct_getfile(Bstruct);
extern Hash bstruct_gethash(Bstruct);
extern void bstruct_freehash(Bstruct);
extern Bool bstruct_isdefined(Bstruct,const char *);
extern Bool bstruct_writearrayindex(Bstruct,long long,long long vec[],int,int);
extern void bstruct_finish(Bstruct);
extern void bstruct_sync(Bstruct);
extern void bstruct_setout(Bstruct,Bool);
extern Bool bstruct_writebool(Bstruct,const char *,Bool);
extern Bool bstruct_writebyte(Bstruct,const char *,Byte);
extern Bool bstruct_writeint(Bstruct,const char *,int);
extern Bool bstruct_writeshort(Bstruct,const char *,short);
extern Bool bstruct_writeushort(Bstruct,const char *,unsigned short);
extern Bool bstruct_writeintarray(Bstruct,const char *,const int[],int);
extern Bool bstruct_writeshortarray(Bstruct,const char *,const short[],int);
extern Bool bstruct_writeushortarray(Bstruct,const char *,const unsigned short[],int);
extern Bool bstruct_writereal(Bstruct,const char *,Real);
extern Bool bstruct_writefloat(Bstruct,const char *,float);
extern Bool bstruct_writedouble(Bstruct,const char *,double);
extern Bool bstruct_writerealarray(Bstruct,const char *,const Real [],int);
extern Bool bstruct_writedoublearray(Bstruct,const char *,const double [],int);
extern Bool bstruct_writefloatarray(Bstruct,const char *,const float [],int);
extern Bool bstruct_writestring(Bstruct,const char *,const char *);
extern Bool bstruct_writebeginarray(Bstruct,const char *,int);
extern Bool bstruct_writeindexarray(Bstruct,const char *,long long *,int);
extern Bool bstruct_writebeginstruct(Bstruct,const char *);
extern Bool bstruct_writeendstruct(Bstruct);
extern Bool bstruct_writeendarray(Bstruct);
extern void bstruct_fprintdata(FILE *,const Bstruct_data *);
extern Bool bstruct_readdata(Bstruct,Bstruct_data *);
extern void bstruct_freedata(Bstruct_data *);
extern Bool bstruct_readbeginarray(Bstruct,const char *,int *);
extern Bool bstruct_readindexarray(Bstruct,long long *,int);
extern Bool bstruct_seekindexarray(Bstruct,int,int);
extern Bool bstruct_readbool(Bstruct,const char *,Bool *);
extern Bool bstruct_readbyte(Bstruct,const char *,Byte *);
extern Bool bstruct_readint(Bstruct,const char *,int *);
extern Bool bstruct_readintarray(Bstruct,const char *,int *,int);
extern Bool bstruct_readshort(Bstruct,const char *,short *);
extern Bool bstruct_readushort(Bstruct,const char *,unsigned short *);
extern Bool bstruct_readfloat(Bstruct,const char *,float *);
extern Bool bstruct_readdouble(Bstruct,const char *,double *);
extern Bool bstruct_readreal(Bstruct,const char *,Real *);
extern Bool bstruct_readbeginstruct(Bstruct,const char *);
extern char *bstruct_readstring(Bstruct,const char *);
extern Bool bstruct_readendstruct(Bstruct,const char *);
extern Bool bstruct_readendarray(Bstruct,const char *);
extern Bool bstruct_readushortarray(Bstruct,const char *,unsigned short[],int);
extern Bool bstruct_readshortarray(Bstruct,const char *,short[],int);
extern Bool bstruct_readrealarray(Bstruct,const char *,Real [],int);
extern Bool bstruct_readdoublearray(Bstruct,const char *,double [],int);
extern Bool bstruct_readfloatarray(Bstruct,const char *,float [],int);
extern Real *bstruct_readvarrealarray(Bstruct,const char *,int *);
extern void bstruct_fprintnametable(FILE *,const char *,const Bstruct,Bool);
extern long long bstruct_getarrayindex(Bstruct);

/* Definition of macros */

#define bstruct_printdata(data) bstruct_fprintdata(stdout,data)
#define bstruct_printnametable(name,bstr,isjson) bstruct_fprintnametable(stdout,name,bstr,isjson)

#endif /* of BSTRUCT_H */
