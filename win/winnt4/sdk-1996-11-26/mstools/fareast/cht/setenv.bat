@echo off
if "%1" == "" goto usage

if "%2" == "" goto setwin32
if exist "%2\vcvars32.bat" call %2\vcvars32 %PROCESSOR_ARCHITECTURE%

:setwin32
set Path=%1\bin;%path%
set Lib=%1\lib;%lib%
set Include=%1\include;%include%
set Mstools=%1
set TARGETLANG=LANG_CHINESE
set TARGETSUBLANG=SUBLANG_CHINESE_TRADITIONAL

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
echo Usage: SETENV SDK [MSDEV]
echo Where: SDK specifies where the toolkit was installed 
echo        MSDEV optionally specifies the BINs of the MSVC directory
echo Example: SETENV C:\MSTOOLS sets the environment relative to C:\MSTOOLS
echo          SETENV C:\MSTOOLS C:\MSDEV\BIN sets Win32 and MSVC environments
echo.

:end
