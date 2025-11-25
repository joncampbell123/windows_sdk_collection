@echo off

@rem
@rem Windows/NT HCT
@rem
@rem fileio.cmd - cmd file to run disk file i/o tests
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

        setlocal
        set _basename=
        set _fs=%2
        set _type=FIXED
        set _rats=HCTDRIVE

        if "%2"=="FAT"    set _basename=diskfat

        if "%2"=="HPFS"   set _basename=diskhpfs

        if "%2"=="NTFS"   set _basename=diskntfs

        if "%2"=="FLOPPY" set _fs=FAT
        if "%2"=="FLOPPY" set _type=REMOVABLE
        if "%2"=="FLOPPY" set _basename=floppy
        if "%2"=="FLOPPY" set _rats=HCTFLOP

        if "%2"=="CDFS"   set _type=CDROM
        if "%2"=="CDFS"   set _basename=cdrom

        if "%_basename%"=="" goto usage

        set _TEST_DRIVE_NAME=%4
        set _TEST_DRIVE_TYPE=%_type%
        set _TEST_DRIVE_FS=%_fs%
	set _TEST_ALT_DRIVE_NAME=C:\

        set HCT_RUN=1

        if "%_basename%"=="floppy" %1\hct\bin\delnode /q %4*.*

        rats %_rats%.rat
        copy %_rats%.log \hct\logs\%_basename%.log
        del  %_rats%.log

        goto end

:usage
        echo.
        echo Usage: fileio [hct drive] [type] [reserved] [test drive]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [type] is NTFS, HPFS, FAT or FLOPPY
        echo       [reserved] parameters are not currently used but must have values
        echo       [test drive] is the FAT partition to test
        echo.

:end
        endlocal

