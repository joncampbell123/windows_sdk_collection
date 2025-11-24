@echo off
REM MAPICHNG.BAT
REM Used to change the flavor of currently installed MAPI components
REM Copyright 1994 Microsoft Corporation. All Rights Reserved.

set MAPI_DEBUG=0
set MAPI_NODEBUG=0
set MAPI_SPOOLSYS=0
set MAPI_SUFFIX=32

REM Parameters present?
if "%1" == "" goto Usage
if "%2" == "" goto Usage

REM Case insensitive check for DEBUG or NODEBUG
set MAPI_%1=1

set MAPI_SOURCE=
if %MAPI_DEBUG% == 1 set MAPI_SOURCE=.\DEBUG
if %MAPI_NODEBUG% == 1 set MAPI_SOURCE=.\NODEBUG

REM Parameter correct?
if NOT "%MAPI_SOURCE%" == "" goto CheckInputFlavor
set MAPI_%1=
goto Usage

:CheckInputFlavor
REM See if this is the 16-bit or 32-bit version of MAPI. Use the
REM contents of the source directory to figure this out.

if EXIST %MAPI_SOURCE%\MAPIX32.DLL goto CheckParam2
if NOT EXIST %MAPI_SOURCE%\MAPIX.DLL goto WrongSourceDir
set MAPI_SUFFIX=

:CheckParam2
REM Confirm that there's a MAPIX DLL in the target. For 32-bit,
REM this proves that we have a good target directory. We'll need
REM one more test (below) for 16-bit.

if EXIST %2\mapix%MAPI_SUFFIX%.dll goto NeedBackSlash
if EXIST %2mapix%MAPI_SUFFIX%.dll goto NoBackSlash

REM No MAPIX DLL. See if it's because this isn't the system directory
REM or if it's because MAPI isn't installed.

if EXIST %2\USER.EXE goto NoWMS
if EXIST %2USER.EXE goto NoWMS

echo %2 is NOT a correctly installed Windows System directory.
goto Usage

:NoWMS
echo The Windows Messaging Services are not installed.
goto Done

:NeedBackSlash
set MAPI_WINSYS=%2\
goto CheckOutputFlavor

:NoBackSlash
set MAPI_WINSYS=%2

:CheckOutputFlavor
REM Make sure that we're copying the correct version of MAPI to
REM the correct version of MAPI. CheckParam2 above already would
REM have done the right thing for 32 bit (confirming MAPIX32.DLL),
REM but if the source is 16-bit, the presence of MAPIX.DLL doesn't
REM prove that the destination is correct, since this DLL exists
REM in both 16 and 32 bit MAPI system directories. To really be
REM sure, confirm that mapix32.dll does not exist on the target.

if "%MAPI_SUFFIX%" == "32" goto DoCopy
if NOT EXIST %MAPI_WINSYS%mapix32.dll goto DoCopy

echo %2 is a 32-bit installation; aborting attempt to install 16-bit MAPI.
goto Done

:DoCopy
REM Find out if spooler is in Windows or System directory.

if EXIST %MAPI_WINSYS%mapisp%MAPI_SUFFIX%.exe set MAPI_SPOOLSYS=1

REM Brute force check to see if MAPI is active. If so, abort the copy.

del %MAPI_WINSYS%MAPIX%MAPI_SUFFIX%.DLL > NUL
if EXIST %MAPI_WINSYS%MAPIX%MAPI_SUFFIX%.DLL goto WMSRunning

REM Copy everything to system directory. This is better than a replace
REM because debug might add files.

xcopy /v %MAPI_SOURCE%\*.* %MAPI_WINSYS%

REM Special case for Spooler - goes to Windows directory (sometimes)

if %MAPI_SPOOLSYS% == 1 goto Done
copy %MAPI_WINSYS%mapisp%MAPI_SUFFIX%.exe %MAPI_WINSYS%..
del %MAPI_WINSYS%mapisp%MAPI_SUFFIX%.exe
goto Done

:WrongSourceDir
echo The current directory does not contain the DEBUG or NODEBUG directory containing the MAPI components to switch.
goto Done

:Usage
echo.
echo usage: mapichng Debug/NoDebug Windows-System-Directory
echo e.g.: MAPICHNG DEBUG C:\WINDOWS\SYSTEM
echo       MAPICHNG NODEBUG C:\WINDOWS\SYSTEM
echo.
echo NOTE:  The current directory MUST contain the DEBUG or NODEBUG directory containing the MAPI components that will
echo        be switched.
echo.
goto Done

:WMSRunning
echo The Windows Messaging Services are currently running on this computer,
echo the system cannot be updated at this time.


:Done
set MAPI_DEBUG=
set MAPI_NODEBUG=
set MAPI_SPOOLSYS=
set MAPI_SOURCE=
set MAPI_WINSYS=
set MAPI_SUFFIX=

