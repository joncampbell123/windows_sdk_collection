@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctguic.cmd - cmd file to run video GUI stress test w/clipping on
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

        guiman gmvideo9.scr
        goto debugdone
:DEBUG
        windbg -g guiman gmvideo9.scr
:DEBUGDONE

        copy gmvideo9.log %HCTDIR%\logs\gmvideo9.log

        goto end

:usage
        echo.
        echo Usage: hctgui [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

