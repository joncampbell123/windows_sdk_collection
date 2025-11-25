@echo off

@rem
@rem Windows/NT HCT
@rem
@rem vvideo.bat - calls VDM video tests
@rem
@rem %1 - mode (in hex) to run tests in
@rem %2 - logging level
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage

        del *.log

        vvdgfont -uq%1 -l%2
        vvdmem   -uq%1 -l%2
        vvdpos   -uq%1 -l%2
        vvdvcolr -uq%1 -l%2
        vvdpalet -uq%1 -l%2
rem	 vvdpixel -uq%1 -l%2
rem        vvdchrio -uq%1 -l%2
        vvdvstat -uq%1 -l%2
        vvdshape -uq%1 -l%2
rem	 vvdscrol -uq%1 -l%2

        findstr "+TEST_RESULT+" *.log | findstr FAIL >> vdmvga.tmp

        goto end

:usage
        echo.
        echo Usage: vdmvga [reserved] [reserved] [reserved]
        echo.
        echo where
        echo       [reserved] are not used but must have values
        echo.

:end
