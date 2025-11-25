REM     This is a sample batch file illustrating how
REM     to call BACKUP.PCD from a batch file.

REM ======================================================================

REM     THIS SAMPLE IS NONE OPERATIONAL

TITLE *** BACKUP TESTING IN PROGRESS - VARIATION ONE ***

REM Variation ONE
REM Logfile: VAR_ONE.LOG
REM Options: Test all drives, run net tests, UI tests, registry tests
REM Create tree with TREE_ONE.BAT

REM First make some net connections to backup

net use O: /d
net use T: /d

net use O: \\popcorn\public
net use T: \\kernel\scratch

REM Now set the environmental variable used by the test

SET NETDRIVES=O: T:

REM Now call the script (pasted in from initial dialog.)

mtrun backup.pcd /C "-l:VAR_ONE.LOG  -t:G:\  -d:J:\MT\BACKUPUI\  -bat:TREE_ONE.BAT  -all  -net  -ui"


REM ======================================================================

TITLE *** BACKUP TESTING IN PROGRESS - VARIATION TWO ***

REM Variation TWO
REM Logfile: VAR_TWO.LOG
REM Options: Test current drive, don't run net tests, or UI tests, or registry tests
REM Create tree with TREE_TWO.BAT

REM Since I am not testing net drives, no need to set NETDRIVES

REM So just call the script. (pasted in from initial dialog.)

mtrun backup.pcd /C "-l:VAR_TWO.LOG  -t:G:\  -d:J:\MT\BACKUPUI\  -bat:TREE_TWO.BAT  -nr"
