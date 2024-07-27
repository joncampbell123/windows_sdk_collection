echo off
echo #
echo #	       Microsoft Windows Version 1.03 - Software Development Kit
echo #
echo # To use this Software Development Kit you must have the following:
echo #
echo #	   You must have the following hardware:
echo #
echo #	    o A personal computer that supports Microsoft Windows
echo #	    o 512K memory
echo #	    o A floppy disk drive configured as drive A
echo #	    o A hard disk drive configured as drive C, or second floppy
echo #	      disk drive configured as drive B
echo #	    o A graphics monitor (monochrome or color) and a graphics
echo #	      adapter card.
echo #	    o A mouse is recommended but not required.
echo #
pause
echo #
echo #	   You must have the following software:
echo #
echo #	    o DOS 2.x or 3.x
echo #	    o Microsoft Windows, version 1.03 or later
echo #	    o At least one of the following:
echo #		 Microsoft C Compiler, version 4.0
echo #		 Microsoft Pascal Compiler, version 3.3 or later
echo #		 Microsoft Macro Assembler, version 4.0
echo #
echo #
echo # If you do not have the equipment or software above, abort this
echo # batch file.
echo #
echo # This file should have been copied to your hard disk.  If not, then
echo # abort this batch file and read the README.TXT file on disk 2.
echo #
pause
cd c:\
if "%1"=="" goto usage
if "%2"=="" goto usage
if "%3"=="" goto usage
if "%4"=="" goto usage
goto ok
:usage
echo #
echo # ERROR in parameters to this batch file
echo # Usage: INSTALL binDir winDir incDir libDir
echo #	binDir = path to a subdirectory in the search path.
echo #		 Typically \bin
echo #	winDir = path to the subdirectory where Windows was installed.
echo #		 Typically \windows
echo #	incDir = path to the subdirectory where the INCLUDE environment
echo #		 variable points to. Typically \include
echo #	libDir = path to the subdirectory where the LIB environment
echo #		 variable points to. Typically \lib
goto abort
:ok
echo #
echo # Insert disk 1 in drive A:
echo #
pause
copy a:\exehdr.exe     %1
copy a:\implib.exe     %1
copy a:\lib.exe        %1
copy a:\link4.exe      %1
copy a:\make.exe       %1
copy a:\mapsym.exe     %1
copy a:\rc.exe	       %1
copy a:\rcpp.exe       %1
copy a:\symdeb.exe     %1
copy a:\winstub.exe    %1

echo #
echo # Insert disk 2 in drive A:
echo #
pause
copy a:\atrm1111.fnt   %2
copy a:\shaker.exe     %2
copy a:\dialog.exe     %2
copy a:\fontedit.exe   %2
copy a:\heapwalk.exe   %2
copy a:\iconedit.exe   %2
copy a:\slapjr.exe     %2

echo #
echo # Insert disk 3 in drive A:
echo #
pause
copy a:\inc\style.h	       %3
copy a:\inc\windows.h	       %3
copy a:\lib\mlibw.lib	       %4
copy a:\lib\mwinlibc.lib       %4
copy a:\lib\slibw.lib	       %4
copy a:\lib\swinlibc.lib       %4
copy a:\lib\clibw.lib	       %4
copy a:\lib\llibw.lib	       %4
copy a:\lib\cwinlibc.lib       %4
copy a:\lib\lwinlibc.lib       %4

echo #
echo # Insert disk 4 in drive A:
echo #
pause
copy a:\cmacros.inc	       %3
copy a:\pascal.lib	       %4
copy a:\paslibw.lib	       %4
copy a:\windows.inc	       %3

:abort
