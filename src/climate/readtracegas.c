/**************************************************************************************/
/**                                                                                \n**/
/**               r  e  a  d  t  r  a  c  e  g  a  s  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads atmopsheric trace gas concentration from text file.         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readtracegas(Tracedata *trace,         /**< trace gas data read */
                  const Filename *filename, /**< filename of trace gas file */
                  const Config *config,     /**< LPJmL configuration */
                  Bool isout                /**< error output? */
                 )                          /** \return TRUE on error */
{
  LPJfile file;
  int yr, yr_old,size;
  Bool iseof;
  file.isjson = FALSE;
  Verbosity verbose;
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;

  if (filename->fmt == FMS || (iscoupled(*config) && filename->issocket))
  {
    trace->data = NULL;
    trace->nyear = 0;
    trace->firstyear = 0;
    if(filename->issocket)
    {
      if(openinput_coupler(filename->id,LPJ_FLOAT,0,&size,config))
        return TRUE;
      trace->id=filename->id;
      if(size!=1)
      {
        if(verbose)
          fprintf(stderr,"ERROR149: Invalid number of bands=%d received from socket, must be 1.\n",size);
        return TRUE;
      }
    }
  }
  else if (filename->fmt == TXT)
  {
    if ((file.file.file = fopen(filename->name, "r")) == NULL)
    {
      if (isout)
        printfopenerr(filename->name);
      return TRUE;
    }
    trace->data = newvec(Real, 1);
    if (trace->data == NULL)
    {
      printallocerr("trace");
      fclose(file.file.file);
      return TRUE;
    }
    /* find start year in trace-file */
    if (fscanint(&file, &yr, "year", FALSE, isout ? ERR : NO_ERR) || fscanreal(&file, trace->data, "trace", FALSE, isout ? ERR : NO_ERR))
    {
      if (isout)
        fprintf(stderr, "ERROR129: Cannot read trace gas data in first line of '%s'.\n",
                filename->name);
      free(trace->data);
      fclose(file.file.file);
      return TRUE;
    }
    trace->firstyear = yr;
    trace->nyear = 1;
    yr_old = yr;
    while (!feof(file.file.file))
    {
      trace->data = (Real *)realloc(trace->data, sizeof(Real)*(trace->nyear + 1));
      if (trace->data == NULL)
      {
        printallocerr("trace");
        fclose(file.file.file);
        return TRUE;
      }
      if (fscaninteof(file.file.file, &yr, "year", &iseof, isout) || fscanreal(&file, trace->data + trace->nyear, "trace", FALSE, isout ? ERR : NO_ERR))
      {
        if (iseof)
          break;
        if (isout)
          fprintf(stderr, "ERROR129: Cannot read trace gas data in line %d of '%s'.\n",
                  trace->nyear + 1, filename->name);
        free(trace->data);
        fclose(file.file.file);
        return TRUE;
      }
      if (yr != yr_old + 1)
      {
        if (isout)
          fprintf(stderr, "ERROR157: Invalid year=%d in line %d of '%s'.\n",
                  yr, trace->nyear + 1, filename->name);
        free(trace->data);
        fclose(file.file.file);
        return TRUE;
      }
      trace->nyear++;
      yr_old = yr;
    } /* of while */
    fclose(file.file.file);
  }
  else if(filename->fmt==SOCK && config->start_coupling>config->firstyear-config->nspinup)
  {
    if(verbose)
      fprintf(stderr,"ERROR149: No filename specified for trace data required for socket connection before coupling year %d, first simulatiomn year=%d.\n",
             config->start_coupling,config->firstyear-config->nspinup);
    return TRUE;
  }
  else
    return TRUE;
  return FALSE;
} /* of 'readtracegas' */
