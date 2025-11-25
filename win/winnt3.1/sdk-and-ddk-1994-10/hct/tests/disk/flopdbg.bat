@echo off

@rem
@rem Windows/NT HCT
@rem
@rem dbgflop.bat - bat file to run floppy disk file i/o tests
@rem

        if "%1"=="" goto usage

        setlocal
        set _basename=flopdbg
        set _fs=FAT
        set _type=REMOVABLE
        set _rats=HCTFLOP

        if "%_basename%"=="" goto usage

        set _TEST_DRIVE_NAME=%4
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
        echo Usage: dbgflop [test drive]
        echo.
        echo where 
        echo       [test drive] is the FLOPPY FAT partition (w/ colon e.g a:) to test
        echo.

:end
        endlocal

