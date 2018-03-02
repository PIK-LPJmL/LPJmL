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
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "types.h"

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
        fscanf(file,"%d",&line_count);
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

Bool fscanline(FILE *file,String s)
{
  int c;
  int len;
  c=fscanspace(file);
  if(c==EOF)
  {
    s[0]='\n';
    return TRUE;
  }
  s[0]=(char)c;
  len=1;
  while((c=fgetc(file))!=EOF)
  {
    if(len==STRING_LEN)
    {
      s[len]='\0';
      return TRUE;
    }
    if(c=='\n')
    {
      s[len++]=c;
      s[len]='\0';
      line_count++;
      line_pos=0;
      return FALSE;
    }
    s[len++]=(char)c; /* add character to string */
    line_pos++;
  }
  return FALSE;
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

Bool fscanstring(LPJfile *file, /**< pointer to  a LPJ file         */
                 String s,   /**< pointer to a char array of dimension
                                  STRING_LEN+1                        */
                 const char *name, /**< name of string                */
                 Verbosity verb  /**< enable error output */
                )            /** \return TRUE on error                */
{
  int c;
  int len;
#ifdef USE_JSON
  struct json_object *item;
  const char *str;
  if(file->isjson)
  {
    if(!json_object_object_get_ex(file->file.obj,name,&item))
    {
      if(verb)
        fprintf(stderr,"ERROR225: Name '%s' for string not found.\n",name);
      return TRUE;
    }
    if(json_object_get_type(item)!=json_type_string)
    {
      if(verb)
        fprintf(stderr,"ERROR226: Type of '%s' is not string.\n",name);
      return TRUE;
    }
    str=json_object_get_string(item);
    strncpy(s,str,STRING_LEN);
    s[STRING_LEN]='\0';
    if (verb >= VERB)
      printf("\"%s\" : \"%s\"\n",name,s);
    return FALSE;
  }
#endif
  /* searching for first occurrence of non-whitespace character  */
  c=fscanspace(file->file.file);
  if(c=='\"') /* opening '"' found? */
  {
    len=0;
    while((c=fgetc(file->file.file))!=EOF)
    {
      line_pos++;
      if(c=='\"') /* closing '"' found? */
      {
        s[len]='\0';  /* yes, return with success */
        if(verb>=VERB)
          printf("\"%s\" : \"%s\"\n",name,s);
        return FALSE;
      }
      else if(len==STRING_LEN)  /* string too long? */
      {
        if(verb)
          fprintf(stderr,"ERROR103: String too long in line %d of '%s'.\n",line_count,incfile);
      
        break;
      }
      else if(c=='\\') /* backslash found? */
      {
        if((c=fgetc(file->file.file))==EOF) /* yes, read next character */
        {
          if(verb)
            fprintf(stderr,"ERROR103: EOF reached reading string in line %d of '%s'.\n",line_count,incfile);
          s[len]='\0';
          return TRUE;
        }
        else
          switch(c)
          {
            case '"': case '\\':
              s[len++]=(char)c;
              line_pos++;
              break;
            case 'n':
              s[len++]='\n';
              line_pos++;
              break;
            case 't':
              s[len++]='\t';
              line_pos++;
              break;
            default:
              if(verb)
                fprintf(stderr,"ERROR103: Invalid control character '\\%c' reading string in line %d of '%s'.\n",(char)c,line_count,incfile);
              s[len]='\0';
              return TRUE;
          }
      }
      else
      {
        s[len++]=(char)c;
        line_pos++;
      }
    }
  }
  else
  {
    s[0]=(char)c;
    len=1;
    while((c=fgetc(file->file.file))!=EOF)
    {
      if(isspace(c))
      {
        if(c=='\n')
        {
          line_count++;
          line_pos=0;
        }
        else
          line_pos++;
        s[len]='\0';  /* yes, return with success */
        if(verb>=VERB)
          printf("\"%s\" : \"%s\"\n",name,s);
        return FALSE;
      }
      else if(len==STRING_LEN)  /* string too long? */
      {
        if(verb)
          fprintf(stderr,"ERROR103: String too long in line %d of '%s'.\n",line_count,incfile);
        s[len]='\0';  /* terminate string */
        return TRUE;
      }
      else
      {
        s[len++]=(char)c;
        line_pos++;
      }
    }
    s[len]='\0';
    if(verb>=VERB)
      printf("\"%s\" : \"%s\"\n",name,s);
    return FALSE;
  }
  if(c==EOF && verb)
    fprintf(stderr,"ERROR103: EOF reached reading string in line %d of '%s'.\n",line_count,incfile);
  s[len]='\0';  /* terminate string */
  return TRUE;
} /* of 'fscanstring' */
