@echo off
cls
echo Win32s Debugging
echo ----------------
echo To assist debugging Win32 application, a special debugging version
echo of Win32s is provided along with symbol files for both the debug
echo and nondebug (retail) versions of Win32s.
echo.
echo These symbol files can be used in conjunction with the debug version
echo of Windows 3.1 (as provided by the Windows 3.1 SDK) to get run-time
echo diagnostic messages on a separate debugging terminal.
echo.
echo The symbol files are also useful when the kernel debugger (WDEB386.EXE)
echo is used to debug problems.  Symbolic addresses for Win32s routines
echo will be enabled when symbol files are present.  Further information
echo on using a debug version of Windows 3.1 and the kernel debugger is
echo privided in the Windows 3.1 SDK.
echo.
echo This script controls switching between debug and nondebug versions of Win32s.
echo You must run the SWITCH.BAT file from the directory that it resides in.
echo.

if "%1" == "" goto usage
if "%2" == "" goto usage
if "%2" == "nodebug" goto install
if "%2" == "NODEBUG" goto install
if "%2" == "debug" goto install
if "%2" == "DEBUG" goto install
goto usage

rem Make sure Windows directory is correct path
:install
if not exist %1\system.ini goto usage

pause
xcopy ..\%2 %1\system\win32s\
copy  %1\system\win32s\win32s16.* %1\system
copy  %1\system\win32s\winmm16.* %1\system
copy  %1\system\win32s\w32sys.* %1\system
del %1\system\win32s\win32s16.*
del %1\system\win32s\winmm16.*
del %1\system\win32s\w32sys.*
del %1\system\win32s\olecli.*
if not exist %1\system\win32s\freecell.exe goto end
del %1\system\win32s\freecell.*
del %1\system\win32s\cards.*

goto end

:usage
echo Usage: switch [full path to Windows] [debug nodebug]
echo Example: switch c:\windows debug
echo This installs the debug version of Win32s into the appropriate Windows
echo directories containing Win32s files. Note: Win32s must already be installed.
echo.

:end
