@echo off
echo *********************************************************
echo *							*
echo *	Microsoft Windows 3.1.103 -- 1.2Mb disk version *
echo *							*
echo *********************************************************
echo *					                *
echo *	This version requires seven 1.2Mb floppy disks  *
echo *							*
echo *********************************************************
if NOT "%1"=="" goto itsok
..\tools\ync /c abx "Drive a or b"
if ERRORLEVEL==2 goto EXIT
if ERRORLEVEL==1 goto BDRIVE

call trans a:
goto EXIT

:BDRIVE
call trans b:
goto EXIT

:itsok
call trans %1

:EXIT
