@echo off
@echo **********************************************************
@echo This script will set the environment to build os2 base 1.2
@echo **********************************************************
set oldpath=%PATH%
set path=%root%\tools;%root%\tools\itlc;c:\os2
set include=%root%\bsrc\src\inc;%root%\bsrc\src\h;%root%\pmsrc\src\inc;%root%\pmsrc\src\h;
set lib=%root%\bsrc\src\lib;%root%\pmsrc\src\lib;
set msgdir=%root%\bsrc\src\txt
if "%lname%" == "" set lname=msus
set lang=%root%\lang\%lname%
set sedlang=\%root%\lang\%lname%
set ipfc=%root%\tools\itlc
set tmp=\
rem this is for the IPFC compiler and have to do with NLS 
rem L is for the language
set L=ENU
rem P is for the codepage
set P=437
rem C is for the country
set C=1
@echo on
