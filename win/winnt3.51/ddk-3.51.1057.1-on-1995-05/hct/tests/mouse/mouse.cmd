@echo off

@rem
@rem Windows/NT HCT
@rem
@rem mouse.cmd - cmd file to run mouse tests
@rem
@rem %1 - hct drive
@rem %2 - reserved
@rem %3 - reserved
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

        moustest %1 %2 %3 %4 %5 %6 %7 %8 %9
        goto donedebug
:DEBUG
        windbg -g moustest %1 %2 %3 %4 %5 %6 %7 %8 %9
:DONEDEBUG
        copy mouse.log %HCTDIR%\logs
        del mouse.log

        goto end

:usage
        echo.
        echo Usage: mouse [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] are not used buy must be included
        echo.

:end
