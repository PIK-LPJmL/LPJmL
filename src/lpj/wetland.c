/***************************************************************************/
/**                                                                       **/
/**                     w  e  t  l  a  n  d  .  c                         **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Definition of wetland stand                                       **/
/**                                                                       **/
/**     written by Sibyll Schaphoff                                       **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2016-07-04 13:09:30 +0200 (Mo, 04 Jul 2016#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"
#include "natural.h"
#include "wetland.h"
#include "agriculture.h"

Standtype wetland_stand = {WETLAND,"wetland",new_natural,free_natural,
fwrite_natural,fread_natural,fprint_natural,
daily_natural,annual_natural,dailyfire,isdailyoutput_natural };
Standtype setaside_wetland = {SETASIDE_WETLAND,"wetland setaside",new_agriculture,
        free_agriculture,fwrite_agriculture,
        fread_agriculture,fprint_agriculture,
#ifdef DAILY_ESTABLISHMENT
        daily_setaside,
#else
        daily_natural,
#endif
        annual_setaside,NULL,isdailyoutput_natural};
