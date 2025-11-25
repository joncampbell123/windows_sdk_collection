@echo off
if exist %1\mstools\mstools.inf goto setup
echo .
echo Usage:
echo       SETUPSDK [drive letter of location of SDK]
echo .
echo e.g. SETUPSDK F: will setup the Win32 SDK from F:
echo      SETUPSDK    will setup from the current drive.
echo .
goto end
:setup
setup /i %1\mstools\mstools.inf /s %1\
:end
