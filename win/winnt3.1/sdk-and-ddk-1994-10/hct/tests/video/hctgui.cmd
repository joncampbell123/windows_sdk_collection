@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctgui.cmd - cmd file to run video GUI stress test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        guiman gmvideo3.scr
        copy gmvideo3.log \hct\logs\hctgui.log

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