@echo off

@rem
@rem Windows/NT HCT
@rem
@rem cdrom1.cmd - cmd file to run cdrom file i/o test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

        setlocal

        set _TEST_DRIVE_NAME=%4\hct\tests\disk\data
        set _TEST_DRIVE_TYPE=CDROM
        set _TEST_DRIVE_FS=CDFS
        set _TEST_ALT_DRIVE_NAME=C:\

        set HCT_RUN=1

        rats hctcd.rat
        copy hctcd.log \hct\logs\cdrom1.log
        del  hctcd.log

        goto end

:usage
        echo.
        echo Usage: cdrom1 [hct drive] [reserved] [reserved] [test drive]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo       [test drive] is the CD-ROM drive to test
        echo.

:end
        endlocal

