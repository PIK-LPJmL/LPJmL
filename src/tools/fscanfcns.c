/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  f  c  n  s  .  c                              \n**/
/**                                                                                \n**/
/**     Toolbox to read strings and tokens from text file with error               \n**/
/**     handling                                                                   \n**/
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
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <json-c/json.h>
#include "types.h"
#include "errmsg.h"

#define LINE_LEN 1024  /* line length read in at once */

static int line_count; /* line number currently read */
static int line_pos;   /* position in line */
static String incfile; /* filename currently processed by cpp */

int getlinecount(void) /** \return current line number */
{
  return line_count;
} /* of getlinecount' */

char *getfilename(void) /** \return current processed filename */
{
  return incfile;
} /* of 'getfilename' */

void initscan(const char *filename /**< name of file opened for reading */
             )
{
  line_count=1;
  line_pos=0;
  strncpy(incfile,filename,STRING_LEN);
  incfile[STRING_LEN]='\0';
} /* of 'initscan' */

static int fscanspace(FILE *file /* file pointer of a text file       */
                     )           /* returns first non-space character */
{
  int c;
  int len;
  /* searching for first occurence of non-whitespace character */
  while((c=fgetc(file))!=EOF)
    if(!isspace(c))
    {
      /* cpp includes '# line "includefile" into processed files,
         this is now parsed */
      if(line_pos==0 && c=='#')
      {
        /* skip non-space character */
        while((c=fgetc(file))!=EOF)
          if(isspace(c))
            break;
        /* read line number */
        if(fscanf(file,"%d",&line_count));
        while((c=fgetc(file))!=EOF)
          if(!isspace(c)|| c=='\n')
            break;
        if(c!='\n')
        {
          /* read filename of include file */
          if(c=='"')
          {
            len=0;
            while((c=fgetc(file))!=EOF)
              if(c!='\"')
              {
                if(len==STRING_LEN)
                  break;
                incfile[len++]=(char)c;
              }
              else
                break;
            incfile[len]='\0';
          } 
          /* read until end of line */
          while((c=fgetc(file))!=EOF)
           if(c=='\n')
             break;
        }
        line_pos=0;
      }
      else 
        break;
    }
    else if(c=='\n')
    {
      /* new line read increment line count */
      line_count++;
      line_pos=0;
    }
  return c;
} /* of 'fscanspace' */

char *fscanline(FILE *file /**< pointer to text file */
               )           /** \return pointer to line or NULL on error or end of file */
{
  char *line,*ptr;
  int size;
  int c;
  int len;
  c=fscanspace(file);
  if(c==EOF)
    return NULL;
  size=LINE_LEN;
  line=malloc(size);
  if(line==NULL)
  {
    printallocerr("line");
    return NULL;
  }
  line[0]=(char)c;
  len=1;
  while((c=fgetc(file))!=EOF)
  {
    if(len==size)
    {
      size+=LINE_LEN;
      ptr=line;
      line=realloc(line,size);
      if(line==NULL)
      {
        printallocerr("line");
        free(ptr);
        return NULL;
      }
    }
    if(c=='\n')
    {
      line[len++]=(char)c;
      if(len==size)
      {
        ptr=line;
        line=realloc(line,size+1);
        if(line==NULL)
        {
          printallocerr("line");
          free(ptr);
          return NULL;
        }
      }
      line[len]='\0';
      line_count++; /* increase line number */
      line_pos=0;
      return line;
    }
    line[len++]=(char)c; /* add character to string */
    line_pos++;
  }
  if(len==size)
  {
    ptr=line;
    line=realloc(line,size+1);
    if(line==NULL)
    {
      printallocerr("line");
      free(ptr);
      return NULL;
    }
  }
  line[len]='\0';
  return line;
} /* of 'fscanline' */

Bool fscantoken(FILE *file, /**< file pointer of a text file         */
                String s    /**< pointer to a char array of dimension
                                 STRING_LEN+1                        */
               )            /** \return TRUE on error                */
{
  int c;
  int len;
  /* searching for first occurrence of non-whitespace character */
  c=fscanspace(file);
  if(c==EOF) /* reached end of file? */
  {
    s[0]='\0';
    return TRUE;
  }
  s[0]=(char)c;
  len=1;
  while((c=fgetc(file))!=EOF)
  {
    if(isspace(c)) /* white space detected? */
    {
      ungetc(c,file);
      break;
    }
    else if(len==STRING_LEN)  /* string too long? */
    {
      s[len]='\0';  /* terminate string */
      return TRUE;
    }
    else
    {
      s[len++]=(char)c; /* add character to string */
      line_pos++;
    }
  }
  s[len]='\0';  /* terminate string */
  return FALSE;
} /* of 'fscantoken' */

const char *fscanstring(LPJfile *file,    /**< pointer to  a LPJ file         */
                        const char *dflt, /**< pointer to a string or NULL */
                        const char *name, /**< name of string or NULL        */
                        Verbosity verb    /**< enable error output */
                      )                   /** \return string or NULL on error */
{
  const char *str;
  struct json_object *item;
  if(name==NULL)
    item=file;
  else
  {
    if(!json_object_object_get_ex(file,name,&item))
    {
      if(dflt!=NULL)
      {
        if(verb)
          fprintf(stderr,"WARNING027: Name '%s' for string not found, set to '%s'.\n",name,dflt);
        return dflt;
      }
      else
      {
        if(verb)
          fprintf(stderr,"ERROR225: Name '%s' for string not found.\n",name);
        return NULL;
      }
    }
  }
  if(json_object_get_type(item)!=json_type_string)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Type of '%s' is not string.\n",(name==NULL) ? "N/A" : name);
    return NULL;
  }
  str=json_object_get_string(item);
  if (verb >= VERB)
    printf("\"%s\" : \"%s\"\n",(name==NULL) ? "N/A" : name,str);
  return str;
} /* of 'fscanstring' */
