@REM Win32s Build Script
@REM -------------------
@REM

@REM Build Win32s Setup DLL extension
cd iniupd
nmake
cd ..
copy iniupd\iniupd.dll

@REM Build Win32s Setup user interface dialogs
cd bldcui
nmake
cd ..
copy bldcui\mscuistf.dll

@if "%1"=="ole32s" goto ole32s
@if "%1"=="OLE32S" goto ole32s

@REM Build floppy image from retail binaries
dsklayt2 w32s.lyt ..\disks\retail\win32s\disk1\32sinst.inf /k144 /f /c comp\nodebug /d ..\disks\retail\win32s

@if "%1"=="win32s" goto exit
@if "%1"=="WIN32S" goto exit

:ole32s
@REM Build floppy image from retail binaries
dsklayt2 ow32s.lyt ..\disks\retail\ole32s\disk1\32sinst.inf /k144 /f /c comp\nodebug /d ..\disks\retail\ole32s

:exit

