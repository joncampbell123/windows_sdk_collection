@echo off

@rem
@rem Windows/NT HCT
@rem
@rem multiser.cmd - cmd file to run x86 mulitport serial tests on COM1
@rem
        
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

        if %DEBUG%==1 goto DEBUG

        rats multi%4.rat
        goto donedebug
:DEBUG
        windbg -g rats multi%4.rat
:DONEDEBUG
        copy multi%4.log %HCTDIR%\logs\multiser.log

        goto end

:usage
        echo.
        echo Usage: multiser [hct drive] [reserved] [reserved] [#ports] 
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [port] is the # of com ports on board (4, 8, 16)
        echo       [reserved] parameters are not currently used but must have values
        echo. 
        echo NOTE: com3 must be first com port on board!
        echo. 

:end
rem        endlocal
