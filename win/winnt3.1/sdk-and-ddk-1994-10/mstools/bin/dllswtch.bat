@echo off
cls
echo Win32 Debugging DLLs
echo --------------------
echo To assist in debugging and profiling Win32 applications, special 
echo debug versions of the key Windows NT Systems DLL have been provided.
echo.
echo These DLLs, which include all of the DLLs for which import libraries are
echo provided with the Win32 SDK, will enable a developer to get symbolic
echo information in stack traces, set breakpoints on Win32 APIs, and obtain
echo detailed profiling information for Win32 APIs.
echo.
echo This batch file controls switching between debug and nodebug versions
echo of the DLLs.
echo.
echo You must have a Windows NT CD in the CD-ROM, or an image of the CD on the
echo network to install the debug versions of the DLLs. 
echo.

set SWITCH_PLATFORM=
if "%PROCESSOR_ARCHITECTURE%" == "x86"  set SWITCH_PLATFORM=i386
if "%PROCESSOR_ARCHITECTURE%" == "MIPS" set SWITCH_PLATFORM=mips
if "%PROCESSOR_ARCHITECTURE%" == "ALPHA" set SWITCH_PLATFORM=alpha
if "%SWITCH_PLATFORM%" == "" goto errornoplatform

if "%1" == "" goto usage
if "%2" == "" goto usage
if not exist %2\support\debugdll\%SWITCH_PLATFORM%\advapi32.dll goto usage
if "%1" == "nodebug" goto install_nodebug
if "%1" == "NODEBUG" goto install_nodebug
if "%1" == "debug"   goto install_debug
if "%1" == "DEBUG"   goto install_debug
goto usage

:install_debug
if exist %SystemRoot%\system32\advapi32.org goto error_debug

pause
pushd %2\support\debugdll\%SWITCH_PLATFORM%
for %%f in (*.dll) do ren %SystemRoot%\system32\%%f *.org
copy *.dll %SystemRoot%\system32 1>NUL 2>&1
popd
echo.
echo DLLSWTCH Complete.  You must reboot your system for the changes to take
echo effect.
goto end

:error_debug
echo.
echo DLLSWTCH has detected that the debug dlls have already been installed
echo in the %SystemRoot%\system32 directory. If you want to restore the
echo original system DLLs, use DLLSWTCH NODEBUG.
echo.
goto end

:install_nodebug
if not exist %SystemRoot%\system32\advapi32.org goto error_nodebug

pause
pushd %2\support\debugdll\%SWITCH_PLATFORM%
del %SystemRoot%\system32\*.dbl 1>NUL 2>&1
if exist %SystemRoot%\system32\advapi32.dbl goto error_dbl
for %%f in (*.dll) do ren %SystemRoot%\system32\%%f *.dbl
copy %SystemRoot%\system32\*.org %SystemRoot%\system32\*.dll 1>NUL 2>&1
del %SystemRoot%\system32\*.org 1>NUL 2>&1
popd
if exist %SystemRoot%\system32\advapi32.org goto error_org
echo.
echo DLLSWTCH Complete.  You must reboot your system for the changes to take
echo effect.
goto end

:error_dbl
echo.
echo Please delete the old .dbl files from %SystemRoot%\system32 before
echo running DLLSWTCH nodebug.
echo.
goto end

:error_nodebug
echo.
echo DLLSWTCH has detected that the original DLLs have already been restored
echo in the %SystemRoot%\system32 directory. If you want to install the debug
echo system DLLs, use: DLLSWTCH DEBUG.
echo.
goto end

:error_org
echo.
echo DLLSWTCH has restored the original DLLs, but was unable to delete the .org
echo files in the %SystemRoot%\system32 directory.  Please reboot your computer
echo and delete these files by hand before running DLLSWTCH again.
echo.
goto end

:errornoplatform
echo.
echo Cannot continue with unrecognized PROCESSOR_ARCHITECTURE variable.
echo.
goto end

:usage
echo Usage: dllswitch debug/nodebug cdroot
echo Example: dllswitch debug f:
echo This installs the debug DLLs from the Windows NT CD or network image on f:
echo into the %SystemRoot%\system32 directory. It echo will rename the original
echo DLLs to dllname.org.
echo.

:end
