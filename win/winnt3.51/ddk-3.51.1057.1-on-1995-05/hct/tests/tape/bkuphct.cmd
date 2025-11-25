@echo off


@rem peteg 5/11/94: drastic changes to use new backup script
@rem                from John Hazen
@rem
@rem Windows/NT HCT
@rem
@rem bkuphct.cmd - cmd file to run MS-Test backup
@rem

       if "%1"=="" goto usage
       if "%2"=="" goto usage
       if "%3"=="" goto usage
	      if "%4"=="" goto usage
       if "%5"=="" goto usage
     rem if "%6"=="" goto usage

setlocal

rem peteg 5/24/94: changed to use new funct

set OTHERDRIVES=%6 %7 %8 %9
rem regset "SOFTWARE\MICROSOFT\NTBACKUP\BACKUP ENGINE" "Sort Bsd List" 0
rem set mb_use=%6
set tree_drv=%4

rem peteg 6/8/94: remove all drives option for Beta2 back in later
rem if "%5" == "y" goto alldrives
rem if "%5" == "Y" goto alldrives
rem if "%5" == "n" goto defdrive
rem if "%5" == "N" goto defdrive
set mb_use=%5
goto defdrive

:ALLDRIVES
rem mtrun backup.pcd /C "-l:BKUPHCT.LOG  -t:%4  -d:%hctdir%\tests\tape\  -bat:MAKETREE.BAT  -all  -net -nr"
mtrun backup.pcd /C "-l:BKUPHCT.LOG  -t:%4 -d:%hctdir%\tests\tape\ -bat:MAKETREE.BAT -all -other -nr -nw"
goto end

:DEFDRIVE
rem mtrun backup.pcd /C "-l:BKUPHCT.LOG  -t:%4  -d:%hctdir%\tests\tape\  -bat:MAKETREE.BAT  -net -nr"
mtrun backup.pcd /C "-l:BKUPHCT.LOG  -t:%4 -d:%hctdir%\tests\tape\ -bat:MAKETREE.BAT -other -nr -nw"
goto end

:usage
        echo.
        echo Usage: bkuptest [hct drive] [reserved] [reserved] [tree drive - must be blank] [backup drive] ...
        echo.
        echo where
        echo  [hct drive] is drive where hct's being run from
	       echo  [restore drive] is the drive w/ 100M free that is to be restored to
        echo  [reserved] parameters are not currently used but must have values
        echo  [tree drive] a drive where a tree is created, backed up, and restored to - must be blank
        echo  [backup drive](s) is a list of additional drives to backup
	       echo
        echo.


:end


rem peteg 1/14/94 change registry setting back
copy bkuphct.log %hctdir%\logs
rem regset "SOFTWARE\MICROSOFT\NTBACKUP\BACKUP ENGINE" "Sort Bsd List" 1
endlocal

