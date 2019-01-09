#!/bin/sh
#################################################################################
##                                                                             ##
##            c  o  n  f  i  g  u  r  e  .  s  h                               ##
##                                                                             ##
##   configure script to copy appropriate Makefile.$osname                     ##
##                                                                             ##
##   Usage: configure.sh [-h] [-prefix dir] [-debug] [-nompi]                  ##
##                                                                             ##
## (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file ##
## authors, and contributors see AUTHORS file                                  ##
## This file is part of LPJmL and licensed under GNU AGPL Version 3            ##
## or later. See LICENSE file or go to http://www.gnu.org/licenses/            ##
## Contact: https://github.com/PIK-LPJmL/LPJmL                                 ##
#################################################################################

debug=0
nofeedback=0
nompi=0
prefix=$PWD
if [ $# -gt 0 ]
then
  if [ $1 = "-h" ]
  then
    echo $0 - configure LPJmL $(cat VERSION)
    echo Usage: $0 [-h] [-nofeedback] [-prefix dir] [-debug] [-nompi]
    echo
    echo Arguments:
    echo "-h          print this help text"
    echo "-nofeedback sends no feedback for usage statistics to PIK, see https://goo.gl/#analytics/goo.gl/DYv3KW/all_time"
    echo "-prefix dir set installation directory for LPJmL. Default is current directory"
    echo "-debug      set debug flags and disable optimization"
    echo "-nompi      do not build MPI version"
    echo
    echo After successfull completion of $0 LPJmL can be compiled by make all
    echo Invoke make clean after change in configuration
    exit 0
  fi
fi
if [ $# -gt 0 ]
then
  if [ $1 = "-nofeedback" ]
  then
    nofeedback=1
    shift 1
  fi
fi
if [ $# -gt 0 ]
then
  if [ $1 = "-prefix" ]
  then
    if [ $# -lt 2 ]
    then
      echo >&2 Error: prefix directory missing
      echo >&2 Usage: $0 [-h] [-nofeedback] [-prefix dir] [-debug] [-nompi]
      exit 1
    fi
    prefix=$2
    shift 2
  fi
fi
if [ $# -gt 0 ]
then
  if [ $1 = "-debug" ]
  then
    debug=1
    shift 1
  fi
fi
if [ $# -gt 0 ]
then
  if [ $1 = "-nompi" ]
  then
    nompi=1
  else
    echo >&2 Invalid option $1
    echo >&2 Usage: $0 [-h] [-nofeedback] [-prefix dir] [-debug] [-nompi]
  fi
fi

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
      if test -d /p ;
      then
        cp config/Makefile.cluster2015 Makefile.inc
      else 
        cp config/Makefile.intel_mpi Makefile.inc
      fi
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
  echo "CFLAGS	= \$(WFLAG) \$(LPJFLAGS) \$(DEBUGFLAGS)" >>Makefile.inc
  echo "LNOPTS	= \$(WFLAG) \$(DEBUGFLAGS) -o " >>Makefile.inc
else
  echo "CFLAGS	= \$(WFLAG) \$(LPJFLAGS) \$(OPTFLAGS)" >>Makefile.inc
  echo "LNOPTS	= \$(WFLAG) \$(OPTFLAGS) -o " >>Makefile.inc
fi
if [ "$nofeedback" = "0" ]
then
  wget https://goo.gl/DYv3KW --header="User-Agent: Mozilla/5.0 (LPJmL 4.0.001 internal gitlab configure; U; Intel Mac OS X; en-US; rv:1.8.1.12) Gecko/20080219 Navigator/9.0.0.6" -O /dev/null
  echo "CALLHOME=1" >>Makefile.inc
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

export MANPATH=\$MANPATH:\$LPJROOT/man

# define alias

alias printheader="printclm -data"
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

setenv PATH \$PATH:\$LPJROOT/bin

# set path for input files

setenv LPJINPATH $inpath

# include manpages of LPJmL

setenv MANPATH \$MANPATH:\$LPJROOT/man
EOF
ln -sf lpjml bin/lpj
chmod +x bin/lpj_paths.csh
echo Put . $prefix/bin/lpj_paths.sh in your ~/.profile
