@echo off

@rem
@rem Windows/NT HCT
@rem
@rem stress.cmd - start various system stress tests
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        del *.log
	procload /l /n
        runstrss %1 %2 %3 %4 %5 %6 %7 %8 %9
	procload /l /s
        copy stress.log \hct\logs\stress.log
	copy procload.log \hct\logs\mp.log

        goto end

:usage
        echo Usage: stress [hct drive] [hours] [number of stress tests]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [hours] is the number of hours to run the stress tests
        echo       [number of stress tests] is how many tests will run
        echo.

:end
