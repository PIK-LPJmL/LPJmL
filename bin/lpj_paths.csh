#################################################################################
##                                                                             ##
##          l  p  j  _  p  a  t  h  s  .  c  s  h                              ##
##                                                                             ##
##    csh script to set environment variables for LPJmL. A call to this        ##
##    script has to be put in ~/.cshrc in the following way:                   ##
##                                                                             ##
##    source $LPJROOT/bin/lpj_paths.csh                                        ##
##                                                                             ##
##    LPJROOT has to be set to your root directory of LPJmL                    ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
##    Created: 20.03.2026                                                      ##
##                                                                             ##
#################################################################################

setenv LPJROOT /p/tmp/bloh/git/master-6.0.6 # change here to your directory

# set search path for LPJmL commands

setenv PATH $LPJROOT/bin\:$PATH

# set path for input files

setenv LPJINPATH /p/projects/lpjml/inputs/public_standard

# include manpages of LPJmL

setenv MANPATH $LPJROOT/man\:$MANPATH

# define alias

alias printheader "printclm -data"
alias soil2cdf "clm2cdf -notime -raw -byte"
alias grid2clm "addheader -nbands 2 -nyear 1 -scale 0.01 -ncell 0 -id LPJGRID"
alias cft2clm "addheader -nbands 64 -firstyear 1700 -lastyear 2005 -scale 0.001 -ncell 0 -id LPJLUSE"
alias restart2json "restart2yaml -json"
