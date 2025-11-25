@rem @echo off

@rem
@rem Windows/NT HCT
@rem
@rem cdrom1.cmd - cmd file to run cdrom file i/o test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage
	if "%5"=="" goto usage

        setlocal

	call cdtest.bat %4 %5

        goto end

:usage
        echo.
	echo Usage: cdrom1 [hct drive] [reserved] [reserved] [test drive] [cddrive]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo       [test drive] is the CD-ROM drive to test
        echo.

:end
        endlocal
