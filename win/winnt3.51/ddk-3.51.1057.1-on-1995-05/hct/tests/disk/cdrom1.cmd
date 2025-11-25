@echo off

@rem
@rem Windows/NT HCT
@rem
@rem cdrom1.cmd - cmd file to run cdrom file i/o test
@rem

        set DEBUG=0

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage
        if "%5"=="" goto usage
        if "%6"=="" goto usage

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

        if "%2"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%3"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%4"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%5"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%6"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%7"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%8"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF
        if "%9"=="-VERBOSE" set RATS_RESULTS_LOGGED=0xFFFFFFFF

rem        setlocal

        //peteg 3/3/94: add cdrom\hct time/date stamp data to env. for new test
        set _TEST_CD_DATE=%5
        set _TEST_CD_TIME=%6

        set _TEST_DRIVE_NAME=%4\hct\tests\disk\data
        set _TEST_DRIVE_TYPE=CDROM
        set _TEST_DRIVE_FS=CDFS
        set _TEST_ALT_DRIVE_NAME=C:\

        set HCT_RUN=1

        if %DEBUG%==1 goto DEBUG

        rats hctcd.rat
        goto debugdone

:DEBUG
        windbg -g rats hctcd.rat

:DEBUGDONE

        copy hctcd.log %HCTDIR%\logs\cdrom1.log
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
rem        endlocal
