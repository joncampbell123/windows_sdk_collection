REM Win32s Build Script
REM -------------------
REM

REM Build Win32s Setup DLL extension
cd iniupd
nmake
cd ..

REM Build Win32s Setup user interface dialogs
cd bldcui
nmake
cd ..

REM Build floppy image from retail binaries
dsklayt2 w32s.lyt ..\floppy\32sinst.inf /f /c nodebug /d ..\floppy
