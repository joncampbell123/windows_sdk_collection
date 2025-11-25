@echo off

if "%1"=="" goto usage

rem This will put the SDK headers & libs first in the search path.

if "%MSTOOLS%"=="" goto no_mstools
call %MSTOOLS%\setenv %MSTOOLS%


if "%BASEDIR%"=="" goto setbasedir
if NOT "%BASEDIR%"=="%1" goto setbasedir

if "%DDKBUILDENV%"=="" goto setenv
if NOT "%DDKBUILDENV%"=="%2" goto envtest
goto done

:setbasedir

rem set BASEDIR to ddk directory and set path to point to ddk binaries

set BASEDIR=%1
set path=%path%;%BASEDIR%\bin

:setenv

set NTMAKEENV=%BASEDIR%\inc
set BUILD_MAKE_PROGRAM=nmake.exe
set BUILD_DEFAULT=-ei -nmake -i

if "%tmp%"=="" set tmp=\

if "%PROCESSOR_ARCHITECTURE%"=="" goto cpuerror
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto alpha
if "%PROCESSOR_ARCHITECTURE%"=="MIPS" goto mips
if "%PROCESSOR_ARCHITECTURE%"=="x86" goto i386
if "%PROCESSOR_ARCHITECTURE%"=="PPC" goto ppc

goto cpuerror

:alpha

if "%Cpu%" == "" set Cpu=ALPHA
set BUILD_DEFAULT_TARGETS=-alpha
set ALPHA=1
set JENSEN=1
set NTALPHADEFAULT=1

goto envtest

:ppc

if "%Cpu%" == "" set Cpu=PPC
set BUILD_DEFAULT_TARGETS=-ppc
set PPC=1
set _PPC_=1
set NTPPCDEFAULT=1

goto envtest

:mips

if "%Cpu%" == "" set Cpu=MIPS
set BUILD_DEFAULT_TARGETS=-mips
set MIPS_R4000=1
set _JAZZIDW=1
set NTMIPSDEFAULT=1

goto envtest

:i386

if "%Cpu%" == "" set Cpu=i386
set BUILD_DEFAULT_TARGETS=-386

:envtest

if "%2"=="" goto free
if "%2"=="free" goto free
if "%2"=="FREE" goto free
if "%2"=="checked" goto checked
if "%2"=="CHECKED" goto checked
goto usage

:free

rem set up an NT free build environment

set DDKBUILDENV=free
set C_DEFINES=-D_IDWBUILD -DDBCS -DJAPAN -DKKBUGFIX -DDBCS_VERT -DFE_SB
set NTDBGFILES=1
set NTDEBUG=
set NTDEBUGTYPE=
set MSC_OPTIMIZATION=
set LANGUAGE=JPN

goto done

:checked

rem set up an NT checked build environment

set DDKBUILDENV=checked
set C_DEFINES=-D_IDWBUILD -DRDRDBG -DSRVDBG -DDBCS -DJAPAN -DKKBUGFIX -DDBCS_VERT -DFE_SB
set NTDBGFILES=
set NTDEBUG=ntsd
set NTDEBUGTYPE=both
set MSC_OPTIMIZATION=/Od /Oi
set LANGUAGE=JPN

:done

set _OBJ_DIR=obj
set NEW_CRTS=1
IF "%_NTROOT%"==""  set _NTROOT=%BASEDIR%

doskey /macrofile=%BASEDIR%\bin\generic.mac

if "%DDKDRIVE%"=="" goto noddkdrive

doskey /macrofile=%BASEDIR%\bin\ddktree.mac
cd %BASEDIR%
%DDKDRIVE%
goto end

:noddkdrive

pushd %BASEDIR%
goto end

:cpuerror

echo.
echo Error: PROCESSOR_ARCHITECTURE environment variable not recognized.
echo.
echo.

goto end

:no_mstools
@echo Error: MSTOOLS environment variable not recognized.
@echo        The Win32 SDK must be installed.

goto end

:usage

echo.
echo usage: setenv ^<directory^> [free^|^checked]
echo.
echo   Example:  setenv d:\ddk checked     set checked environment
echo   Example:  setenv d:\ddk             defaults to free environment
echo.
echo.


:end
