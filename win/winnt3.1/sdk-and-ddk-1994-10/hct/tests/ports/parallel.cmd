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

        setlocal

        lpttest -f\hct\tests\ports\lpttest.ini -plpt%4
        copy lpthct.log \hct\logs\parallel.log

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
        endlocal
