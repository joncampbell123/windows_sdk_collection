@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctwavou.cmd - cmd file to run HCT MM WAV Output test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	wavetst -g wavout.pro
	copy wavout.log \hct\logs
        del wavout.log

        goto end

:usage
        echo.
	echo Usage: hctwavou [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

