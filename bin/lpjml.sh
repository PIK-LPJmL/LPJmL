#!/bin/sh
#################################################################################
##                                                                             ##
##           l  p  j  m  l  .  s  h                                            ##
##                                                                             ##
##    sh script to run lpjml on login as well as on batch nodes on cluster2015 ##
##    at PIK                                                                   ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
#################################################################################

host=$(hostname)
if [ "$host" = "login01" ] || [ "$host" = "login02" ]
then
  unset I_MPI_DAPL_UD_PROVIDER
fi
$LPJROOT/bin/lpjml $*
