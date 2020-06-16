/**************************************************************************************/
/**                                                                                \n**/
/**                     c  o  o  r  d  .  c                                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Implementation of datatype Coord                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include <sys/stat.h>

struct coordfile
{
  FILE *file;    /* file pointer */
  int first;     /* index of first coordinate */
  int n;         /* number of coordinates in file */
  struct
  {
    float lon,lat;
  } cellsize;
  float scalar;  /* scaling factor */
  Bool swap;     /* byte order has to be changed */
  int fmt;       /* file format (RAW/CLM) */
  size_t offset; /* offset in binary file */
  Type datatype; /* data type */
};               /* Definition of opaque datatype Coordfile */

Coordfile opencoord(const Filename *filename, /**< filename of coord file */
                    Bool isout                /**< enable error output */
                   )                          /** \return open coord file
                                                  or NULL on error       */
{
  Coordfile coordfile;
  Header header;
  struct stat filestat;
  coordfile=new(struct coordfile);
  int version;
  if(coordfile==NULL)
  {
    printallocerr("coord");
    return NULL;
  }
  if(filename->fmt==META)
  {
    /* set default values */
    header.scalar=0.01;
    header.datatype=LPJ_SHORT;
    header.nbands=2;
    header.firstcell=0;
    header.ncell=0;
    header.cellsize_lon=header.cellsize_lat=0.5;
    coordfile->file=openmetafile(&header,&coordfile->swap,&coordfile->offset,filename->name,isout);
    if(coordfile->file==NULL)
    {
      free(coordfile);
      return NULL;
    }
    if(header.ncell<=0)
    {
      if(isout)
        fprintf(stderr,"ERROR221: Invalid number of cells in description file '%s'.\n",filename->name);
      free(coordfile);
      return NULL;
    }
    coordfile->n=header.ncell;
    coordfile->first=header.firstcell;
    coordfile->cellsize.lon=header.cellsize_lon;
    coordfile->cellsize.lat=header.cellsize_lat;
    coordfile->datatype=header.datatype;
    if(header.nbands!=2)
    {
      if(isout)
        fprintf(stderr,"ERROR218: Number of bands=%d in description file '%s' is not 2.\n",
                header.nbands,filename->name);
      fclose(coordfile->file);
      free(coordfile);
      return NULL;
    }
    return coordfile;
  }
  coordfile->file=fopen(filename->name,"rb");
  if(coordfile->file==NULL)
  {
    if(isout)
      printfopenerr(filename->name);
    free(coordfile);
    return NULL;
  }
  if(filename->fmt==RAW)
  {
    fstat(fileno(coordfile->file),&filestat);
    coordfile->n=filestat.st_size/sizeof(Intcoord);
    coordfile->first=0;
    coordfile->swap=FALSE;
    coordfile->scalar=0.01;
    coordfile->cellsize.lon=coordfile->cellsize.lat=0.5;
    coordfile->datatype=LPJ_SHORT;
    coordfile->offset=0;
  }
  else
  {
    if(filename->fmt==CLM)
      version=READ_VERSION;
    else
      version=2;
    if(freadheader(coordfile->file,&header,&coordfile->swap,
                   LPJGRID_HEADER,&version))
    {
      if(isout)
        fprintf(stderr,"ERROR154: Invalid header in '%s'.\n",filename->name);
      fclose(coordfile->file);
      free(coordfile);
      return NULL;
    }
    coordfile->n=header.ncell;
    coordfile->first=header.firstcell;
    coordfile->cellsize.lon=header.cellsize_lon;
    coordfile->cellsize.lat=header.cellsize_lat;
    coordfile->scalar=(float)((version==1) ? 0.01 : header.scalar);
    coordfile->datatype=header.datatype;
    coordfile->offset=headersize(LPJGRID_HEADER,version);
    if(header.nbands!=2)
    {
      if(isout)
        fprintf(stderr,"ERROR218: Number of bands=%d in grid file '%s' is not 2.\n",
                header.nbands,filename->name);
      fclose(coordfile->file);
      free(coordfile);
      return NULL;
    }
  }
  coordfile->fmt=filename->fmt;
  return coordfile;
} /* of 'opencoord' */

void getcellsizecoord(float *lon,               /**< longitudinal size */
                      float *lat,               /**< latitudinal size */
                      const Coordfile coordfile /**< open coord file */)
{
  *lon=coordfile->cellsize.lon;
  *lat=coordfile->cellsize.lat;
} /* of 'getcellsizecoord' */

int numcoord(const Coordfile coordfile /**< open coord file */
            )                          /** \return number of coords in file */
{
  return coordfile->n;
} /* of 'numcoord' */

void closecoord(Coordfile coordfile /**< coord file to be closed */
               )
{
  if(coordfile!=NULL)
  {
    fclose(coordfile->file);
    free(coordfile);
  }
} /* of 'closecoord' */

Bool readintcoord(FILE *file,      /**< pointer to binary file */
                  Intcoord *coord, /**< short coords read */
                  Bool swap        /**< Byte order has to be swapped (TRUE/FALSE) */
                 )                 /** \return TRUE on error */
{
  if(fread(coord,sizeof(Intcoord),1,file)!=1)
    return TRUE;
  if(swap)
  {
    /* data is in different byte order */
    coord->lat=swapshort(coord->lat);
    coord->lon=swapshort(coord->lon);
  }
  return FALSE;
} /* of 'readintcoord' */

Bool readcoord(Coordfile coordfile, /**< open coord file */
               Coord *coord,        /**< cell coordinate read from file */
               const Coord *resol   /**< resolution (deg) */
              )                     /** \return FALSE for successful read */
{
  Intcoord icoord;
  float lon,lat;
  double dlon,dlat;
  int ilon,ilat;
  switch(coordfile->datatype)
  {
    case LPJ_SHORT:
      if(readintcoord(coordfile->file,&icoord,coordfile->swap))
        return TRUE;
      coord->lat=icoord.lat*coordfile->scalar;
      coord->lon=icoord.lon*coordfile->scalar;
      if(resol->lat==0.25)
      {
        if(coord->lat>=0)
          coord->lat+=0.005;
        else
          coord->lat-=0.005;
      }
      if(resol->lon==0.25)
      {
        if(coord->lon>=0)
          coord->lon+=0.005;
        else
          coord->lon-=0.005;
      }
      break;
    case LPJ_FLOAT:
      if(freadfloat(&lon,1,coordfile->swap,coordfile->file)!=1)
        return TRUE;
      if(freadfloat(&lat,1,coordfile->swap,coordfile->file)!=1)
        return TRUE;
      coord->lat=lat;
      coord->lon=lon;
      break;
    case LPJ_DOUBLE:
      if(freaddouble(&dlon,1,coordfile->swap,coordfile->file)!=1)
        return TRUE;
      if(freaddouble(&dlat,1,coordfile->swap,coordfile->file)!=1)
        return TRUE;
      coord->lat=dlat;
      coord->lon=dlon;
      break;
    case LPJ_INT:
      if(freadint(&ilon,1,coordfile->swap,coordfile->file)!=1)
        return TRUE;
      if(freadint(&ilat,1,coordfile->swap,coordfile->file)!=1)
        return TRUE;
      coord->lat=ilat*coordfile->scalar;
      coord->lon=ilon*coordfile->scalar;
      break;
    default:
      return TRUE;
  } /* of switch */
  /* calculate cell area */
  coord->area=cellarea(coord,resol);
  return FALSE;
} /* of 'readcoord' */

Bool writecoord(FILE *file,        /**< pointer to binary file */
                const Coord *coord /**< cell coordinate written to file */
               )                   /** \return FALSE for successful write */
{
  Intcoord icoord;
  icoord.lat=(short)(coord->lat*100);
  icoord.lon=(short)(coord->lon*100);
  return fwrite(&icoord,sizeof(icoord),1,file)!=1;
} /* of 'writecoord' */

Bool writefloatcoord(FILE *file,        /**< pointer to binary file */
                     const Coord *coord /**< cell coordinate written to file */
                    )                   /** \returns TRUE on error */
{
  struct
  {
    float lon,lat;
  } fcoord;
  fcoord.lat=(float)coord->lat;
  fcoord.lon=(float)coord->lon;
  return fwrite(&fcoord,sizeof(fcoord),1,file)!=1;
} /* of 'writefloatcoord' */

int seekcoord(Coordfile coordfile, /**< open coord file */
              int pos              /**< position in file */
             )                     /** \return return code of fseek */
{
  return fseek(coordfile->file,
               (coordfile->fmt==RAW) ? pos*sizeof(Intcoord) : 
               (pos-coordfile->first)*2*typesizes[coordfile->datatype]+coordfile->offset,
               SEEK_SET);
} /* of 'seekcoord' */

Real cellarea(const Coord *coord, /**< cell coordinate */
              const Coord *resol  /**< resolution (deg) */
             )                    /** \return area of cell (m^2) */
{
  return (111194.9*resol->lat)*(111194.9*resol->lon)*cos(deg2rad(coord->lat));
} /* of 'cellarea' */

Bool fscancoord(LPJfile *file, /**< pointer to text file */
                Coord *coord,  /**< cell coordinate read */
                Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
               )               /** \return TRUE on error */
{
  if(fscanreal(file,&coord->lon,"longitude",FALSE,verb))
    return TRUE;
  if(fscanreal(file,&coord->lat,"latitude",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscancoord' */

int findcoord(const Coord *c,      /**< coordinate */
              const Coord array[], /**< array of coordinates */
              int size             /**< size of array */
             )         /** \return index of coordinate found or NOT_FOUND */
{
  int i;
  for(i=0;i<size;i++)
    if(fabs(array[i].lon-c->lon)<epsilon && fabs(array[i].lat-c->lat)<epsilon)
      return i;
  return NOT_FOUND;
} /* of 'findcoord' */

char *sprintcoord(String s,          /**< string to fill */
                  const Coord *coord /**< cell coordinate */
                 )                   /** \return pointer to string */
{
  int pos;
  if(coord->lat<0)
    snprintf(s,STRING_LEN,"%.6gS",-coord->lat);
  else
    snprintf(s,STRING_LEN,"%.6gN",coord->lat);
  pos=strlen(s);
  if(coord->lon<0)
    snprintf(s+pos,STRING_LEN-pos," %.6gW",-coord->lon);
  else
    snprintf(s+pos,STRING_LEN-pos," %.6gE",coord->lon);
  return s;
} /* of 'sprintcoord' */

void fprintcoord(FILE *file,        /**< pointer to text file */
                 const Coord *coord /**< cell coordinate */
                )
{
  if(coord->lat<0)
    fprintf(file,"%.6gS",-coord->lat);
  else
    fprintf(file,"%.6gN",coord->lat);
  if(coord->lon<0)
    fprintf(file," %.6gW",-coord->lon);
  else
    fprintf(file," %.6gE",coord->lon);
} /* of 'fprintcoord' */
