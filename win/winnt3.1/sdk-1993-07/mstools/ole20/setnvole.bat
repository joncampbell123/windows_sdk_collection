@echo off
if "%1" == "" goto usage
set Lib=%1\samples\lib;%1\lib;%lib%
set Include=%1\samples\h;%1\h;%include%
set Path=%1\bin;%path%
set DEVROOT_DIR=%1
goto end

:usage
echo.
echo   Usage: SETNVOLE OLEROOT
echo   Where: OLEROOT specifies the root directory for the OLE toolkit
echo Example: SETNVOLE C:\MSTOOLS\OLE20
echo.

:end
