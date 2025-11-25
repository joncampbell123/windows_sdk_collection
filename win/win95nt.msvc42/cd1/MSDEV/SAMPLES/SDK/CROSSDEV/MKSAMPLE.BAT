rem Build script for all SDK\CrossDev samples
rem Defaults: Win32, Release build
rem set PLATFORM=
rem         INTEL for Win32
rem         MIPS for Win32, Mips
rem         M68K for Mac 68k
rem         MPPC for Mac Ppc
rem set DEBUG=\
rem       0 for Release build, 
rem       1 for Debug build
rem      
rem Limitations:
rem	             Doesn't stop on error
rem              Doesn't stop until it finishes

if "%PLATFORM%" == "" set PLATFORM=INTEL
if "%DEBUG%" == "" set DEBUG=0

cd gdidemo
nmake -f makefile
cd ..\generic
nmake -f makefile
cd ..\getdev
nmake -f makefile
cd ..\getsys
nmake -f makefile
cd ..\gridfont
nmake -f makefile
cd ..\hooktest
nmake -f makefile
cd ..\mdi_
nmake -f makefile
cd ..\menu
nmake -f makefile
cd ..\multipad
nmake -f makefile
cd ..\owncombo
nmake -f makefile
cd ..\showdib
nmake -f makefile
cd ..\subclass
nmake -f makefile
cd ..
