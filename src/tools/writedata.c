/**************************************************************************************/
/**                                                                                \n**/
/**                   w  r  i  t  e  d  a  t  a  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions for writinng JSON-like objects into restart file                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "errmsg.h"
#include "swap.h"

static Bool writename(FILE *file,const String name)
{
  /* Function write name of object into restar file */
  Bool rc;
  Byte b;
  if(name==NULL)
    b=0;
  else
  {
    if(strlen(name)>255)
    {
      fprintf(stderr,"ERROR610: String too long for key, must be<256.\n");
      return TRUE;
    }
    b=strlen(name);
  }
  rc=fwrite(&b,1,1,file)!=1;
  if(b)
    rc=fwrite(name,1,b,file)!=b;
  return rc;
} /* of 'writename' */

Bool writebool(FILE *file,       /**< pointer to restart file */
               const char *name, /**< name of object or NULL */
               Bool value        /**< value written to file */
              )                  /** \return TRUE on error */
{
  Byte b;
  b=LPJ_BOOL;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  b=value;
  return fwrite(&b,sizeof(b),1,file)!=1;
} /* of 'writebool' */

Bool writebyte(FILE *file,       /**< pointer to restart file */
               const char *name, /**< name of object or NULL */
               Byte value        /**< value written to file */
              )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? LPJ_ZERO : LPJ_BYTE;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,file)!=1;
  return rc;
} /* of 'writebyte' */

Bool writeint(FILE *file,       /**< pointer to restart file */
              const char *name, /**< name of object or NULL */
              int value         /**< value written to file */
             )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? LPJ_ZERO : LPJ_INT;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,file)!=1;
  return rc;
} /* of 'writeint' */

Bool writeintarray(FILE *file,        /**< pointer to restart file */
                   const char *name,  /**< name of object or NULL */
                   const int value[], /**< array written to file */
                   int size           /**< size of arary */
                  )                   /** \return TRUE on error */
{
  int i;
  if(writearray(file,name,size))
    return TRUE;
  for(i=0;i<size;i++)
    if(writeint(file,NULL,value[i]))
      return TRUE;
  return writeendarray(file);
} /* of 'writeintarray */

Bool writeushortarray(FILE *file,                   /**< pointer to restart file */
                      const char *name,             /**< name of object or NULL */
                      const unsigned short value[], /**< array written to file */
                      int size                      /**< size of arary */
                     )                              /** \return TRUE on error */
{
  int i;
  if(writearray(file,name,size))
    return TRUE;
  for(i=0;i<size;i++)
    if(writeushort(file,NULL,value[i]))
      return TRUE;
  return writeendarray(file);
} /* of 'writeushortarray */

Bool writeshortarray(FILE *file,          /**< pointer to restart file */
                     const char *name,    /**< name of object or NULL */
                     const short value[], /**< array written to file */
                     int size             /**< size of arary */
                    )                     /** \return TRUE on error */
{
  int i;
  if(writearray(file,name,size))
    return TRUE;
  for(i=0;i<size;i++)
    if(writeshort(file,NULL,value[i]))
      return TRUE;
  return writeendarray(file);
} /* of 'writeshortarray */

Bool writeushort(FILE *file,          /**< pointer to restart file */
                 const char *name,    /**< name of object or NULL */
                 unsigned short value /**< value written to file */
                )                     /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? LPJ_ZERO : LPJ_USHORT;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,file)!=1;
  return rc;
} /* of 'writeushort' */

Bool writeshort(FILE *file,       /**< pointer to restart file */
                const char *name, /**< name of object or NULL */
                short value       /**< value written to file */
               )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? LPJ_ZERO : LPJ_SHORT;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,file)!=1;
  return rc;
} /* of 'writeshort' */


Bool writereal(FILE *file,       /**< pointer to restart file */
               const char *name, /**< name of object or NULL */
               Real value        /**< value written to file */
              )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  if(value==0.0)
    b=LPJ_ZERO;
  else
    b=(sizeof(Real)==sizeof(double)) ? LPJ_DOUBLE : LPJ_FLOAT;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  if(value!=0.0)
    rc=fwrite(&value,sizeof(value),1,file)!=1;
  return rc;
} /* of 'writereal' */

Bool writefloat(FILE *file,       /**< pointer to restart file */
                const char *name, /**< name of object or NULL */
                float value       /**< value written to file */
               )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? LPJ_ZERO : LPJ_FLOAT;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  if(value!=0.0)
    rc=fwrite(&value,sizeof(value),1,file)!=1;
  return rc;
} /* of 'writefloat' */

Bool writestring(FILE *file,       /**< pointer to restart file */
                 const char *name, /**< name of object or NULL */
                 const char *value /**< string written to file */
                )                  /** \return TRUE on error */
{
  int len;
  Byte b;
  b=LPJ_STRING;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  len=strlen(value);
  fwrite(&len,sizeof(len),1,file);
  return fwrite(value,1,len,file)!=len;
} /* of 'writestring' */

Bool writearray(FILE *file,       /**< pointer to restart file */
                const char *name, /**< name of object or NULL */
                int size          /**< size of arary */
               )                  /** \return TRUE on error */
{
  Byte b;
  b=LPJ_ARRAY;
  fwrite(&b,1,1,file);
  if(writename(file,name))
    return TRUE;
  return fwrite(&size,sizeof(size),1,file)!=1;
} /* of 'writearray' */

Bool writerealarray(FILE *file,       /**< pointer to restart file */
                    const char *name, /**< name of object or NULL */
                    const Real vec[], /**< array written to file */
                    int size          /**< size of arary */
                   )                  /** \return TRUE on error */
{
  int i;
  writearray(file,name,size);
  for(i=0;i<size;i++)
    writereal(file,NULL,vec[i]);
  return writeendarray(file);
} /* of 'writerealarray' */

Bool writestocks(FILE *file,          /**< pointer to restart file */
                 const char *name,    /**< name of object or NULL */
                 const Stocks *stocks /**< stocks written to file */
                )                     /** \return TRUE on error */
{
  writestruct(file,name);
  writereal(file,"C",stocks->carbon);
  writereal(file,"N",stocks->nitrogen);
  return writeendstruct(file);
} /* of 'wrritestocks' */

Bool writestocksarray(FILE *file,         /**< pointer to restart file */
                      const char *name,   /**< name of object or NULL */
                      const Stocks vec[], /**< array written to file */
                      int size            /**< size of arary */
                     )                    /** \return TRUE on error */
{
  int i;
  writearray(file,name,size);
  for(i=0;i<size;i++)
    writestocks(file,NULL,vec+i);
  return writeendarray(file);
} /* of 'writestocksarray' */

Bool writestruct(FILE *file,      /**< pointer to restart file */
                 const char *name /**< name of object or NULL */
                )                 /** \return TRUE on error */
{ 
  Byte b;
  b=LPJ_STRUCT;
  fwrite(&b,1,1,file);
  return writename(file,name);
} /* of 'writestruct' */

Bool writeendstruct(FILE *file /**< pointer to restart file */
                   )           /** \return TRUE on error */
{
  Byte b;
  b=LPJ_ENDSTRUCT;
  return fwrite(&b,1,1,file)!=1;
} /* of 'writeendstruct' */

Bool writeendarray(FILE *file /**< pointer to restart file */
                  )           /** \return TRUE on error */
{
  Byte b;
  b=LPJ_ENDARRAY;
  return fwrite(&b,1,1,file)!=1;
} /* of 'writeendarray' */
