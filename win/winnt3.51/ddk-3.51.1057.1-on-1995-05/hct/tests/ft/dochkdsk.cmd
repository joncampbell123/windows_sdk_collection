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
@rem   dochkdsk driveletter fileSystem logfile
@rem
@rem -------------------------------------------------------------------------

echo.                                    >> %3
echo BEGIN_TEST: Start of chkdsk %1 test >> %3
echo.                                    >> %3

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg
if "%3" == "" goto bad_arg


chkdsk %1: >> %2
if errorlevel 3 goto fail_chkdsk3
if errorlevel 2 goto fail_chkdsk2
if errorlevel 1 goto fail_chkdsk

echo VERIFY PASS: chkdsk worked >> %3
goto done

:dont_chk
echo VERIFY PASS: The %2 filesystem is not supported by chkdsk. >> %3
goto done

:fail_chkdsk3 
echo VERIFY FAIL: chkdsk returned errorlevel = 3     >> %3
fttest -mVERIFY FAIL: chkdsk returned errorlevel = 3
goto done

:fail_chkdsk2 
echo VERIFY FAIL: chkdsk returned errorlevel = 2     >> %3
fttest -mVERIFY FAIL: chkdsk returned errorlevel = 2
goto done

:fail_chkdsk
echo VERIFY FAIL: chkdsk returned errorlevel = 1     >> %3
fttest -mVERIFY FAIL: chkdsk returned errorlevel =  1
goto done


:bad_arg
echo BLOCKED: bad args passed to dochkdsk.cmd >> %3
if not "%nopause%" == "" goto done
fttest -mBLOCKED: bad args passed to dochkdsk.cmd 
goto done

:done
echo.                                >> %3
echo END_TEST: End of chkdsk %1 test >> %3
echo.                                >> %3
