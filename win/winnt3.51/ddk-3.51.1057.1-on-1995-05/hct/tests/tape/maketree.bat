REM     Template Batch file for creating a sample backup tree.
REM     For use with BACKUP.PCD, an MT 3.0 test script.

REM     The name of this file is specified when you run the MT
REM     Script.  Due to limitions in MT it currently must be a
REM     BAT file, not a CMD file.

TITLE *** TREE CREATION IN PROCESS ***

REM     Edit the lines below to call whatever tools you have at
REM     your disposal to create a tree on the target drive.

REM     You have the following environmental variables at your
REM     disposal. Niether includes a trailing "\".
REM     %~TREEDRIVE%   = Driveletter where the tree is to be created.
REM     %~TREEPATH%    = Fully qualified path where the tree is to be created.
REM     %~DATA%        = Fully qualified path where logs will be stored.
REM     %~CURDIR%      = Directory from which the script is running.

REM     You should not use the "START" command from this file,
REM     All processing should happen in this CMD window.

REM     This file does NOT have to be named "MAKETREE.BAT", but
REM     the script defaults to that name.

REM     DO NOT CHANGE ANYTHING ABOVE THIS LINE ******************


%~TREEDRIVE%
cd %~TREEPATH%

%~CURDIR%\USEDISK %~TREEPATH% 10 10 200 15 %mb_use% 2 0
rem peteg 5/27/94 bug bug use bkuphct.cmd set parameter for
rem tree path\drive for now
rem%~CURDIR%\USEDISK %tree_drv% 10 10 10 15 %mb_use% 2 0
