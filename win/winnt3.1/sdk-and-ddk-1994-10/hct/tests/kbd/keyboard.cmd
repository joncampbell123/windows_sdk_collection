@echo off

@rem
@rem Windows/NT HCT
@rem
@rem keyboard.cmd - cmd file to run kbd test
@rem
@rem %1 - hct drive
@rem %2 - reserved
@rem %3 - reserved
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        kbtests %1 %2 %3 %4 %5 %6 %7 %8 %9
        copy keyboard.log \hct\logs
        del keyboard.log

        goto end

:usage
        echo.
        echo Usage: keyboard [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] are not used but must have values
        echo.

:end
