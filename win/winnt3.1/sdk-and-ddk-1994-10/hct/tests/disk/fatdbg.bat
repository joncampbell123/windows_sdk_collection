@echo off

@rem
@rem Windows/NT HCT
@rem
@rem fatdbg.bat - bat file to run standalone debug FAT file i/o tests
@rem

        if "%1"=="" goto usage

        setlocal
	set path=%PATH%;\hct\bin;
        set _basename=fatdbg
        set _fs=FAT
        set _type=FIXED
        set _rats=DBGDRIVE

        set _TEST_DRIVE_NAME=%1
        set _TEST_DRIVE_TYPE=%_type%
        set _TEST_DRIVE_FS=%_fs%
	set _TEST_ALT_DRIVE_NAME=C:\

        set HCT_RUN=1

        rats %_rats%.rat
        copy %_rats%.log \hct\logs\%_basename%.log
        del  %_rats%.log

        goto end

:usage
        echo.
        echo Usage: fatdbg [test drive]
        echo.
        echo where 
        echo       [test drive] is the FAT partition to test
        echo.

:end
        endlocal

