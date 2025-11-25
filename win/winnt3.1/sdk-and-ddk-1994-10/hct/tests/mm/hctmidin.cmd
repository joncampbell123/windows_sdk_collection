@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctmidin.cmd - cmd file to run HCT MM Midi Input test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	mintst minddk.pro -g
	copy minddk.log \hct\logs
	del minddk.log

        goto end

:usage
        echo.
	echo Usage: hctmidin [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

