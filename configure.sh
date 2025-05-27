#!/bin/bash
#################################################################################
##                                                                             ##
##            c  o  n  f  i  g  u  r  e  .  s  h                               ##
##                                                                             ##
##   configure script to copy appropriate Makefile.$osname                     ##
##                                                                             ##
##   Usage: configure.sh [-h] [-v] [-l] [-prefix dir] [-debug] [-check]        ##
##                       [-nompi] [-noerror] [-Dmacro[=value] ...]             ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
#################################################################################

USAGE="Usage: $0 [-h] [-v] [-l] [-prefix dir] [-debug] [-nompi] [-check] [-noerror] [-Dmacro[=value] ...]"
ERR_USAGE="\nTry \"$0 --help\" for more information."
debug=0
nompi=0
prefix=$PWD
checking=""
macro=""
warning="-Werror"
while(( "$#" )); do
  case "$1" in
    -h|--help)
      echo $0 - configure LPJmL $(cat VERSION)
      echo
      echo $USAGE
      echo
      echo Arguments:
      echo "-h,--help       print this help text"
      echo "-v,--version    print version"
      echo "-l,--license    print license"
      echo "-prefix dir     set installation directory for LPJmL. Default is current directory"
      echo "-debug          set debug flags and disable optimization"
      echo "-check          enable run-time checking of memory leaks and access out of bounds"
      echo "-noerror        do not stop compilation on warnings"
      echo "-nompi          do not build MPI version"
      echo "-Dmacro[=value] define macro for compilation"
      echo
      echo "After successfull completion of $0 LPJmL can be compiled by 'make all'"
      echo "Invoke 'make clean; make all' after change in configuration"
      echo -e "\nSee LICENSE file or invoke $0 -l to print license"
      echo -e "\n(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file"
      exit 0
      ;;
    -l|--license)
      more LICENSE
      exit 0
      ;;
    -v|--version)
      cat VERSION
      exit 0
      ;;
    -prefix)
      if [ $# -lt 2 ]
      then
        echo >&2 Error: prefix directory missing
        echo >&2 $USAGE
        echo -e >&2 $ERR_USAGE
        exit 1
      fi
      prefix=$2
      shift 2
      ;;
    -debug)
      debug=1
      shift 1
      ;;
    -check)
      checking="\$(CHECKFLAGS)"
      shift 1
      ;;
    -noerror)
      warning=""
      shift 1
      ;;
    -nompi)
      nompi=1
      shift 1
      ;;
    -D*)
      macro="$macro $1"
      shift 1
      ;;
    -*)
      echo >&2 Error: Invalid option \'$1\'
      echo >&2 $USAGE
      echo -e >&2 $ERR_USAGE
      exit 1
      ;;
    *)
      echo >&2 Error: Invalid argument \'$1\'
      echo >&2 $USAGE
      echo -e >&2 $ERR_USAGE
      exit 1
      ;;
  esac
done

echo Configuring LPJmL $(cat VERSION)...

osname=$(uname)

if test -d /p ;
then
  inpath=/p/projects/lpjml/input/historical
else
  inpath=""
  echo >&2 No input directory found, LPJINPATH has to be set
fi

if [ "$osname" = "Linux" ]  || [ "$osname" = "CYGWIN_NT-5.1" ] || [ "$osname" = "Darwin" ]
then
  echo Operating system is $osname
  if [ "$nompi" = "1" ]
  then
    if which icc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.icc Makefile.inc
    elif which icx >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.icx Makefile.inc
    elif which gcc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.gcc Makefile.inc
    else
      echo >&2 Error: No gcc found
      exit 1
    fi
  else
    if which mpiicc >/dev/null 2>/dev/null  && which icc >/dev/null 2>/dev/null ;
    then
      echo Intel MPI compiler mpiicc found
      cp config/Makefile.mpiicc Makefile.inc
    elif which mpiicx >/dev/null 2>/dev/null ;
    then
      echo Intel MPI compiler mpiicx found
      cp config/Makefile.mpiicx Makefile.inc
    elif which mpicc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.mpich Makefile.inc
      echo MPICH found
    elif which icc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.icc Makefile.inc
    elif which gcc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.gcc Makefile.inc
    else
      echo >&2 Error: No gcc found
      exit 1
    fi
    if which sbatch >/dev/null 2>/dev/null ;
    then
       echo SLURM found
       ln -sf lpjsubmit_hpc bin/lpjsubmit
    else
       echo >&2 No batch system found
    fi
  fi
  echo "Create executables with 'make all'"
  echo "Invoke 'make clean; make all' after change in configuration"
else
  echo >&2 Warning: unsupported operating system, Makefile.$osname created
  cp config/Makefile.gcc Makefile.$osname
  cp config/Makefile.$osname Makefile.inc
fi
if [ "$debug" = "1" ]
then
  echo "CFLAGS	= \$(WFLAG) \$(LPJFLAGS) $macro $warning $checking \$(DEBUGFLAGS)" >>Makefile.inc
  echo "LNOPTS	= \$(WFLAG) \$(DEBUGFLAGS) $checking -o " >>Makefile.inc
else
  echo "CFLAGS	= \$(WFLAG) \$(LPJFLAGS) $macro $warning $checking \$(OPTFLAGS)" >>Makefile.inc
  echo "LNOPTS	= \$(WFLAG) \$(OPTFLAGS) $checking -o " >>Makefile.inc
fi
echo "GIT_REPO=" $(git remote -v|head -1|cut  -f2|cut -d' ' -f1) >>Makefile.inc
echo LPJROOT	= $prefix >>Makefile.inc
cat >bin/lpj_paths.sh <<EOF
#################################################################################
##                                                                             ##
##          l  p  j  _  p  a  t  h  s  .  s  h                                 ##
##                                                                             ##
##    sh script to set environment variables for LPJmL. A call to this         ##
##    script has to be put in ~/.profile in the following way:                 ##
##                                                                             ##
##    . \$LPJROOT/bin/lpj_paths.sh                                              ##
##                                                                             ##
##    LPJROOT has to be set to your root directory of LPJmL                    ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
##    Created: $(date +"%d.%m.%Y")                                                      ##
##                                                                             ##
#################################################################################

export LPJROOT=$prefix # change here to your directory

# set search path for LPJmL commands

export PATH=\$LPJROOT/bin:\$PATH

# set path for input files

export LPJINPATH=$inpath

# include manpages of LPJmL

export MANPATH=\$LPJROOT/man:\$MANPATH

# define alias

alias printheader="printclm -data"
alias soil2cdf="clm2cdf -notime -raw -byte"
EOF

chmod +x bin/lpj_paths.sh

cat >bin/lpj_paths.csh <<EOF
#################################################################################
##                                                                             ##
##          l  p  j  _  p  a  t  h  s  .  c  s  h                              ##
##                                                                             ##
##    csh script to set environment variables for LPJmL. A call to this        ##
##    script has to be put in ~/.cshrc in the following way:                   ##
##                                                                             ##
##    source \$LPJROOT/bin/lpj_paths.csh                                        ##
##                                                                             ##
##    LPJROOT has to be set to your root directory of LPJmL                    ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
##                                                                             ##
##    Created: $(date +"%d.%m.%Y")                                                      ##
##                                                                             ##
#################################################################################

setenv LPJROOT $prefix # change here to your directory

# set search path for LPJmL commands

setenv PATH \$LPJROOT/bin\:\$PATH

# set path for input files

setenv LPJINPATH $inpath

# include manpages of LPJmL

setenv MANPATH \$LPJROOT/man\:\$MANPATH

# define alias

alias printheader "printclm -data"
alias soil2cdf "clm2cdf -notime -raw -byte"
EOF
chmod +x bin/lpj_paths.csh
echo Put . $prefix/bin/lpj_paths.sh in your ~/.profile
