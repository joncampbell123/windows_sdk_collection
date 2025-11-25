echo off
set include=include

set BackupDir=
set RestoreDrive=
set RestoreDir=

set BackupDir=%1
set RestoreDrive=%2
set RestoreDir=%3

if  "%RestoreDrive%" == "" goto ErrorOut1
if  "%BackupDir%" == ""    goto ErrorOut1
if  "%RestoreDir%" == ""   goto ErrorOut1

call testdrvr erase.mst /run
call bkuptst1
call testdrvr ntbackup.mst /c %RestoreDrive%\%RestoreDir% /run

goto out

:ErrorOut1
echo  No test directory or drive is specified.
echo  BKUPTEST Directory Drive: Directtory ( No drive letter for Directory )

:out
