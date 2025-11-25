@echo off

@rem
@rem Windows/NT HCT
@rem
@rem serial.cmd - cmd file to run x86 serial port tests
@rem
        
        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

        setlocal

        del *.log
        rats ser%4%5.rat
        copy ser%4%5.log \hct\logs\serial.log

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
        endlocal
