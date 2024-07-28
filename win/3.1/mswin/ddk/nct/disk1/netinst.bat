echo off
cls
echo **********************************************************
echo **  Windows 3.1 Network Compatiblity Test Installation **
echo **********************************************************
echo 
echo 
if (%1)==() goto USAGE
if (%2)==() goto USAGE
if NOT EXIST APPLET.wtd goto NOTFLOP
copy APPLET.wtd %1\APPLET.wtd
if NOT EXIST %1\APPLET.wtd goto DIRERR
if NOT EXIST %2\win.com goto WDIRERR
goto CONT
:DIRERR
cls
echo ******************************************************
echo 
echo Sorry, the %1 directory must exist in order
echo for the NCT files to correctly install.  Please use
echo the DOS Make Directory (MD) command to create an NCT
echo directory.
echo 
echo Example:  MD C:\NCT31
echo 
echo ******************************************************
goto END
:WDIRERR
CLS
echo ******************************************************
echo 
echo Sorry, the %2 directory must exist in order
echo for the NCT files to correctly install.  Please also
echo verify that %2 represents the directory on your
echo hard drive in which Windows version 3.1 is installed.
echo 
echo ******************************************************
goto END

:NOTFLOP
CLS
echo ******************************************************
echo 
echo Sorry, please make sure that you are running the
echo Install batch file from the floppy drive containing the
echo the NCT diskette labeled "Windows 3.1 NCT Disk #1".
echo 
echo ******************************************************
goto END

:NOTDISK2
CLS
echo ******************************************************
echo 
echo Sorry, please make sure that you have inserted the
echo NCT diskette labeled "Windows 3.1 NCT Disk #2".
echo 
echo Strike any key to continue.
echo ******************************************************
pause 
goto TRYDISK2

:USAGE
CLS
echo *************************************************
echo 
echo The correct usage of the Install batch file is:
echo 
echo Install [C:\][NCTDIR] [C:\][WINDIR]
echo 
echo Where C:\ is a valid hard drive partition, NCTDIR
echo is the directory that you wish to install the NCT
echo files into, and WINDIR is the directory that you
echo have already installed Windows version 3.1.
echo 
echo Example:   INSTALL  C:\NCT31  C:\WINDOWS
echo 
echo *************************************************
goto END

:CONT
expand *.* %1                  
attrib +r %1\*.exe
ATTRIB +R %1\*.wtd
attrib +r %1\*.pif
attrib +r %1\*.bat
attrib +r %1\*.dll
attrib +r %1\*.inc

:Question
cls
echo ******************************************************
echo 
echo Do you want to install the NCT documentation (Winword
echo document) into the NCT directory?  The documentation 
echo is NOT compressed and is readily available on NCT
echo Disk #2 at any time.
echo 
echo 
echo You will also find important NCT notes in the README.TXT
echo file which has been installed in the specified NCT
echo directory.  It is also available on NCT disk #2.
echo 
ync /c yn "Do you want to install the NCT Documentation (Yes, No)?"

if NOT ERRORLEVEL 1 goto DISK2
goto ask286


:DISK2
:TRYDISK2
cd ..\disk2
if NOT EXIST nct.doc goto NOTDISK2

copy *.txt %1                  
copy *.doc %1
attrib +r %1\*.txt
ATTRIB +R %1\*.doc

:ASK286
cls
echo ******************************************************
echo 
echo Hardware Configuration:
ync /c yn "Are you running on a 286 based system (Yes, No)?"

if ERRORLEVEL 1 goto 386
copy %1\winNCT.286 %1\winNCT.ini

:386
copy %1\winplay.ini %2
copy %1\winNCT.ini  %2
echo homedir=%1 >> %2\winNCT.ini
echo homedir=%1 >> %1\winnct.ini

ren %1\winplay.ini wplay.ini
ren %1\winnct.ini wnct.ini
cls

:LASTMESSAGE
cls
echo ***********************************************************
echo 
echo Windows NCT Installation Batch File Complete
echo 
echo If you are unfamiliar with the NCT please consult the 
echo NCT documentation - "Executing the NCT" - on how to 
echo run the NCT.  
echo 
echo Also, refer to the README.TXT on this diskette for
echo further information not found in the NCT documentation.
echo (note: readme.txt is also available in the NCT directory)
echo 
echo **********************************************************
:END
