set pth=%path%
set path=%root%
cd %root%
%remote%
md %home%\test
md %home%\output
cd %home%\test
set pmt=%prompt%
set prompt=
if a%machine%==a goto error
if %machine%==1 goto start
:hold
if exist %home%\sync\absent.%machine% rename %home%\sync\absent.%machine% present.%machine%
if exist %home%\sync\release1 goto hold
goto ready
:start
del %home%\test\*.* <%root%\y.dat >nul
md %home%\sync
cd %home%\sync
del *.* <%root%\y.dat >nul
copy %root%\y.dat %home%\sync\release1 >nul
copy %root%\y.dat present.1 >nul
copy %root%\y.dat absent.2  >nul
copy %root%\y.dat absent.3  >nul
copy %root%\y.dat absent.4  >nul
copy %root%\y.dat absent.5  >nul
copy %root%\y.dat absent.6  >nul
copy %root%\y.dat absent.7  >nul
copy %root%\y.dat absent.8  >nul
copy %root%\y.dat absent.9  >nul
copy %root%\y.dat absent.10 >nul
copy %root%\y.dat absent.11 >nul
copy %root%\y.dat absent.12 >nul
copy %root%\y.dat absent.13 >nul
copy %root%\y.dat absent.14 >nul
copy %root%\y.dat absent.15 >nul
copy %root%\y.dat absent.16 >nul
cd %home%\test             
echo Start the other machines.
pause
del %home%\sync\release1 >nul
:count
set number=0
if exist %home%\sync\present.1 set number=1
if exist %home%\sync\present.2 set number=2
if exist %home%\sync\present.3 set number=3
if exist %home%\sync\present.4 set number=4
if exist %home%\sync\present.5 set number=5
if exist %home%\sync\present.6 set number=6
if exist %home%\sync\present.7 set number=7
if exist %home%\sync\present.8 set number=8
if exist %home%\sync\present.9 set number=9
if exist %home%\sync\present.10 set number=10
if exist %home%\sync\present.11 set number=11
if exist %home%\sync\present.12 set number=12
if exist %home%\sync\present.13 set number=13
if exist %home%\sync\present.14 set number=14
if exist %home%\sync\present.15 set number=15
if exist %home%\sync\present.16 set number=16
if %number%==0 goto clean
echo >%home%\sync\number.bat set number=%number%
echo >%home%\sync\number.cmd set number=%number%
:ready
call sync
call %home%\sync\number
if not exist %home%\sync\present.%machine% goto out
set output=%home%\output\perf%machine%.%number%
if not %machine%==1 goto doit1
md %output%
del  %output%\*.bak >nul
ren  %output%\*.out *.bak
:doit1
call sync
call test01a
call test01b
call test02a
call test02b
call test03
call test04a
call test04b
call test05
call test06
if not %machine%==1 goto ready
del %home%\sync\present.%number% >nul
if not %number%==12 goto count
del %home%\sync\present.11 > nul
del %home%\sync\present.10 > nul
del %home%\sync\present.9 > nul
del %home%\sync\present.8 > nul
del %home%\sync\present.7 > nul
goto count
:clean
echo on
del test06dr\*.tmp >nul
rd  test06dr
del *.* <%root%\y.dat >nul
:out
set prompt=%pmt%
set pmt=
set path=%pth%
set pth=
echo Machine %machine% finished with performance tests.
goto exit
:error
echo Machine number is not defined
:exit
