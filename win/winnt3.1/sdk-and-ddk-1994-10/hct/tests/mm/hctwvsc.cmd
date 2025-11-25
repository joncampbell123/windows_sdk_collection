@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctwavsan.cmd - cmd file to run HCT Wave Sanity check test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	wavetst -g wavsan.pro
	copy wavsan.log \hct\logs
        del wavsan.log

        goto end

:usage
        echo.
	echo Usage: hctwavsan [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

