@rem -------------------------------------------------------------------------
@rem
@rem   dobadsec
@rem   --------
@rem
@rem   This script will simulate bad sectors.  The bad sectors are chose
@rem   at random within the current test regions.
@rem
@rem   Usage
@rem   -----
@rem   dobadsec drive_letter num_sectors logfile
@rem
@rem -------------------------------------------------------------------------

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg
if "%3" == "" goto bad_arg

echo.                                    >> %3
echo BEGIN_TEST: Start of dobadsec %1 %2 >> %3
echo.                                    >> %3

@rem
@rem Simulate the bad sectors and fail test if genbsec returns any errors
@rem 

genbsec -d:%1 -n:%2 >> %3
if errorlevel 1 goto fail_badsect
echo VERIFY PASS: genbsec.exe returned no errors >> %3
type badsect.lst >> %3
goto done

:fail_badsect
echo VERIFY FAIL: genbsec.exe returned a failure >> %3
if "%nopause%" == "" pause
goto done

:done
echo.                                 >> %3
echo END_TEST: End of dobadsect %1 %2 >> %3
echo.                                 >> %3
goto end

:end
