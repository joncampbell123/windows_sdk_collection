echo off
NTBACKUP backup %BackupDir% /T NORMAL /L backup1.log /V
goto OUT
:Out
