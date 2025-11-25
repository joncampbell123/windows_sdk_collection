@echo off

@rem
@rem Windows/NT HCT
@rem
@rem fileio.cmd - cmd file to run disk file i/o tests
@rem

        set DEBUG=0

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%5"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%6"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%7"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF

rem        setlocal
        set _basename=
        set _fs=%2
        set _type=FIXED
        set _rats=HCTDRIVE

        if "%2"=="FAT"    set _basename=diskfat

        rem if "%2"=="HPFS"  set _basename=diskhpfs

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
 
       REM delete prev log.
       del  %HCTDIR%\logs\%_basename%.log
       :START 

       if "%_basename%"=="floppy" %HCTDIR%\bin\delnode /q %4*.*

       if %DEBUG%==1 goto DEBUG

       rats %_rats%.rat
       goto donedebug
:DEBUG
       windbg -g rats %_rats%.rat
:DONEDEBUG

       type %_rats%.log  >> %HCTDIR%\logs\%_basename%.log
       del  %_rats%.log

      REM If no more drives to test lets quit.
      rem    if "%5" ==""    goto end
       
       REM  so there are more drives to test

       REM lets get next drive
       rem   shift
       REM set test drive
       rem  set _TEST_DRIVE_NAME=%4
       REM start testing next drive
       rem  goto start







:usage
        echo.
        echo Usage: fileio [hct drive] [type] [reserved] [test drive]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [type] is NTFS, FAT or FLOPPY
        echo       [reserved] parameters are not currently used but must have values
        echo       [test drive] is the FAT partition to test
        echo.

:end
rem        endlocal
