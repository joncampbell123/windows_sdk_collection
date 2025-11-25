@echo off
rem  need to put mssetup include & libs first on search path in order to
rem  build mssetup sample and other code, else you get the wrong setuapi.h

if "%MSTOOLS%" == "" goto usage

set Lib=%MSTOOLS%\mssetup\lib;%lib%
set Include=%MSTOOLS%\mssetup\include;%include%

goto end

:usage
echo.
echo It is necessary to set Win32 SDK environment variables first.
echo  I.e. \mstools\setenv.bat
echo.

:end
