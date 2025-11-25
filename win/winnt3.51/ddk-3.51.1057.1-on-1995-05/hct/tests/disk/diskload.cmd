@echo off

@rem
@rem Windows/NT HCT
@rem
@rem diskload.cmd - stress disk I/O
@rem

        set DEBUG=0

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

@rem peteg 11/29/93
@rem        if "%5"=="" goto usage  removed, allow just one drive operation

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

rem         setlocal

        del %HCTDIR%\logs\diskload.log

        if %DEBUG%==1 goto DEBUG

        diskload %2 %3 %4 %5 %6 %7 %8 %9
        goto donedebug

:DEBUG
        windbg -g diskload %2 %3 %4 %5 %6 %7 %8 %9
:DONEDEBUG

        copy diskload.log %HCTDIR%\logs
        
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
rem         endlocal
