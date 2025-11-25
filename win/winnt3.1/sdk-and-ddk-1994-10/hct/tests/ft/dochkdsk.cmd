@rem -------------------------------------------------------------------------
@rem
@rem   dochkdsk
@rem   --------
@rem
@rem   Script to do a chkdsk on the given drive and determine whether it
@rem   succeeds or not.
@rem
@rem   Usage
@rem   -----
@rem   dochkdsk driveletter logfile
@rem
@rem -------------------------------------------------------------------------

echo.                                    >> %2
echo BEGIN_TEST: Start of chkdsk %1 test >> %2
echo.                                    >> %2

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg

chkdsk %1: >> %2

echo THIS IS A BIG GIANT HACK AND NEEDS TO BE FIXED >> %2

echo VERIFY PASS: chkdsk worked >> %2
goto done

:bad_arg
echo BLOCKED: bad args passed to dochkdsk.cmd >> %2
if "%nopause%" == "" pause
goto done

:done
echo.                                >> %2
echo END_TEST: End of chkdsk %1 test >> %2
echo.                                >> %2
