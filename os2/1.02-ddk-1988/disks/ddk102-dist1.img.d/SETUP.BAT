echo off
cls
rem
rem *******************************************************
rem * DDDK Utility:	SETUP Device Driver Development Kit
rem *******************************************************
rem
rem SETUP will setup a development environment for 
rem MS OS/2 DDDK by copying two disks to a directory
rem structure that will have the current directory 
rem as the parent.
if '%1' == '' goto erruse
if '%2' == '1' goto prompt1
if '%2' == '2' goto prompt2
cls
echo          ------------------
echo          MS OS/2 DDDK Setup  
echo          ------------------
echo SETUP is now creating the DDDK directory
echo structure.  As you are prompted for each 
echo Distribution Disk be sure to insert the 
echo appropriate one.
echo Note that this setup batch file must be copied to
echo the destination drive or the batch file will
echo not work properly.
echo the root directory is
cd
:prompt1
echo Be Sure Distribution Disk 1 is in drive %1 and
pause
if exist %1setup.bat goto xfer1
goto prompt1
:xfer1
echo Copying files ...
xcopy %1 /s/v/e >nul
if errorlevel 1 goto termerr
:prompt2
echo Insert Distribution Disk 2 in drive %1 and
pause
if exist %1\src\dev\diskdd\disktest.asm goto xfer2
goto prompt2
:xfer2
echo Copying files ...
xcopy %1 /s/v/e >nul
if errorlevel 1 goto termerr
cls
echo                 ------------------
echo                 MS OS/2 DDDK Setup
echo                 ------------------
echo SETUP has finished creating the DDDK structure.
echo Be sure the 5.1 tools are properly installed
echo before the DDDK is used.
goto exit
rem ---------------------------
rem - Usage Error Handler   
rem ---------------------------
:erruse
echo Setup: You must specify a drive letter
echo Usage: SETUP d:
goto exit
rem --------------------------
rem - Fatal Error Handler
rem --------------------------
:termerr
echo *************************
echo *** FATAL SETUP ERROR ***
echo *** SETUP has failed! ***
echo *************************
:exit
