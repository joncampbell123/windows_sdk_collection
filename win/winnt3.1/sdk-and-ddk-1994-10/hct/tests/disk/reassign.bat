@echo off
@rem -------------------------------------------------------------------------
@rem
@rem   reassign.cmd
@rem   --------
@rem
@rem	 This is REASSIGN_BLOCKS IOCTL test case for hard disk
@rem     drives/drivers.
@rem
@rem     This test should be run as part of the certification
@rem     of all disk drives/drivers.
@rem
@rem   Usage:
@rem   ------
@rem
@rem	 reassign drive_#
@rem
@rem	 eg: reassign 0
@rem
@rem   Results:
@rem   --------
@rem
@rem	 The summary results PASS/FAIL will be written to reassign.log
@rem
@rem
@rem   Notes:
@rem   ------
@rem	 This test is HIGHLY DESTRUCTIVE. The test drive will have to be
@rem	 low-level formatted after this test is run.
@rem
@rem -------------------------------------------------------------------------

    @if "%1" == ""   goto usage
    @if "%1" == "/?" goto usage
    @if "%1" == "/-" goto usage
    @if "%1" == "/h" goto usage
    @if "%1" == "-h" goto usage

    if exist *.log del *.log

    reassign \\.\PhysicalDrive%1 rw rw
    if errorlevel 1 goto test_failed

    echo PASS: Reassign blocks test on drive %1 passed >> reassign.log
    echo Status        PASS >>reassign.log
    goto done
:test_failed
    echo FAIL: Reassign blocks test on drive %1 failed >> reassign.log
    echo Status        FAIL >>reassign.log
    goto done
:usage
    echo usage: reassign drive#
    echo	reassign 0 -- runs the test on Physical drive 0

:done
