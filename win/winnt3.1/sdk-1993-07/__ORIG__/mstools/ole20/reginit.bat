@echo off
if "%1" == "" goto usage

%1\bin\regadd
%SystemRoot%\regedit %1\bin\ole2.reg
%SystemRoot%\regedit %1\bin\outline.reg
%SystemRoot%\regedit %1\bin\oletest.reg
%SystemRoot%\regedit %1\bin\sdemo1.reg
if "%CPU%"=="MIPS" goto end
%SystemRoot%\regedit %1\bin\dispcalc.reg
%SystemRoot%\regedit %1\bin\dspcalc2.reg
%SystemRoot%\regedit %1\bin\ole2auto.reg
%SystemRoot%\regedit %1\bin\spoly.reg
%SystemRoot%\regedit %1\bin\spoly2.reg
%SystemRoot%\regedit %1\bin\stdole.reg
%SystemRoot%\regedit %1\bin\test32.reg

goto end

:usage
echo.
echo   Usage: RegInit OLEROOT
echo   Where: OLEROOT specifies the root directory for the OLE toolkit
echo Example: RegInit C:\MSTOOLS\OLE20
echo.

:end
