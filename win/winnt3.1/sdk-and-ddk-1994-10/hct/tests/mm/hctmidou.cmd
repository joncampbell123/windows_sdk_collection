@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctmidou.cmd - cmd file to run HCT MM Midi Output test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	mouttst -g moutddk.pro
	copy moutddk.log \hct\logs
	del moutddk.log

        goto end

:usage
        echo.
	echo Usage: hctmidou [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

