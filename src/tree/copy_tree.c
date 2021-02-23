/***************************************************************************/
/**                                                                       **/
/**               c  o p  y  _  t  r  e  e  .  c                          **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function copies tree-specific variables                           **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2016-07-04 08:48:34 +0200 (Mo, 04 Jul 2016#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"
#include "tree.h"

void copy_tree(Pft *dst,const Pft *src)
{
  Pfttree *dst_tree;
  const Pfttree *src_tree;
  dst_tree=dst->data;
  src_tree=src->data;
  *dst_tree=*src_tree;
} /* of 'copy_tree' */
