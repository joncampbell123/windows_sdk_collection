@echo off

@rem
@rem Windows/NT HCT
@rem
@rem cdfsdbg.bat - bat file to manually run debug cdfs file i/o tests
@rem

        if "%1"=="" goto usage

        setlocal
	set path=%PATH%;\hct\bin;
        set _basename=cdfsdbg
        set _fs=CDFS
        set _type=CDROM
        set _rats=HCTDRIVE

        set _TEST_DRIVE_NAME=%1
        set _TEST_DRIVE_TYPE=%_type%
        set _TEST_DRIVE_FS=%_fs%
	set _TEST_ALT_DRIVE_NAME=C:\

        set HCT_RUN=1

        if "%_basename%"=="floppy" ..\..\bin\delnode /q %1*.*

        rats %_rats%.rat
        copy %_rats%.log \hct\logs\%_basename%.log
        del  %_rats%.log

        goto end

:usage
        echo.
        echo Usage: cdfsdbg [test drive]
        echo.
        echo where 
        echo       [test drive] is the cdrom drive letter (w/ colon e.g. c: to test)
        echo.

:end
        endlocal

