@echo off
@rem -------------------------------------------------------------------------
@rem
@rem   remove.cmd
@rem   ----------
@rem
@rem     This is an interactive IOCTL test for removable media
@rem     disk drives and drivers that support these devices.
@rem
@rem     This test should be run as part of the certification
@rem     of all removable media disk devices and drivers.
@rem
@rem   Usage:
@rem   ------
@rem
@rem     remove drive
@rem
@rem     eg: remove a:
@rem
@rem   Results:
@rem   --------
@rem
@rem     The summary results PASS/FAIL will be written to remove.log
@rem
@rem   Notes:
@rem   ------
@rem
@rem -------------------------------------------------------------------------

    @if "%1" == ""   goto usage
    @if "%1" == "/?" goto usage
    @if "%1" == "/-" goto usage
    @if "%1" == "/h" goto usage
    @if "%1" == "-h" goto usage

    if exist *.log del *.log >NUL 2>&1

    copy test1.dat %1\test.dat	>>err.log 2>&1
    echo .
    echo Replace media in test drive with another formatted disk.
    echo Wait until disk fully spins up.
    pause
    if exist %1\test.dat del test.dat >NUL 2>&1
    copy test2.dat %1\test.dat	>>err.log 2>&1

    echo .
    echo Replace media in test drive with original disk.
    echo Wait until disk fully spins up.
    pause

    comp test1.dat %1\test.dat <n.dat >>err.log 2>&1
    if errorlevel 1 goto error_one

    echo .
    echo Replace media in test drive with second disk.
    echo Wait until disk fully spins up.
    pause

    comp test2.dat %1\test.dat <n.dat >err.log 2>&1
    if errorlevel 1 goto error_one
    echo PASS: Changed media test on drive %1 passed >>remove.log
    echo STATUS:    PASS >>remove.log
    goto done
:error_one
    echo FAIL: changed media test on drive %1 failed >>remove.log
    echo STATUS:    FAIL >>remove.log
    goto done
:usage
    @echo usage: remove drive_letter:
    goto success

:done
