@echo off
@rem 
@rem Windows/NT HCT
@rem
@rem bkuphct.bat - batch file to run MS-Test backup from command line
@rem

if  "%1" == "" goto usage

@rem @echo This test requires 100MB free space
@rem @Pause

rem  Initialize new log file, overwriting any previous one...
echo Starting Tape Backup Test... >bkuphct.log

rem Set environment variable WINDIRONLY to WINDIR without drive letter
setlocal
windir.exe
call windir.bat
del windir.bat

rem make sure the target directory doesn't exist
if exist %1\hct%windironly% rd /s %1\hct%windironly% <y

rem  And do the real testing...
set include=include
testdrvr erase.mst /run
NTBACKUP backup %windir%\system32 /T NORMAL /L %windir%\backup1.log /V
testdrvr ntbackup.mst /c %1\HCT /run

if exist %1\hct\%windironly% goto cmp
echo Tape not restored, no comparison done. >>bkuphct.log
goto skip

:cmp
tapecomp %1 >>bkuphct.log

echo Clean up %1\hct\*.tmp

:skip
copy bkuphct.log \hct\logs

goto end

:usage
     echo.
     echo Usage: bkuptest [restore drive]
     echo.
     echo where 
	echo	   [restore drive] is the drive w/ 100M free that is to be restored to
	echo	   
     echo.


:end
endlocal
