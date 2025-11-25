@rem -------------------------------------------------------------------------
@rem 
@rem   dofstate.cmd
@rem   ------------
@rem   Script to run ftstate and verify whether it worked or not.
@rem
@rem   Usage:
@rem   ------
@rem   dofstate <logfile>
@rem
@rem -------------------------------------------------------------------------

echo.                                  >> %1
echo BEGIN_TEST: Start of ftstate test >> %1
echo.                                  >> %1

if "%1" == "" goto bad_arg

ftstate.exe >> %1
if errorlevel 1 goto fail_ftstate
echo VERIFY PASS: ft component became healthy >> %1
goto done
:fail_ftstate
echo VERIFY FAIL: ft component never became healthy >> %1
if "%nopause%" == "" pause
goto done

:bad_arg
echo BLOCKED: bad args passed to dofstate.cmd
if "%nopause" == "" pause
goto done

:done
echo.                              >> %1
echo END_TEST: End of ftstate test >> %1
echo.                              >> %1
