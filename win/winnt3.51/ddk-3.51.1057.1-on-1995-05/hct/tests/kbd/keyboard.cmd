@echo off

@rem
@rem Windows/NT HCT
@rem
@rem keyboard.cmd - cmd file to run kbd test
@rem
@rem %1 - hct drive
@rem %2 - reserved
@rem %3 - reserved
@rem

        set DEBUG=0

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

        if %DEBUG%==1 goto DEBUG

        kbtests %1 %2 %3 %4 %5 %6 %7 %8 %9
        goto DEBUGDONE
:DEBUG
        windbg -g kbtests %1 %2 %3 %4 %5 %6 %7 %8 %9

:DEBUGDONE
        copy keyboard.log %HCTDIR%\logs
        del keyboard.log

        goto end

:usage
        echo.
        echo Usage: keyboard [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] are not used but must have values
        echo.

:end
