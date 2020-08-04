/***************************************************************************/
/**                                                                       **/
/**                    w o o d p l a n t a t i o n  .  c           **/
/**                                                                       **/
/**     Definition of woodplantation stand                                **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: 08.10.2009                                           **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"
#include "natural.h"
#include "agriculture.h"
#include "tree.h"
#include "woodplantation.h"

#if defined IMAGE || defined INCLUDEWP
Standtype woodplantation_stand={WOODPLANTATION,"woodplantation",new_agriculture,
                              free_agriculture,fwrite_agriculture,
                              fread_agriculture,fprint_agriculture,
                              daily_woodplantation,annual_woodplantation,NULL};
#endif

