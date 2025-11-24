@echo off
if "%1" == "" goto usage
set Path=%1\bin;%path%
set Lib=%1\lib;%1\mfc\lib;%lib%
set Include=%1\h;%1\mfc\include;%include%

if "%PROCESSOR_ARCHITECTURE%" == "MIPS" goto Mips
set Cpu=i386
goto end

:Mips
set Cpu=MIPS

goto end

:usage
echo.
echo Usage: SETENV SDK
echo Where: SDK specifies where the toolkit was installed 
echo Example: SETENV C:\MSTOOLS sets the environment relative to C:\MSTOOLS
echo.

:end
