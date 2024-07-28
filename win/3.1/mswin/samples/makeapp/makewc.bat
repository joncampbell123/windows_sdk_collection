@echo off
set _dstdir=%1
set _file=%2
set _class=%3
set _type=%4
set _ptr=%5
set _prefix=%6
set _app=%7

if "%_dstdir%"=="" goto help
if "%_file%"=="" goto help
if "%_class%"=="" goto help
if "%_type%"=="" goto help
if "%_ptr"=="" goto help
if "%_prefix%"=="" goto help
if "%_app%"=="" goto help
if not exist .\client.h goto help
goto gotparms
:help
echo.
echo MAKEWC - Generic Window class source generator
echo.
echo Usage:   makewc destdir filename ClassName TYPENAME ptrtype AppPrefix_ appname
echo.
echo Example: makewc c:\myapp mywnd MyWnd MYWND pmywnd MyApp_ myapp
echo.
echo Must be run from MAKEAPP directory.
echo.
goto exit

:gotparms
echo.
echo Creating %_class% window class in %_dstdir%\%_file%.c...

call makewc2 makewc.c %_dstdir%\%_file%.c %_class% %_type% %_prefix% %_app% %_ptr%
call makewc2 makewc.h %_dstdir%\%_file%.h %_class% %_type% %_prefix% %_app% %_ptr%
echo.
echo Done!
echo.
cd %_dstdir%

:exit
set _dstdir=
set _file=
set _class=
set _type=
set _ptr=
set _prefix=
set _app=
