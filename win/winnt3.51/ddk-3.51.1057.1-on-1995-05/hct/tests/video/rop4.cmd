@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctgui.cmd - cmd file to run video GUI stress test
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

        if %DEBUG%==1 goto debug

        rop4 rop4.ini
        goto debugdone
:DEBUG
        windbg -g rop4 rop4.ini
:DEBUGDONE
        copy rop4.log %HCTDIR%\logs\rop4.log

        goto end

:usage
        echo.
        echo Usage: rop4 [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

