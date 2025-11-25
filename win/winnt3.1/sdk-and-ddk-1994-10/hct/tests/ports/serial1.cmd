@echo off

@rem
@rem Windows/NT HCT
@rem
@rem serial1.cmd - cmd file to run x86 serial port tests on COM1
@rem
        
        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        setlocal

        del *.log
        rats ser%2.rat
        copy ser%2.log \hct\logs\serial1.log

        goto end

:usage
        echo.
        echo Usage: serial [hct drive] [port] [reserved] 
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [port] is the COM port to test
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end
        endlocal
