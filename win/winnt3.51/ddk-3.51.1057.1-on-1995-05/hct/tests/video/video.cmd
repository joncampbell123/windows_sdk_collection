@echo off

@rem
@rem Windows/NT HCT
@rem
@rem video.cmd - cmd file to run video tests
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        vidhct -t %2
        copy videohct.log %HCTDIR%\logs\%2.log

        goto end

:usage
        echo.
        echo Usage: video [hct drive] [test] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [test] is the video test to run
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end
