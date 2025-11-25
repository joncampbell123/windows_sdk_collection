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

@rem
@rem Log the beggining and ending times of this script
@rem
call dotimer "BEGIN_ENDTOMD %1" "." time.log

echo.                                     >> %2
echo BEGIN_TEST: Start of endtomd %1 test >> %2
echo.                                     >> %2

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg

rem endtomd %1 -p117      >> %2

endtomd %1 -d:fsetmap.dat   >> %2

if errorlevel 1 goto fail_endtomd
echo VERIFY PASS: endtomd test passed >> %2
goto done

:fail_endtomd
echo VERIFY FAIL: endtomd test failed >> %2
if not "%nopause%" == "" goto done
fttest -mVERIFY FAIL: endtomd test failed 
goto done

:bad_arg
echo BLOCKED: bad args passed to doendmd
if not "%nopause%" == "" goto done 
fttest -mBLOCKED: bad args passed to doendmd
goto done

:done
echo.                                 >> %2
echo END_TEST: End of endtomd %1 test >> %2
echo.                                 >> %2

@rem
@rem Logging the ending times as well as notify statmon
@rem
call dotimer "END___ENDTOMD %1" "." time.log
