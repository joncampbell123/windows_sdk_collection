@echo off
set _dstdir=%1
set _file=%2
set _class=%3

if "%_dstdir%"=="" goto help
if "%_file%"=="" goto help
if "%_class%"=="" goto help
if not exist .\makeapp.rc goto help
goto gotparms
:help
echo.
echo MAKEAPP - Generic Windows application generator
echo.
echo Usage:   makeapp destdir filename ClassName
echo.
echo Example: makeapp c:\myapp myapp MyApp
echo          nmake
echo.
echo Must be run from MAKEAPP directory.
echo.
goto exit

:gotparms
echo.
echo Creating %_class% application in %_dstdir%...

if exist %_dstdir%\con goto nomkdir
mkdir %_dstdir%
:nomkdir
call makeapp2 makefile    %_dstdir%\makefile   %_file% %_class%
call makeapp2 makeapp.rc  %_dstdir%\%_file%.rc  %_file% %_class%
copy          makeapp.ico %_dstdir%\%_file%.ico >nul
call makeapp2 makeapp.def %_dstdir%\%_file%.def %_file% %_class%
call makeapp2 makeapp.rc  %_dstdir%\%_file%.rc  %_file% %_class%
call makeapp2 makeapp.ver %_dstdir%\%_file%.ver %_file% %_class%
call makeapp2 makeapp.h   %_dstdir%\%_file%.h   %_file% %_class%
call makeapp2 makeapp.dlg %_dstdir%\%_file%.dlg %_file% %_class%
call makeapp2 app.c       %_dstdir%\app.c      %_file% %_class%
call makeapp2 app.h       %_dstdir%\app.h      %_file% %_class%
call makeapp2 frame.c     %_dstdir%\frame.c    %_file% %_class%
call makeapp2 frame.h     %_dstdir%\frame.h    %_file% %_class%
call makeapp2 client.c    %_dstdir%\client.c   %_file% %_class%
call makeapp2 client.h    %_dstdir%\client.h   %_file% %_class%
call makeapp2 dlg.c       %_dstdir%\dlg.c      %_file% %_class%
call makeapp2 dlg.h       %_dstdir%\dlg.h      %_file% %_class%
call makeapp2 dlgdefs.h   %_dstdir%\dlgdefs.h  %_file% %_class%
call makeapp2 menu.h      %_dstdir%\menu.h     %_file% %_class%

echo.
echo Done!
echo.
echo Type nmake to create %_file%.exe
echo.
cd %_dstdir%

:exit
set _dstdir=
set _file=
set _class=
