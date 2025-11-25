@echo off

@rem
@rem Windows/NT HCT
@rem
@rem mappedio.cmd - cmd file to run mapped i/o disk test
@rem

        set DEBUG=0

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

        if "%2"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%3"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%4"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%5"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%6"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%7"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%8"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%9"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF

REM delete old log file
del  %HCTDIR%\logs\mapiohct.log

       if %DEBUG%==1 goto DEBUG
:START 
       rats maphct%4.rat
       goto donedebug
:DEBUG
       windbg -g rats maphct%4.rat
:DONEDEBUG

       type  maphct%4.log >>  %HCTDIR%\logs\mapiohct.log
       del %4:\mapio.dat

REM is there another drive to test.
if  "%5"=="" goto end
    shift
    goto START

goto end

:usage
        echo.
        echo Usage: mappedio [hct drive] [reserved] [reserved] [partition]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo       [partition] is the partition to stress and
        echo.

:end
