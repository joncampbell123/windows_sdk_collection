@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctcdmnl.cmd - cmd file to run HCT MM MCI CD Audio Manual test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	mcigen -g mcicdmnl.pro
	copy mcicdmnl.log \hct\logs
	del mcicdmnl.log

        goto end

:usage
        echo.
	echo Usage: hctcdmnl [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

