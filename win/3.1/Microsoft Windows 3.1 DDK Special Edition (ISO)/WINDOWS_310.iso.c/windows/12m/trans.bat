@echo off
if NOT "%1"=="" goto itsok
echo *********************************************************
echo *							*
echo *	Microsoft Windows 3.1.103 -- 1.2Mb disk version *
echo *							*
echo *********************************************************
echo *					                *
echo *	This version requires seven 1.2Mb floppy disks  *
echo *							*
echo *	USAGE:  TRANSFER drive-spec:			*
echo *	WHERE:  drive-spec: is your 1.2Mb floppy drive	*
echo *	   IE:  TRANSFER b:				*
echo *							*
echo *********************************************************
goto egress
:itsok
echo 
echo Transferring Windows 3.1.103 to drive %1
..\tools\ync /c ca "Continue or Abort"
if ErrorLevel==1 goto EGRESS


:DISK1
cls
echo Insert disk #1 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK2
..\tools\imdump disk1.img %1
..\tools\imdump disk1.img %1 /v

:DISK2
echo 
echo Insert disk #2 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK3
..\tools\imdump disk2.img %1
..\tools\imdump disk2.img %1 /v

:DISK3
echo 
echo Insert disk #3 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK4
..\tools\imdump disk3.img %1
..\tools\imdump disk3.img %1 /v

:DISK4
echo 
echo Insert disk #4 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK5
..\tools\imdump disk4.img %1
..\tools\imdump disk4.img %1 /v

:DISK5
echo 
echo Insert disk #5 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK6
..\tools\imdump disk5.img %1
..\tools\imdump disk5.img %1 /v

:DISK6
echo 
echo Insert disk #6 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK7
..\tools\imdump disk6.img %1
..\tools\imdump disk6.img %1 /v

:DISK7
echo 
echo Insert disk #7 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK8
..\tools\imdump disk7.img %1
..\tools\imdump disk7.img %1 /v

:DISK8
GOTO DONE
echo 
echo Insert disk #8 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK9
..\tools\imdump disk8.img %1
..\tools\imdump disk8.img %1 /v

:DISK9
GOTO DONE
echo 
echo Insert disk #9 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK10
..\tools\imdump disk9.img %1
..\tools\imdump disk9.img %1 /v

:DISK10
echo 
echo Insert disk #10 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto EGRESS
..\tools\imdump diska.img %1
..\tools\imdump diska.img %1 /v
goto EGRESS
:DISK11
echo 
echo Insert disk #11 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK12
..\tools\imdump diskb.img %1
..\tools\imdump diskb.img %1 /v

:DISK12
echo 
echo Insert disk #12 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK13
..\tools\imdump diskc.img %1
..\tools\imdump diskc.img %1 /v

:DISK13
echo 
echo Insert disk #13 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK14
..\tools\imdump diskd.img %1
..\tools\imdump diskd.img %1 /v

:DISK14
echo 
echo Insert disk #14 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK15
..\tools\imdump diske.img %1
..\tools\imdump diske.img %1 /v

:DISK15
echo 
echo Insert disk #15 in %1
..\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto EGRESS
..\tools\imdump diskf.img %1
..\tools\imdump diskf.img %1 /v

:DONE
echo All done!
pause
:EGRESS
echo on
