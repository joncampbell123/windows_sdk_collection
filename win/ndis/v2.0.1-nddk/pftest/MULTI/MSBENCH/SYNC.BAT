@echo off
if os==DOS break on
if not %machine%==1 goto wait
cd %home%\sync
copy %home%\sync\present.1 %home%\sync\ready.1 >nul
:one
%root%\sleep 2
if exist present.2  if not exist ready.2  goto one
if exist present.3  if not exist ready.3  goto one
if exist present.4  if not exist ready.4  goto one
if exist present.5  if not exist ready.5  goto one
if exist present.6  if not exist ready.6  goto one
if exist present.7  if not exist ready.7  goto one
if exist present.8  if not exist ready.8  goto one
if exist present.9  if not exist ready.9  goto one
if exist present.10 if not exist ready.10 goto one
if exist present.11 if not exist ready.11 goto one
if exist present.12 if not exist ready.12 goto one
if exist present.13 if not exist ready.13 goto one
if exist present.14 if not exist ready.14 goto one
if exist present.15 if not exist ready.15 goto one
if exist present.16 if not exist ready.16 goto one
del release2 >nul
del %home%\sync\ready.1
copy %root%\y.dat release1 >nul
%root%\sleep 10
del release1 >nul
cd %home%\test
copy %root%\y.dat %home%\sync\release2 >nul
goto out
:wait
copy %home%\sync\present.%machine% %home%\sync\ready.%machine% >nul
:loop1
%root%\sleep 3
if not exist %home%\sync\release1 goto loop1
del %home%\sync\ready.%machine% >nul
cd %home%\test
:loop2
if not exist %home%\sync\release2 goto loop2
:out
if os==DOS break off
echo on

