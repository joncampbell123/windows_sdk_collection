@echo off
if exist %1\mstools\win32.inf goto set1
if exist %1\win32sdk\mstools\win32.inf goto set2
echo .
echo Usage:
echo       SETUPSDK [path to the MSTOOLS dir of SDK]
echo .
echo e.g. SETUPSDK F:\sdk will setup the Win32 SDK from F:\sdk\mstools
echo      SETUPSDK        will setup from the current drive.
echo .
goto end
:set1
set SDKSRC=%1
goto setup
:set2
set SDKSRC=%1\win32sdk
:setup
if "%OS%" == "" goto seti386
if "%PROCESSOR_ARCHITECTURE%" == "x86" goto seti386
setup /f /i %SDKSRC%\mstools\win32.inf /s %SDKSRC%\
goto end
:seti386
%SDKSRC%\mstools\bin\i386\setup /f /i %SDKSRC%\mstools\win32.inf /s %SDKSRC%\
:end
set SDKSRC=
