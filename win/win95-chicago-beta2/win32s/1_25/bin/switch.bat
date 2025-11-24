@echo off
if "%1" == "" goto usage1
if "%2" == "" goto usage1
if "%2" == "nodebug" goto install
if "%2" == "NODEBUG" goto install
if "%2" == "debug" goto install
if "%2" == "DEBUG" goto install
goto usage1

:install
cls
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
echo °                                                                            °
echo ° Win32s Debugging                                                           °
echo ° ----------------                                                           °
echo ° To assist debugging Win32 application, a special debugging version         °
echo ° of Win32s is provided along with symbol files for both the debug           °
echo ° and nondebug (retail) versions of Win32s.                                  °
echo °                                                                            °
echo ° These symbol files can be used in conjunction with the debug version       °
echo ° of Windows 3.1 (as provided by the Windows 3.1 SDK) to get run-time        °
echo ° diagnostic messages on a separate debugging terminal.                      °
echo °                                                                            °
echo ° The symbol files are also useful when the kernel debugger (WDEB386.EXE)    °
echo ° is used to debug problems.  Symbolic addresses for Win32s routines         °
echo ° will be enabled when symbol files are present.  Further information        °
echo ° on using a debug version of Windows 3.1 and the kernel debugger is         °
echo ° privided in the Windows 3.1 SDK.                                           °
echo °                                                                            °
echo ° This script controls switching between debug and nondebug versions of      °
echo ° Win32s.                                                                    °
echo °                                                                            °
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

rem Make sure Windows directory is correct path
if not exist %1\system.ini goto usage11
if not exist switch.bat goto usage2

pause

xcopy ..\%2 %1\system\win32s\

copy  %1\system\win32s\win32s16.* %1\system
copy  %1\system\win32s\winmm16.* %1\system
copy  %1\system\win32s\w32sys.* %1\system

copy  %1\system\win32s\compobj.* %1\system
copy  %1\system\win32s\ole2.* %1\system
copy  %1\system\win32s\ole2prox.* %1\system
copy  %1\system\win32s\storage.* %1\system

copy  %1\system\win32s\ole2disp.* %1\system
copy  %1\system\win32s\ole2conv.* %1\system
copy  %1\system\win32s\ole2nls.* %1\system
copy  %1\system\win32s\typelib.* %1\system
copy  %1\system\win32s\stdole.tlb %1\system

del   %1\system\win32s\win32s16.*
del   %1\system\win32s\winmm16.*
del   %1\system\win32s\w32sys.*
del   %1\system\win32s\olecli.*

del   %1\system\win32s\compobj.*
del   %1\system\win32s\ole2.*
del   %1\system\win32s\ole2prox.*
del   %1\system\win32s\storage.*

del  %1\system\win32s\ole2disp.*
del  %1\system\win32s\ole2conv.*
del  %1\system\win32s\ole2nls.*
del  %1\system\win32s\typelib.*
del  %1\system\win32s\stdole.tlb

if not exist %1\system\win32s\freecell.exe goto end
del %1\system\win32s\freecell.*
del %1\system\win32s\cards.*

goto end

:usage1
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
:usage11
echo ° Usage:                                                                     °
echo °     switch [full path to Windows] [debug nodebug]                          °
echo °                                                                            °
echo ° Example:                                                                   °
echo °     switch c:\windows debug                                                °
echo °     This installs the debug version of Win32s into the appropriate         °
echo °     Windows directories containing Win32s files.                           °
echo °     Note: Win32s must already be installed.                                °
echo °                                                                            °
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

:usage2
echo °                                                                            °
echo ° You must run the SWITCH.BAT file from the directory that it resides in.    °
echo °                                                                            °
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

:end
