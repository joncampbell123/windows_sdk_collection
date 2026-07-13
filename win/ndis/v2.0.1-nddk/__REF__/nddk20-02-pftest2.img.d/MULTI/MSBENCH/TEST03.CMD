@echo off

set ltmp=c:\temp

rem test03.bat
rem This batch file runs copy tests. It is assumed that:
rem   1) the default drive is T: which is redirected to the test server 
rem   2) the client has a hard disk, drive C:
rem The results of the tests are written to file test03.out.
rem The tests use a wildcard pattern of cf*.*
rem The file sizes are: .0, 2048, 48KB, .0KB, and 1MB.
rem  The names are the files are cf.0.01, cf.0.02,... cf.0.0nn,
rem  cf2048.01,..., cf48K.01, etc.
echo on

if not exist %home%\copyfils\cf*.0%machine% copy %home%\copyfils\cf*.01 %home%\copyfils\cf*.0%machine%
attrib +r cf*.0%machine%
del %home%\copyfils\rc*.0%machine%

call sync
if not %machine%==1 copy %home%\copyfils\cf*.0%machine% %home%\copyfils\rc*.0%machine% >nul
:loop1
if not %machine%==1 copy %home%\copyfils\cf*.0%machine% %home%\copyfils\rc*.0%machine% >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop1
if not %machine%==1 goto next1
%root%\sleep 3
time 0
copy %home%\copyfils\cf*.0%machine% %home%\copyfils\rc*.0%machine% >>%output%\test03.out
time >>%output%\test03.out <%root%\cr.dat
:next1

call sync
md %ltmp%
del %ltmp%\rc*.0%machine%

call sync
if not %machine%==1 copy %home%\copyfils\cf*.0%machine% %ltmp%\rc*.0%machine% >nul
:loop2
if not %machine%==1 copy %home%\copyfils\cf*.0%machine% %ltmp%\rc*.0%machine% >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop2
if not %machine%==1 goto next2
%root%\sleep 3
time 0
copy %home%\copyfils\cf*.0%machine% %ltmp%\rc*.0%machine% >>%output%\test03.out
time >>%output%\test03.out <%root%\cr.dat
:next2
call sync
rename %ltmp%\rc*.0%machine% cf*.0%machine%
del %home%\copyfils\rc*.0%machine%

call sync
if not %machine%==1 copy %ltmp%\cf*.0%machine% %home%\copyfils\rc*.0%machine% >nul
:loop3
if not %machine%==1 copy %ltmp%\cf*.0%machine% %home%\copyfils\rc*.0%machine% >nul
if not %machine%==1 if not exist %home%\sync\ready.1 goto loop3
if not %machine%==1 goto next3
%root%\sleep 3
time 0
copy %ltmp%\cf*.0%machine% %home%\copyfils\rc*.0%machine% >>%output%\test03.out
time >>%output%\test03.out <%root%\cr.dat
:next3

del %ltmp%\*.0%machine%
rd %ltmp%

