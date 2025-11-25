@echo off
echo.
echo This setup script will build runner.dll and webrun.cpl.
echo It will also install WebRunner on your system.
echo.
nmake /f runner.mak
if errorlevel 1 goto failure1
goto next

:failure1
echo NMAKE failed for runner.mak
echo Installation has been cancel.

goto error

:next
nmake /f webrun.mak
if errorlevel 1 goto failure2 
goto install

:failure2
echo NMAKE failed for webrun.mak
echo Installation has been cancel.
goto error

:install
copy webrun.cpl %SystemRoot%\SYSTEM32
CONTROL WEBRUN
echo Web Runner installed Successfully!
echo You must now copy runner.dll to the
echo directory you specified for the html file.

:error
