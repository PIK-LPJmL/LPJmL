/**************************************************************************************/
/**                                                                                \n**/
/**      f  w  r  i  t  e  o  u  t  p  u  t  _  c  h  4  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fwriteoutput_ch4(Outputfile *output,  /**< output file array */
                      Real pch4,           /**< atmnospheric CH4 (ppb) */
                      Real pco2,           /**< atmospheric CO2 (ppm) */
                      int year,            /**< simulation year */
                      const Config *config /**< LPJmL configuration */
                     )
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
    if(output->files[PCO2].issocket)
      send_conc_coupler(PCO2,&pco2,year,config);
    if(output->files[PCH4].issocket)
      send_conc_coupler(PCO2,&pch4,year,config);
  }
} /* of 'fwriteoutput_ch4' */
