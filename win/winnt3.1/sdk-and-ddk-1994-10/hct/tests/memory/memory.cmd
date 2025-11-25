@echo off

@rem
@rem Windows/NT HCT
@rem
@rem memory.cmd - run virtual memory tests
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        rats vmemhct.rat
        copy vmemhct.log \hct\logs

        goto end

:usage
        echo Usage: memory [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end
