@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctwavin.cmd - cmd file to run HCT Wave Input test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	wavetst -g wavin.pro
	copy wavin.log \hct\logs
	del wavin.log

        goto end

:usage
        echo.
	echo Usage: hctwavin [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

