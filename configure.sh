#!/bin/bash
#################################################################################
##                                                                             ##
##            c  o  n  f  i  g  u  r  e  .  s  h                               ##
##                                                                             ##
##   configure script to copy appropriate Makefile.$osname                     ##
##                                                                             ##
##   Usage: configure.sh [-h] [-prefix dir] [-debug] [-check] [-nompi]         ##
##                       [-Dmacro[=value] ...]                                 ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
#################################################################################

debug=0
nompi=0
prefix=$PWD
macro=""
while(( "$#" )); do
  case "$1" in
    -h)
      echo $0 - configure LPJmL $(cat VERSION)
      echo Usage: $0 [-h] [-prefix dir] [-debug] [-nompi] [-check] [-Dmacro[=value] ...]
      echo
      echo Arguments:
      echo "-h              print this help text"
      echo "-prefix dir     set installation directory for LPJmL. Default is current directory"
      echo "-debug          set debug flags and disable optimization"
      echo "-check          set debug flags, enable pointer checking and disable optimization"
      echo "-nompi          do not build MPI version"
      echo "-Dmacro[=value] define macro for compilation"
      echo
      echo After successfull completion of $0 LPJmL can be compiled by make all
      echo Invoke make clean after change in configuration
      exit 0
      ;;
    -prefix)
      if [ $# -lt 2 ]
      then
        echo >&2 Error: prefix directory missing
        echo >&2 Usage: $0 [-h] [-prefix dir] [-debug] [-nompi] [-check] [-Dmacro[=value] ...]
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
      debug=2
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
      echo >&2 Invalid option $1
      echo >&2 Usage: $0 [-h] [-prefix dir] [-debug] [-nompi] [-check] [-Dmacro[=value] ...]
      exit 1
      ;;
    *)
      echo >&2 Invalid argument $1
      echo >&2 Usage: $0 [-h] [-prefix dir] [-debug] [-nompi] [-check] [-Dmacro[=value] ...]
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

if [ "$osname" = "Linux" ]  || [ "$osname" = "CYGWIN_NT-5.1" ]
then
  echo Operating system is $osname
  if [ "$nompi" = "1" ]
  then
    if which icc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.intel Makefile.inc
    elif which gcc >/dev/null 2>/dev/null ;
    then 
      cp config/Makefile.gcc Makefile.inc
    else
      echo >&2 Error: No gcc found
      exit 1
    fi
  else
    if which mpiicc >/dev/null 2>/dev/null ;
    then
      echo Intel MPI found
      cp config/Makefile.cluster2015 Makefile.inc
      if which llsubmit >/dev/null 2>/dev/null ;
      then
         echo LoadLeveler found
         ln -sf lpjsubmit_intel bin/lpjsubmit
      elif which sbatch >/dev/null 2>/dev/null ;
      then
         echo SLURM found
         ln -sf lpjsubmit_slurm bin/lpjsubmit
      else
         echo >2 No batch system found 
      fi
    elif which mpicc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.mpich Makefile.inc
      ln -sf lpjsubmit_mpich bin/lpjsubmit
      echo MPICH found
    elif which icc >/dev/null 2>/dev/null ;
    then 
      cp config/Makefile.intel Makefile.inc
    elif which gcc >/dev/null 2>/dev/null ;
    then 
      cp config/Makefile.gcc Makefile.inc
    else
      echo >&2 Error: No gcc found
      exit 1
    fi
  fi
  echo Create executables with make all
elif [ "$osname" = "Darwin" ]
then
  echo Operating system is $osname
  if [ "$nompi" = "1" ]
  then
    if which icc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.intel Makefile.inc
    elif which gcc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.gcc Makefile.inc
    else
      echo >&2 Error: No gcc found
      exit 1
    fi
  else
    if which mpicc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.darwin_mpich Makefile.inc
      echo MPICH found
    elif which gcc >/dev/null 2>/dev/null ;
    then
      cp config/Makefile.darwin_cc Makefile.inc
    else
      echo >&2 Error: No gcc found
      exit 1
    fi
  fi
  echo Create executables with make all
elif [ "$osname" = "AIX" ] 
then
  echo  Operating system is AIX
  if [ "$nompi" = "1" ]
  then
    cp config/Makefile.aix Makefile.inc
  elif which mpcc >/dev/null 2>/dev/null ;
  then
    echo IBM MPI found
    cp config/Makefile.aix_mpi Makefile.inc
    ln -sf lpjsubmit_aix bin/lpjsubmit
  else
    cp config/Makefile.aix Makefile.inc
  fi
  echo Create executables with make all
else
  echo >&2 Warning: unsupported operating system, Makefile.$osname created
  cp config/Makefile.gcc Makefile.$osname
  cp config/Makefile.$osname Makefile.inc
fi
if [ "$debug" = "1" ]
then
  echo "CFLAGS	= \$(WFLAG) \$(LPJFLAGS) $macro \$(DEBUGFLAGS)" >>Makefile.inc
  echo "LNOPTS	= \$(WFLAG) \$(DEBUGFLAGS) -o " >>Makefile.inc
elif [ "$debug" = "2" ]
then
  echo "CFLAGS	= \$(WFLAG) \$(LPJFLAGS) $macro \$(CHECKFLAGS)" >>Makefile.inc
  echo "LNOPTS	= \$(WFLAG) \$(CHECKFLAGS) -o " >>Makefile.inc
else
  echo "CFLAGS	= \$(WFLAG) \$(LPJFLAGS) $macro \$(OPTFLAGS)" >>Makefile.inc
  echo "LNOPTS	= \$(WFLAG) \$(OPTFLAGS) -o " >>Makefile.inc
fi
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

export MANPATH=$LPJROOT/man:\$MANPATH

# define alias

alias printheader="printclm -data"
alias lpjml='lpjml.sh'
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
alias lpjml 'lpjml.sh'
EOF
ln -sf lpjml bin/lpj
chmod +x bin/lpj_paths.csh
echo Put . $prefix/bin/lpj_paths.sh in your ~/.profile
