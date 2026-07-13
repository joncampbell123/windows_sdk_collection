@echo off
rem test05.bat
rem This batch file does a lot of program loads.
rem The results of the tests are written to file test05.out.
rem Execute a bunch of programs that don't do anything with the 
rem options that are specified.
echo on
 
if %os%==OS2 set load=%home%\loadfils\protect
if %os%==DOS set load=%home%\loadfils\real

call sync
if %machine%==1 goto one
%load%\cv 1>nul 2>nul
%load%\cl >nul
%load%\link /# 1>nul 2>nul
%load%\cv 1>nul 2>nul
%load%\cl >nul
%load%\link /# 1>nul 2>nul
%load%\cv 1>nul 2>nul
%load%\cl >nul
%load%\link /# 1>nul 2>nul
%load%\cv 1>nul 2>nul
%load%\cl >nul
%load%\link /# 1>nul 2>nul
:loop
%load%\cv 1>nul 2>nul
%load%\cl >nul
%load%\link /# 1>nul 2>nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop
goto out

:one
%root%\sleep 3
time 0
%load%\cv 1>nul 2>nul
%load%\cl >nul
%load%\link /# 1>nul 2>nul
time  >>%output%\test05.out <%root%\cr.dat
:out



