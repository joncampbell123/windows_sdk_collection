@REM **********************************************
@REM **          Microsoft LAN Manager           **
@REM **    Copyright(c) Microsoft Corp., 1990    **
@REM **********************************************
@ECHO OFF
SET COMSPEC=A:\COMMAND.COM
VERIFY OFF
PATH = A:\;
PROMPT $P$G 
VER
PATH=\\~~~~~~~~~~~~~~~~5\RPLFILES\LANMAN.DOS\NETPROG;\\~~~~~~~~~~~~~~~~5\RPLFILES\~~~~~~~~4;
SET COMSPEC=\\~~~~~~~~~~~~~~~~5\RPLFILES\~~~~~~~~4\COMMAND.COM
SET TMP=\\~~~~~~~~~~~~~~~~5\WRKFILES\~~~~~~~~2
@ECHO ON
A:\NET START RDR /RIPL:YES
IF ERRORLEVEL 1 PAUSE
USE E: \\~~~~~~~~~~~~~~~~~5\WRKFILES
IF ERRORLEVEL 1 PAUSE
E:
CD ~~~~~~~~2
PATH=E:\~~~~~~~~2;%PATH%
A:\NET LOGON
IF ERRORLEVEL 1 PAUSE
call autoadd ~~~~~~~~~~~~~~~~5 ~~~~~~~~2 ~~~~~~~~4
A:\RPLTERM
