/**************************************************************************************/
/**                                                                                \n**/
/**                      m  a  n  a  g  e  2  j  s  .  c                           \n**/
/**                                                                                \n**/
/**     Converts management parameter files in *.par format into JSON format.      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "types.h"

#define fscanstr2(file,s) if(fscanstr(file,s)) return TRUE;

static int fscanspace(FILE *file /* file pointer of a text file       */
                     )           /* returns first non-space character */
{
  int c;
  Bool iscmt;
  /* searching for first occurence of non-whitespace character */
  iscmt=FALSE;
  while((c=fgetc(file))!=EOF)
    if(iscmt)
    {
      if(c=='*')
      {
        c=fgetc(file);
        if(c==EOF)
          break;
        if(c=='/')
          iscmt=FALSE;
        else
          ungetc(c,file);
      }
    }
    else
    {
      if(!isspace(c))
      {
        if(c=='#')
        {
          while((c=fgetc(file))!=EOF)
            if(c=='\n')
              break;
        }
        else if(c=='/')
        {
          c=fgetc(file);
          if(c==EOF)
            break; 
          if(c=='*')
            iscmt=TRUE;
          if(c=='/')
          {
            while((c=fgetc(file))!=EOF)
              if(c=='\n')
                break;
          }
        }
        else 
          break;
      }
    }
  return c;
} /* of 'fscanspace' */

static Bool fscanstr(FILE *file, /**< pointer to text file */
                     String s    /**< pointer to a char array */
                    )            /** \return TRUE on error  */
{
  int c;
  int len;
  /* searching for first occurrence of non-whitespace character  */
  c=fscanspace(file);
  if(c=='\"') /* opening '"' found? */
  {
    len=0;
    while((c=fgetc(file))!=EOF)
    {
      if(c=='\"') /* closing '"' found? */
      {
        s[len]='\0';  /* yes, return with success */
        return FALSE;
      }
      else if(len==STRING_LEN)  /* string too long? */
      {
        fprintf(stderr,"ERROR103: String too long.\n");
      
        break;
      }
      else if(c=='\\') /* backslash found? */
      {
        if((c=fgetc(file))==EOF) /* yes, read next character */
        {
          fprintf(stderr,"ERROR103: EOF reached reading string.\n");
          s[len]='\0';
          return TRUE;
        }
        else
          switch(c)
          {
            case '"': case '\\':
              s[len++]=(char)c;
              break;
            case 'n':
              s[len++]='\n';
              break;
            case 't':
              s[len++]='\t';
              break;
            default:
              fprintf(stderr,"ERROR103: Invalid control character '\\%c' reading string.\n",(char)c);
              s[len]='\0';
              return TRUE;
          }
      }
      else
      {
        s[len++]=(char)c;
      }
    }
  }
  else
  {
    s[0]=(char)c;
    len=1;
    while((c=fgetc(file))!=EOF)
    {
      if(isspace(c))
      {
        s[len]='\0';  /* yes, return with success */
        return FALSE;
      }
      else if(len==STRING_LEN)  /* string too long? */
      {
        fprintf(stderr,"ERROR103: String too long.\n");
        s[len]='\0';  /* terminate string */
        return TRUE;
      }
      else
      {
        s[len++]=(char)c;
      }
    }
    s[len]='\0';
    return FALSE;
  }
  if(c==EOF)
    fprintf(stderr,"ERROR103: EOF reached reading string.\n");
  s[len]='\0';  /* terminate string */
  return TRUE;
} /* of 'fscanstr' */

int main(int argc,char **argv)
{
  const char *irrig[]={"no","surface","sprinkler","drip"};
  int i,j,n,irrig_system;
  FILE *file;
  char *ptr;
  String s;
  if(argc<2)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
           "Usage: %s laimax.par\n",argv[0]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[1],"r");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }
  fscanstr2(file,s);
  n=strtol(s,&ptr,10);
  if(*ptr!='\0')
  {
    fprintf(stderr,"Cannot read int in '%s', found '%s'.\n", argv[1],s);
    return EXIT_FAILURE;
  }
  printf("[\n");
  for(i=0;i<n;i++)
  {
    fscanstr2(file,s);
    printf("  { \"id\" : %s,",s);
    fscanstr2(file,s);
    printf(" \"name\" : \"%s\",",s);
    printf(" \"laimax\" : [");
    for(j=0;j<12;j++)
    {
      fscanstr2(file,s);
      printf(" %s",s);
      if(j<11)
        printf(",");
    }
    fscanstr2(file,s);
    irrig_system=strtol(s,&ptr,10);
    if(*ptr!='\0')
      printf("] \"default_irrig_system\" : %s}",s);
    else
    {
      if(irrig_system<0 || irrig_system>3)
      {
        fprintf(stderr,"Invalid number %d for irrigation system in '%s', must be in [0,3].\n",
                irrig_system,argv[1]);
        return EXIT_FAILURE;
      }
      printf("] \"default_irrig_system\" : \"%s\"}",irrig[irrig_system]);
    }
    if(i<n-1)
      printf(",\n");
    else 
      printf("\n");
  }
  printf("]\n");
  return EXIT_SUCCESS;
} /* of 'main' */
