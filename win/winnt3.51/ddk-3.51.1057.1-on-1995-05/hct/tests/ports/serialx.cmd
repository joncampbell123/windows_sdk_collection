@echo off

@rem
@rem Windows/NT HCT
@rem
@rem serialx.cmd - cmd file to run x86 serial port tests on COMx
@rem
        
        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

        set DEBUG=0

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

rem        setlocal

        del *.log

        if %DEBUG%==1 goto debug

        rats ser%4.rat
        goto donedebug
:DEBUG
        windbg -g rats ser%4.rat
:DONEDEBUG
        copy ser%4.log %HCTDIR%\logs\serialx.log

        goto end

:usage
        echo.
        echo Usage: serial [hct drive] [reserved] [reserved] [port]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo       [port] is the COM port to test
        echo.

:end
rem        endlocal
