/***************************************************************************/
/**                                                                       **/
/**      f  w  r  i  t  e  o  u  t  p  u  t  _  c  h  4  .  c             **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2017-02-10 18:06:57 +0100 (Fri, 10 Feb 201#$ **/
/**     By         : $Author:: sibylls                         $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

void fwriteoutput_ch4(Outputfile *output, Real pch4, Real pco2, const Config *config)
{
  float value;
  if (isroot(*config))
  {
    if (output->files[PCH4].isopen)
    {
      pch4 *= config->outnames[PCH4].scale;
      switch (output->files[PCH4].fmt)
      {
      case RAW:
        value = (float)pch4;
        fwrite(&value, sizeof(float), 1, output->files[PCH4].fp.file);
        break;
      case TXT:
        fprintf(output->files[PCH4].fp.file, "%g\n", pch4);
        break;
      }
    }
    if (output->files[PCO2].isopen)
    {
      pco2 *= config->outnames[PCO2].scale;
      switch (output->files[PCO2].fmt)
      {
      case RAW:
        value = (float)pco2;
        fwrite(&value, sizeof(float), 1, output->files[PCO2].fp.file);
        break;
      case TXT:
        fprintf(output->files[PCO2].fp.file, "%g\n", pco2);
        break;
      }
    }
  }
} /* of 'fwriteoutput_ch4' */
