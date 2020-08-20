set PROJECT=%1

if "%M_VCVARS%" == "" set M_VCVARS="0"

:comp_vc14_userdefined
	rem ++ everything is set for compiling

	if NOT "%VSINSTALLDIR%"=="" goto comp_vc14_set7
	echo "Trying to set Visual Studio Settings for architecture : x64"
	@if exist "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" goto comp_vc14_set6
	goto comp_vc14_compile

:comp_vc14_set6
	echo "found environment variables for x64"
	if %M_VCVARS%=="1"  goto comp_vc14_compile
	set M_VCVARS="1"
    REM optie is x86 (alja) of  x86_amd64 (Pascal voor C)
	call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64  
	if NOT "%VSINSTALLDIR%"=="" goto comp_vc14_compile

:comp_vc14_set7
	if %M_VCVARS%=="1"  goto comp_vc14_compile
	set M_VCVARS="1"
	call "%VSINSTALLDIR%\vcvarsall.bat" x86_amd64
	goto comp_vc14_compile

:comp_vc14_compile
	REM ++ ready to compile
	echo Using Microsoft compiler (vs2015 - v14)
	call devenv.com %PROJECT% /Build "Release|win32"
	if errorlevel 1 goto failure
	goto end			

:end
exit /b 0
    
:failure
echo: Error in compiling 
