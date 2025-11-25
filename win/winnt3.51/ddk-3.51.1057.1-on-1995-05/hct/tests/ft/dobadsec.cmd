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
@rem   dobadsec drive_letter num_sectors fail_remaps logfile
@rem
@rem -------------------------------------------------------------------------

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg
if "%3" == "" goto bad_arg
if "%4" == "" goto bad_arg

echo.                                       >> %4
echo BEGIN_TEST: Start of dobadsec %1 %2 %3 >> %4
echo.                                       >> %4

@rem
@rem We must wait until after initialization before we can fail the remaps
@rem
@rem BUGBUG we should pass in the size instead of using num_sectors
@rem

if "%_options1%" == "failremaps" call dofstate %1 %2 0.50 %logfile%


@rem
@rem Simulate the bad sectors and fail test if genbsec returns any errors
@rem 

set cmd_arg=
if "%3"=="failremaps" set cmd_arg=-x

genbsec -d:%1 -n:%2 %cmd_arg% -c -l >> %4

if errorlevel 1 goto fail_badsect
echo VERIFY PASS: genbsec.exe returned no errors >> %4
goto done

:fail_badsect
echo VERIFY FAIL: genbsec.exe returned a failure >> %4
if not "%nopause%" == "" goto done
fttest -mVERIFY FAIL: genbsec.exe returned a failure 
goto done

:done

@rem
@rem List out the bad sectors
@rem

call dolstbad %4

echo.                                   >> %4
echo END_TEST: End of dobadsec %1 %2 %3 >> %4
echo.                                   >> %4
goto end

:end
