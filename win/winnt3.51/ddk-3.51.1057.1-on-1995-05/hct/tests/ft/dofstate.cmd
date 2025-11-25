@rem -------------------------------------------------------------------------
@rem 
@rem   dofstate.cmd
@rem   ------------
@rem   Script to run ftstate and verify whether it worked or not.
@rem
@rem   Usage:
@rem   ------
@rem   dofstate <driveLetter> <timer> <factor> <logfile>
@rem
@rem -------------------------------------------------------------------------

echo.                                  >> %4
echo BEGIN_TEST: Start of ftstate test >> %4
echo.                                  >> %4

if "%1" == "" goto bad_arg

ftstate.exe %1: -t:%2 -f:%3                         >> %4

if errorlevel 1 goto fail_ftstate
echo VERIFY PASS: ft component became healthy       >> %4
goto done
:fail_ftstate
echo VERIFY FAIL: ft component never became healthy >> %4
if not "%nopause%" == "" goto done
fttest -mVERIFY FAIL: ft component never became healthy 
goto done

:bad_arg
echo BLOCKED: bad args passed to dofstate.cmd
if not "%nopause%" == "" goto done
fttest -mBLOCKED: bad args passed to dofstate.cmd
goto done

:done
echo.                              >> %4
echo END_TEST: End of ftstate test >> %4
echo.                              >> %4
