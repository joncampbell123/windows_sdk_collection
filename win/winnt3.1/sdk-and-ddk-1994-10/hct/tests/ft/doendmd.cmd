@rem -------------------------------------------------------------------------
@rem 
@rem   doendmd.cmd
@rem   -----------
@rem   Script to run endtomd.exe and verify whether it worked or not.
@rem
@rem   Usage:
@rem   ------
@rem   doendmd filename logfile
@rem
@rem -------------------------------------------------------------------------

echo.                                     >> %2
echo BEGIN_TEST: Start of endtomd %1 test >> %2
echo.                                     >> %2

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg

endtomd %1 >> %2

if errorlevel 1 goto fail_endtomd
echo VERIFY PASS: endtomd test passed >> %2
goto done

:fail_endtomd
echo VERIFY FAIL: endtomd test failed >> %2
if "%nopause%" == "" pause
goto done

:bad_arg
echo BLOCKED: bad args passed to doendmd
if "%nopause%" == "" pause
goto done

:done
echo.                                 >> %2
echo END_TEST: End of endtomd %1 test >> %2
echo.                                 >> %2
