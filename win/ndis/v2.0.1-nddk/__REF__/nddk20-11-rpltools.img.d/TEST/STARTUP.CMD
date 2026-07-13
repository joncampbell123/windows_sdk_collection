set RPLSERVER=
set FILESERVER=
set COMPUTERNAME=

:loop
if not "%logerr%"=="" getremtm \\%RPLSERVER% & call setrmtod & echo error logging on %REMTIME% %REMDATE%>>logerr
net logon %COMPUTERNAME%_USER %COMPUTERNAME%_pw /y
if errorlevel 1 set logerr=YES & goto loop

getremtm \\%RPLSERVER%
call setrmtod
echo %REMTIME% %REMDATE% PASSED - %COMPUTERNAME% booted OS/2.>> bootrec.log
del setrmtod.cmd
rbtos2
