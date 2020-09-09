@echo off

xcopy /Q /I par    		    lpj_trunk\par
xcopy /Q /I include		    lpj_trunk\include
copy  input_image_win.conf 	lpj_trunk\input_image_win.conf
copy  param.conf 		    lpj_trunk\param.conf
copy  param_wp.conf         lpj_trunk\param_wp.conf

:: get revision number
echo -
echo Prepare the zip file. This might take some minutes!!
echo -

:: set the path from which the revision has to be taken
SET modeldir=.\
:: get revision number level from working copy
:: FOR /f "tokens=5" %%i IN ('SubWCRev %modeldir%^|find "Last committed at revision"') DO SET version=%%i 
SET version="git"
:: remove spaces from end of string
SET _version=%version: =%
:: zip the file
call zip -r zips\lpj_%_version%.zip lpj_trunk

echo -
echo zip file with revision number %_version% is written to directory: ZIP
echo -

sleep 10

rmdir /S /Q lpj_trunk
