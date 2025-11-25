@echo off
@rem -------------------------------------------------------------------------
@rem
@rem   disk.cmd
@rem   --------
@rem
@rem     This is an interactive IOCTL test case for hard disk
@rem     drives/drivers.
@rem
@rem     This test should be run as part of the certification
@rem     of all disk drives/drivers.
@rem
@rem     If the disk is also a removeable media device, remove.cmd
@rem     should be ran as well.
@rem
@rem   Usage:
@rem   ------
@rem
@rem	 disk drive
@rem
@rem	 eg: disk c:
@rem
@rem   Results:
@rem   --------
@rem
@rem     The summary results PASS/FAIL will be written to disk.log
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

    if exist *.log del *.log

    getgeom %1 rw rw
    if errorlevel 1 goto geom_failed

    echo PASS: Disk geometry test on drive %1 passed >> disk.log
    set _Geom=PASS
:resume
    getperf %1 rw rw
    if errorlevel 1 goto perf_failed
    echo PASS: Disk performance test of drive %1 passed >>disk.log
    set _Perf=PASS
    goto done

:geom_failed
    set _Geom=FAIL
    echo FAIL: Disk Geometry test on drive %1 failed >> disk.log
    goto resume
:perf_failed
    set _Perf=FAIL
    echo FAIL: Disk performance test of drive %1 failed >>disk.log
    goto done

:usage
    @echo usage: disk drive_letter:
    goto success

:done
	if %_Geom%==%_Perf% goto checkfail
	goto fail
:checkfail

	if %_Geom%==PASS echo Status		  : PASS >>disk.log
	if %_Geom%==FAIL goto fail
	goto success
:fail
	echo Status		  : FAIL >>disk.log

:success
