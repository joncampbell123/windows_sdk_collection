@echo off

@rem
@rem Windows/NT HCT
@rem
@rem parallel.cmd - cmd file to run parallel port test
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

rem        setlocal

        if %DEBUG%==1 goto DEBUG

        lpttest -f%HCTDIR%\tests\ports\lpttest.ini -plpt%4
        goto donedebug
:DEBUG
        windbg -g lpttest -f%HCTDIR%\tests\ports\lpttest.ini -plpt%4
:DONEDEBUG
        copy lpthct.log %HCTDIR%\logs\parallel.log

        goto end

:usage
        echo.
        echo Usage: parallel [hct drive] [reserved] [reserved] [port]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo       [port] is the LPT port to test
        echo.

:end
rem        endlocal
