@echo off

@rem
@rem Windows/NT HCT
@rem
@rem sound.cmd - cmd file to run sound tests
@rem
@rem %1 - hct drive
@rem %2 - test base name - "speaker" or "soundcrd"
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

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
        set _basename=%2
        if "%_basename%"=="" goto usage

 echo.
	echo.
	echo.
	echo.
	pause
        if %DEBUG%==1 goto DEBUG

        rats sndhct.rat
        goto donedebug
:DEBUG
        windbg -g rats sndhct.rat
:DONEDEBUG
        copy sndhct.log %HCTDIR%\logs\%_basename%.log

        goto end

:usage
        echo.
        echo Usage: sound [hct drive] [test base name] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end
rem        endlocal
