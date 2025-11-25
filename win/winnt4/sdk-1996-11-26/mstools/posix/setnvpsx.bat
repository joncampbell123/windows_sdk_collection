@echo off
if "%1" == "" goto usage
set Lib=%1\posix\lib;%lib%
set Include=%1\posix\h;%include%

goto end

:usage
echo.
echo Usage: SETNVPSX SDK
echo Where: SDK specifies the where the toolkit was installed 
echo Example: SETNVPSX C:\MSTOOLS sets the environment relative to C:\MSTOOLS
echo.

:end
