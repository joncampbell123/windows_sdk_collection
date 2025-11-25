@echo off
if "%1" == "" goto usage
set Path=%1\bin;%1\mstest;%path%
set Lib=%1\lib;%1\mssetup\lib;%lib%
set Include=%1\h;%1\mstest\include;%1\mssetup\include;%include%
set Mstools=%1

if "%PROCESSOR_ARCHITECTURE%" == "MIPS" goto Mips
if "%PROCESSOR_ARCHITECTURE%" == "ALPHA" goto Alpha
set Cpu=i386
goto end

:Mips
set Cpu=MIPS

goto end

:Alpha
set Cpu=ALPHA

goto end

:usage
echo.
echo Usage: SETENV SDK
echo Where: SDK specifies where the toolkit was installed 
echo Example: SETENV C:\MSTOOLS sets the environment relative to C:\MSTOOLS
echo.

:end
