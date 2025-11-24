@echo off
if not "%7"=="" goto dotest
:Instruct
ECHO CD LOCAL Test Batch File - Locates CD directory to test
ECHO  %%1 = CDROM Drive letter
ECHO  %%2 = Path to write to
ECHO  %%3 = Minimum K in CD directory
ECHO  %%4 = Maximum K in CD directory
ECHO  %%5 = Minimum Depth in CD directory
ECHO  %%6 = Working Path
ECHO  %%7 = Logfile
ECHO .
ECHO  EXAMPLE: CDLOC E C:\TEMPDIR 100 2048 2 C:\CT\BCT C:\Logfile 
ECHO     1) Create C:\TEMPDIR
ECHO     2) Run WALKD.EXE on drive E:
ECHO     3) Find a directory on E: of size 100K to 2048K, with at least two subdirs
ECHO     4) XCOPY the contents of that directory to C:\TEMPDIR
ECHO     5) Run TCOMP to compare then
ECHO     6) Cleanup directory C:\TEMPDIR
ECHO     7) Place log in C:\Logfile
ECHO .
goto Gone
:dotest
if "%8"=="Stage2" goto Stage2
if not "%8"=="" goto Instruct

REM Step 1
ECHO Beginning CDROM drive %1 test
ECHO Beginning CDROM drive %1 test >%7
ECHO Prepping %2
ECHO Prepping %2 >>%7
MD %2
DELTREE /Y %2\*.*

REM Step 2
ECHO Walking %1:
ECHO Walking %1: >>%7
%6\WALKD %1:
if errorlevel 1 goto Error

REM Step 3
ECHO Searching for directory on %1:, between %3K and %4K, with at least %5 subdirs
ECHO Searching for directory on %1:, between %3K and %4K, with at least %5 subdirs >>%7
%6\DIRSIZE %1:\ %3 %4 %5 >%6\DIRSIZE%1.TMP
if errorlevel 1 goto WholeCD
copy %6\CDLOCHLP.TMP+%6\DIRSIZE%1.TMP %6\CDLOC3%1.BAT >NUL
REM CDLOCHLP will call this batch again with directory in %9
%6\CDLOC3%1.BAT %1 %2 %3 %4 %5 %6 %7 Stage2
:WholeCD
ECHO No match, so no test
ECHO Unable to find directory matching requested size on CD >>%7
goto Disable

:Stage2

REM Step 4
ECHO Using directory %9 for tree copy/compare
ECHO Using directory %9 for tree copy/compare >>%7
ECHO Xcopying/verifying %9 to %2
ECHO Xcopying/verifying %9 to %2 >>%7
XCOPY %9 %2 /s /e /v
if errorlevel 1 goto Error

REM Step 5
ECHO Tree comparing %9 to %2
ECHO Tree comparing %9 to %2 >>%7
TCOMP %9 %2
if errorlevel 1 goto Error

REM Step 7
echo Cleaning %2
echo Cleaning %2 >>%7
DELTREE /Y %2\*.*
DEL %6\CDLOC3%1.BAT
DEL %6\DIRSIZE%1.TMP
RD %2
ECHO All Tests PASSED!
echo Tests passed >>%7
%6\return4
goto Gone

:Disable
echo Cleaning %2
echo Cleaning %2 >>%7
DELTREE /Y %2\*.*
DEL %6\CDLOC3%1.BAT
DEL %6\DIRSIZE%1.TMP
RD %2
ECHO Test Disabled!
echo Tests disabled >>%7
%6\return3
goto Gone

:Error
ECHO ***   TEST FAILED!   ***
echo Exiting due to test error (test failed) >>%7
echo Cleaning %2
echo Cleaning %2 >>%7
DELTREE /Y %2\*.*
DEL %6\CDLOC3%1.BAT
DEL %6\DIRSIZE%1.TMP
RD %2
%6\return2

:Gone
%6\ERRVALUE

