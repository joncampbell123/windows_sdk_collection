@echo off

@rem
@rem Windows/NT HCT
@rem
@rem diskload.cmd - stress disk I/O
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage
        if "%5"=="" goto usage

        setlocal

        del \hct\logs\diskload.log

        diskload %2 %3 %4 %5 %6 %7 %8 %9
        copy diskload.log \hct\logs
        
        echo.
        echo Ignore any error messages here
        echo.
        del c:\*.dl d:\*.dl e:\*.dl 

        goto end

:usage
        echo.
        echo Usage: diskload [hct drive] [reserved] [reserved] [D:n] [D:n]...
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo       [D:n] are drive:thread pairs, where D is the drive to test and 
        echo       n is the number of I/O threads
        echo.
        echo for example, diskload d: -t30 junk c:10 d:10 a:4
        echo.
        echo would run 10 threads on C: and D: and 4 on A: for 30 minutes

:end
        endlocal
