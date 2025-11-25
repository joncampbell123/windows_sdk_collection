@echo off

@rem
@rem Windows/NT HCT
@rem
@rem sound.cmd - cmd file to run sound tests
@rem
@rem %1 - hct drive
@rem %2 - test base name - "speaker" or "soundcrd"
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        setlocal
        set _basename=%2
        if "%_basename%"=="" goto usage

        echo.
	echo.
	echo.
	echo.
	pause

        rats sndhct.rat
        copy sndhct.log \hct\logs\%_basename%.log

        goto end

:usage
        echo.
        echo Usage: sound [hct drive] [test base name] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end
        endlocal


