/**************************************************************************************/
/**                                                                                \n**/
/**                     t  y  p  e  s  .  h                                        \n**/
/**                                                                                \n**/
/**     Definition of basic datatypes and macros                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef TYPES_H /* Already included? */
#define TYPES_H

#include <time.h>

/* Definition of constants */

#define epsilon 1.0E-6 /* a minimal value -- check if neglegible */
#define NOT_FOUND -1
#ifndef TRUE    /* Check whether TRUE or FALSE are already defined */
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define STRING_LEN 255 /* Maximum length of default strings  */

#ifdef _WIN32
#ifndef isnan /* isnan is already defined in math.h of Microsoft Visual Studio 2015, but not in 2010 */
#define isnan(x) (x!=x)
#endif
#define snprintf sprintf_s
#define strdup _strdup
#define fileno _fileno
#define unlink _unlink
#define pclose _pclose
#define popen _popen
#define alloca _alloca

#endif
 
#ifdef __INTEL_COMPILER
/* #pragma warning ( disable : 869 )*/ /* disable warning about unused parameter */
#endif

/* Definition of datatypes */

typedef double Real; /* Default floating point type in LPJ */

typedef int Bool; /* type boolean (TRUE/FALSE) */

typedef unsigned char Byte;

typedef char String[STRING_LEN+1];

extern size_t typesizes[];
extern char *typenames[];

typedef enum {LPJ_BYTE,LPJ_SHORT,LPJ_INT,LPJ_FLOAT,LPJ_DOUBLE} Type;
typedef enum {NO_ERR, ERR, VERB } Verbosity;

typedef struct
{
  char *name; /* name of file */
  char *var;  /* name of variable in NetCDF file or NULL */
  int fmt;    /* format (TXT/RAW/CLM/CDF) */
} Filename;

typedef struct
{
  union
  {
    FILE *file;              /* pointer to text file */
#ifdef USE_JSON
    struct json_object *obj; /* pointer to JSON object */
#endif
  } file;
  Bool isjson;
} LPJfile;
 
/* Declaration of functions */

extern void fail(int,Bool,const char *,...);
extern Bool fscanreal(LPJfile *,Real *,const char *,Bool,Verbosity);
extern Bool fscanbool(LPJfile *,Bool *,const char *,Bool,Verbosity);
extern Bool fscanrealarray(LPJfile *,Real *,int,const char *,Verbosity);
extern Bool fscanstring(LPJfile *,String,const char *,Bool,Verbosity);
extern Bool fscanstruct(const LPJfile *,LPJfile *,const char *,Verbosity);
extern Bool fscanarray(LPJfile *,LPJfile *,int *,Bool,const char *,Verbosity);
extern Bool fscanarrayindex(const LPJfile *,LPJfile *,int,Verbosity);
extern Bool iskeydefined(const LPJfile *,const char *);
extern Bool isboolean(const LPJfile *,const char *);
extern Bool fscanline(FILE *,char [],int,Verbosity);
extern Bool fscantoken(FILE *,String);
extern char *sysname(void);
extern char *getpath(const char *);
extern char *gethost(void);
extern char *getuser(void);
extern char *getdir(void);
extern void fbanner(FILE *,const char * const *,int,int);
extern void frepeatch(FILE *,char,int);
extern Bool isabspath(const char *);
extern char *addpath(const char *,const char *);
extern void printflags(const char *);
extern long long getfilesize(const char *);
extern const char *strippath(const char *);
extern long long diskfree(const char *);
extern void fprintintf(FILE *,int);
extern void getcounts(int [],int [],int,int,int);
extern char *getbuilddate(void);
extern time_t getfiledate(const char *);
extern char *stripsuffix(const char *);
extern Bool hassuffix(const char *,const char *);
extern char *mkfilename(const char *);
extern int findstr(const char *,const char *const *,int);
extern Bool checkfmt(const char *,char);
extern int fputstring(FILE *,int,const char *,int);
extern Bool fscanint(LPJfile *,int *,const char *,Bool,Verbosity);
extern Bool fscansize(LPJfile *,size_t *,const char *,Bool,Verbosity);
extern Bool fscanuint(LPJfile *,unsigned int *,const char *,Bool,Verbosity);
extern Bool fscanfloat(LPJfile *,float *,const char *,Bool,Verbosity);
extern void fprinttime(FILE *,int);
extern Bool readrealvec(FILE *,Real *,Real,Real,size_t,Bool,Type);
extern Bool readfloatvec(FILE *,float *,float,size_t,Bool,Type);
extern Bool readintvec(FILE *,int *,size_t,Bool,Type);
extern Bool readuintvec(FILE *,unsigned int *,size_t,Bool,Type);
extern Bool readfilename(LPJfile *,Filename *,const char *,const char *,Bool,Verbosity);
extern void **newmat(size_t,int,int);
extern void freemat(void **);
extern char *catstrvec(const char * const *,int);
extern char *strdate(const time_t *);
extern int getlinecount(void);
extern char* getfilename(void);
extern void initscan(const char *);
extern void fputprintable(FILE *,const char *);
extern Bool fscaninteof(FILE *,int *,const char *,Bool *,Bool);
#ifdef WITH_FPE
extern void enablefpe(void);
#endif

/* Definitions of macros */

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#define repeatch(c,count) frepeatch(stdout,c,count)
#define banner(s,size,w) fbanner(stdout,s,size,w)
#define newvec(type,size) (type *)malloc(sizeof(type)*(size))
#define newvec2(type,lo,hi) (type *)malloc(sizeof(type)*(hi-(lo)+1))-(lo)
#define newmatrix(type,ysize,xsize) (type **)newmat(sizeof(type),ysize,xsize)
#define freevec(ptr,lo,hi) free(ptr+lo)
#define check(ptr) if((ptr)==NULL) fail(ALLOC_MEMORY_ERR,FALSE,"Cannot allocate memory for '%s' in %s()",#ptr,__FUNCTION__)
#define printintf(i) fprintintf(stdout,i)

#define new(type) (type *)malloc(sizeof(type))
#ifndef min         /* min macro defined? */
#define min(a,b) (((a)<(b)) ?  (a) : (b))
#endif
#ifndef max         /* max macro defined? */
#define max(a,b) (((a)>(b)) ?  (a) : (b))
#endif
#define fwriten(var,size,n,file) if(fwrite(var,size,n,file)!=n) return TRUE
#define fwrite1(var,size,file) fwriten(var,size,1,file)
#define bool2str(b) ((b) ? "true" : "false")
#define printtime(s) fprinttime(stdout,s)

#endif /* of TYPES_H */
