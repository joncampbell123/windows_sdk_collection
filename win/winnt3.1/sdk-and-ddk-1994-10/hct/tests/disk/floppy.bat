@echo off

@rem
@rem Windows/NT HCT
@rem
@rem floppy.bat - manual bat file to run floppy device ioctl tests.
@rem


if "%1"=="" goto usage
if "%2"=="" goto usage

	if exist flopctl.log del flopctl.log

	echo Running getmedia...
	getmedia %1 rw rw <y.dat 1>getmedia.log 2>&1
	if "%2"=="12" goto media12
	if "%2"=="144" goto media144
	if "%2"=="288" goto media288
	goto usage
:media12
	comp getmedia.log 12\_med12.dat <n.dat >NUL 2>&1
	goto errchk1
:media144
	comp getmedia.log 144\_med144.dat <n.dat >NUL 2>&1
	goto errchk1
:media288
	comp getmedia.log 288\_med288.dat <n.dat >NUL 2>&1
	goto errchk1
:errchk1
	if errorlevel 1 goto failmedia
	echo Get Media Info    : PASS >>flopctl.log
	set _Media=PASS
:ret1
	echo Running getgeom...
	getgeom  %1 rw rw <y.dat 1>getgeom.log 2>&1
	if "%2"=="144" goto geom144
	if "%2"=="288" goto geom288
	comp getgeom.log 12\_12.dat <n.dat >NUL  2>&1
	goto errchk2
:geom144
	comp getgeom.log 144\_144.dat <n.dat >NUL  2>&1
	goto errchk2
:geom288
	comp getgeom.log 288\_288.dat <n.dat >NUL  2>&1
	goto errchk2
:errchk2
	if errorlevel 1 goto failgeom
	echo Get Disk Geometry : PASS >>flopctl.log
	set _Geom=PASS
	goto end

:failmedia
	echo Get Media Info    : FAIL >>flopctl.log
	set _Media=FAIL
	goto ret1
:failgeom
	echo Get Disk Geometry : FAIL >>flopctl.log
	set _Geom=FAIL
	goto end
goto end

:usage
        echo.
	echo Usage: floppy [test drive] [capacity (12,144,288)]
        echo.
	echo where [test drive] is the floppy drive letter to be tested.
	echo	   [capacity] is the floppy capacity in 100K ie 1.2meg = 12
        echo.
	goto out
:end
	if %_Geom%==%_Media% goto checkfail
	goto fail
:checkfail

	if %_Geom%==PASS echo Status		  : PASS >>flopctl.log
	if %_Geom%==FAIL goto fail
	goto success
:fail
	echo STATUS:FAIL >>flopctl.log

:success

	echo .
	echo Results of test:
	type flopctl.log
:out
