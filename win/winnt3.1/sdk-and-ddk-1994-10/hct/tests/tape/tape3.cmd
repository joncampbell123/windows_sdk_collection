@echo off

@rem
@rem Windows/NT HCT
@rem
@rem tape.cmd - cmd file to run tape tests
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

	tape.exe qicscsi
	copy qicscsi.log \hct\logs

        goto end

:usage
        echo.
        echo Usage: tape [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end
