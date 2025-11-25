rem @echo off
if "%4" == ""    goto sethelp
if "%4" == "/?"  goto sethelp


        set DEBUG=0

if "%3"=="-DEBUG" set DEBUG=1
if "%4"=="-DEBUG" set DEBUG=1
if "%5"=="-DEBUG" set DEBUG=1
if "%6"=="-DEBUG" set DEBUG=1
if "%7"=="-DEBUG" set DEBUG=1
if "%8"=="-DEBUG" set DEBUG=1
if "%9"=="-DEBUG" set DEBUG=1

if not "%4" == ""  set manp="/d:%4
if not "%5" == ""  set manp=%manp% /d:%5
if not "%6" == ""  set manp=%manp% /d:%6
if not "%7" == ""  set manp=%manp% /d:%7
if not "%8" == ""  set manp=%manp% /d:%8
if not "%9" == ""  set manp=%manp% /d:%9

set manp=%manp% "

if %DEBUG%==1 goto DEBUG

:start2
mtrun manual /c %manp%
goto donedebug

:DEBUG
windbg -g mtrun manual /c %manp%

:DONEDEBUG

goto end

:sethelp
  set manp="/?"
  goto start2



:end
  copy manual.log  %HCTDIR%\logs
  set manp=
