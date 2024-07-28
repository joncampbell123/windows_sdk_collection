echo off
echo Making FINSTALL.DLL Font Installer
if not %1.==. set LANG=%1
cd src
nmake /f fi_src
IF ERRORLEVEL 1 GOTO OOPS
cd..\rc
nmake /f fi_rc
IF ERRORLEVEL 1 GOTO OOPS
cd..\tmu
nmake /f fi_tmu
IF ERRORLEVEL 1 GOTO OOPS
cd..
if exist finstall.dll copy finstall.dll finstall.bak
if exist finstall.dll del finstall.dll
nmake /f fi_root finstall.dll
IF ERRORLEVEL 1 GOTO OOPS
GOTO END
:OOPS
ECHO *******************************ERRORRROR**************************
:END
set LANG=
