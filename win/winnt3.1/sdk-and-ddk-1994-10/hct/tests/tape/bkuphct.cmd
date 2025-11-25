@echo off

@rem
@rem Windows/NT HCT
@rem
@rem bkuphct.cmd - cmd file to run MS-Test backup
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage
	if "%4"=="" goto usage

setlocal
set RestoreDrive=%4
if  "%RestoreDrive%" == "" goto usage

@echo This test requires 100MB free space

@echo on
call bkuphct.bat %RestoreDrive%

goto end

:usage
        echo.
        echo Usage: bkuptest [hct drive] [reserved] [reserved]
        echo.
        echo where 
	echo 	   [restore drive] is the drive w/ 100M free that is to be restored to
        echo       [reserved] parameters are not currently used but must have values
	echo	   
        echo.


:end
endlocal
