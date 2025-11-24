@echo off
if "%1" == "" goto usage

if "%2" == "" goto setwin32
if exist "%2\vcvars32.bat" call %2\vcvars32

:setwin32
set Path=%1\bin;%path%
set Lib=%1\lib;%1\mssetup\lib;%lib%
set Include=%1\include;%1\mssetup\include;%include%
set Mstools=%1

if "%PROCESSOR_ARCHITECTURE%" == "MIPS" goto Mips
if "%PROCESSOR_ARCHITECTURE%" == "ALPHA" goto Alpha
if "%PROCESSOR_ARCHITECTURE%" == "PPC" goto Ppc
set Cpu=i386
goto end

:Mips
set Cpu=MIPS

goto end

:Alpha
set Cpu=ALPHA

goto end

:Ppc
set Cpu=PPC

goto end

:usage
echo.
echo Usage: SETENV SDK [MSVC20BIN]
echo Where: SDK specifies where the toolkit was installed 
echo        MSVC20BIN optionally specifies the BINs of the MSVC directory
echo Example: SETENV C:\MSTOOLS sets the environment relative to C:\MSTOOLS
echo          SETENV C:\MSTOOLS C:\MSVC20\BIN sets Win32 and MSVC environments
echo.

:end
