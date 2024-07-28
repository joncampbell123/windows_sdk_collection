@echo off
echo *********************************************************
echo *							*
echo *	Microsoft Windows 3.1.103 -- 720Kb disk version *
echo *							*
echo *********************************************************
echo *					                *
echo *  This version requires twelve 720Kb floppy disks      *
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
