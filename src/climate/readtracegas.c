/***************************************************************************/
/**                                                                       **/
/**               r  e  a  d  t  r  a  c  e  g  a  s  .  c                **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function initclimate read atmopsheric trace gas concentration     **/
/**     from text file.                                                   **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: 01.03.2013                                           **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

Bool readtracegas(Tracedata *trace, const Filename *filename, Bool isout)
{
  LPJfile file;
  int yr, yr_old;
  Bool iseof;
  file.isjson = FALSE;
  if (filename->fmt == FMS)
  {
    trace->data = NULL;
    trace->nyear = 0;
    trace->firstyear = 0;
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
    }
    fclose(file.file.file);
  }
  else
    return TRUE;
  return FALSE;
} /* of 'readtracegas' */
