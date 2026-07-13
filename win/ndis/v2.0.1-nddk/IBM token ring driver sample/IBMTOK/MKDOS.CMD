@echo off
rem
rem This batch file will create the OS/2 version of the IBM Token Ring
rem MAC driver.  The following must be met:
rem
rem     current directory   -   source files
rem
rem note that because shared source is used for both OS/2 and DOS if the
rem last MAKE was done for the "other" OS then you must touch all .ASM
rem files before MAKEing.
rem
rem This batch file takes one argument, either DEV or PRODUCT.  The default
rem is PRODUCT.  This determines the version that will be built.  After
rem a successful MAKE the driver will be in the current directory as well
rem as the OS2 subdirectory DEV or PRODUCT.
rem

set oldpath=%path%
rem path should point to the 1.2 SDK tools received with the 2.0.1 NDDK
set path=c:\12tools.sdk;%path%
rem lib and include point to C510 libraries and include files respectively
set olib=%lib%
set oinc=%include%
set lib=%lib%;.\inc
set include=%include%;.\inc
set targetos=DOS
set version=%1
set listings=NO
if "%version%"=="" set version=PRODUCT
nmake -e
set path=%oldpath%
set lib=%olib%
set include=%oinc%
set oldpath=
set oinc=
set olib=
