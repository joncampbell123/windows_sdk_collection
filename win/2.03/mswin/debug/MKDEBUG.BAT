echo off

rem This batch file creates a debugging version of Microsoft Windows
rem or Microsoft Windows/386.

cls

if "%1" == "win" goto win
if "%1" == "WIN" goto win
if "%1" == "win386" goto win386
if "%1" == "WIN386" goto win386

echo To create a debugging version of Microsoft Windows or Microsoft
echo Windows/386, you will need the installation disks from the retail
echo package of Microsoft Windows or Microsoft Windows/386, and the
echo installation disks from the Microsoft Windows Software Development
echo Kit.                                                                             
echo Usage:                                                                           
echo             MKDEBUG WIN      for Windows                                                        
echo               or                                                                               
echo             MKDEBUG WIN386   for Windows/386

goto end



:win

A:
CD \DEBUG
COPY SETUP20.INF SETUP.INF
SETUP
goto end



:win386

A:
CD \DEBUG
COPY SETUP386.INF SETUP.INF
SETUP

:end
