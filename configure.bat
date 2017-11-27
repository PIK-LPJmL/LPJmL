@echo off
rem
rem          c  o  n  f  i  g  u  r  e  .  b  a  t
rem
rem  Configure batch file for Microsoft Windows
rem
rem  Last change: 03.03.2009
rem
echo Configuring LPJmL Version 3.5.003 ...
copy config\Makefile.win32 Makefile.inc
echo Create executables with nmake all
set lpjroot=%cd%
echo @echo off>bin\lpj_paths.bat
echo rem>>bin\lpj_paths.bat
echo rem          l  p  j  _  p  a  t  h  s  .  b  a  t>>bin\lpj_paths.bat
echo rem>>bin\lpj_paths.bat
echo set lpjroot="%lpjroot%">>bin\lpj_paths.bat
type config\lpj_paths.txt >>bin\lpj_paths.bat
echo Put %lpjroot%\bin\lpj_paths in your autoexec.bat
