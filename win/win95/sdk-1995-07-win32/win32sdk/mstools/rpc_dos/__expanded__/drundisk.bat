@echo off
Rem Make the RPC runtime only setup disks for Dos.

set RPCSRC=a:
set RPCDST=a:

if not `%1 == ` set RPCSRC=%1
if not `%2 == ` set RPCDST=%2

pause Put distribution disk 1 into drive A: and then hit enter.

if not exist dsetup.exe goto MissingFile
if not exist dsetup.sus goto MissingFile

if not exist %RPCSRC%\rpcns.rpc goto MissingFile
if not exist %RPCSRC%\rpcnsmgm.rpc goto MissingFile
if not exist %RPCSRC%\rpcnslm.rpc goto MissingFile

if not exist %RPCSRC%\rpc16c1.rpc goto MissingFile
if not exist %RPCSRC%\rpc16c3.rpc goto MissingFile
if not exist %RPCSRC%\rpc16c4.rpc goto MissingFile
if not exist %RPCSRC%\rpc16c5.rpc goto MissingFile
if not exist %RPCSRC%\rpc16c6.rpc goto MissingFile

md %RPCDST%\rpcrun
md %RPCDST%\rpctrans

copy dsetup.exe %RPCDST%\setup.exe
copy dsetup.sus %RPCDST%\setup.sus

copy %RPCSRC%\rpcns.rpc	   %RPCDST%\rpcrun
copy %RPCSRC%\rpcnsmgm.rpc  %RPCDST%\rpcrun
copy %RPCSRC%\rpcnslm.rpc   %RPCDST%\rpcrun
copy %RPCSRC%\rpc16c1.rpc   %RPCDST%\rpctrans
copy %RPCSRC%\rpc16c3.rpc   %RPCDST%\rpctrans
copy %RPCSRC%\rpc16c4.rpc   %RPCDST%\rpctrans
copy %RPCSRC%\rpc16c5.rpc   %RPCDST%\rpctrans
copy %RPCSRC%\rpc16c6.rpc   %RPCDST%\rpctrans

goto Done

:MissingFile

Echo A runtime file is missing.  The first argument (%1) maybe incorrect.
echo You may not have installed the MS-DOS support correctly.
echo Re-run the Windows SDK install program and make sure you load
echo all the transports for MS-DOS.

goto Done

:Usage

Echo Usage:
echo	   drundisk Rpc_runtime_path floppy_ disk_letter
echo .
echo	   Example: drundisk c:\dos b:

goto Done

:Done

set RPCSRC=
set RPCDST=
