@echo off
rem test06.bat
rem This batch file measures the time of search operations.
rem Directory TEST06DR is created, then 200 files are created in it.
rem The Search is done with the following patterns:
rem   1) f*.* to find every file
rem   2) f????0.* to find every 10th file
rem   3) fz which will find no files
rem The results of the tests are written to file test06.out.
rem The performance of this test depends on the total number of entries
rem that have been created in directory TEST06DR. To make the test
rem "fair", the directory is recreated before the test is run. 
echo on

if not %machine%==1 goto START

if not exist TEST06DR\*.tmp goto CREA_DIR
rem Subdirectory TEST06DR already exists, deleting it
del test06dr\*.* <%root%\y.dat >NUL
rmdir test06dr

:CREA_DIR
rem Creating subdirectory TEST06DR
mkdir TEST06DR

rem Creating files
fu /i:TEST06DR /n:1 /m:1 /a:200 /c >NUL

:START
rem Find All Entries
call sync
:loop1
if not %machine%==1 %root%\fu /i:TEST06DR /x:f*.*     /n:90 /m:1 /a:200 /h >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop1
if %machine%==1 %root%\sleep 3
if %machine%==1 %root%\fu /i:TEST06DR /x:f*.*     /n:30 /m:1 /a:200 /h >>%output%\test06.out

rem Find every 10th entry
call sync
:loop2
if not %machine%==1 %root%\fu /i:TEST06DR /x:f????0.* /n:150 /m:1 /a:200 /h >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop2
if %machine%==1 %root%\sleep 3
if %machine%==1 %root%\fu /i:TEST06DR /x:f????0.* /n:30 /m:1 /a:200 /h >>%output%\test06.out

rem Find no files
call sync
:loop3
if not %machine%==1 %root%\fu /i:TEST06DR /x:fz /n:500 /m:1 /a:200 /h >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop3
if %machine%==1 %root%\sleep 3
if %machine%==1 %root%\fu /i:TEST06DR /x:fz /n:30 /m:1 /a:200 /h >>%output%\test06.out
