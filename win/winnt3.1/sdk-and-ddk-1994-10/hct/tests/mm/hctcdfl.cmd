@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctcdfl.cmd - cmd file to run HCT MM MCI CD Audio "Full" test
@rem now certification test

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	mcigen -g mcicdfl.pro
	copy mcicdfl.log \hct\logs
	del mcicdfl.log

        goto end

:usage
        echo.
	echo Usage: hctcdfl [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

