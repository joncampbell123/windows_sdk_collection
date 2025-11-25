@rem -------------------------------------------------------------------------
@rem
@rem   doparcmp
@rem   ------
@rem
@rem   Script to do a redundent data comparison within an FT volume
@rem
@rem   Usage
@rem   -----
@rem   doparcmp drive_letter _options1 logfile
@rem
@rem   author
@rem   ------
@rem   Mitch Millar 3/25/94
@rem
@rem -------------------------------------------------------------------------

@rem
@rem We should be healthy but just in case let's make sure
@rem
call dofstate %1 0 0 %3

echo.                                      >> %3
echo BEGIN_TEST: Start of doparcmp %1 test >> %3
echo.                                      >> %3

call dotimer "BEGIN_TEST: Start of doparcmp %1 test" "." time.log

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg
if "%3" == "" goto bad_arg

@rem
@rem Run parcomp and check the errorlevel for failure
@rem
@rem The -q option is for "quiet" mode
@rem

set cmd_args=
if "%2" == "failremaps" set cmd_args=-f badsect.lst

partcomp %1: %cmd_args% -q -d           >> %3
if errorlevel 1 goto fail_parcomp

echo VERIFY PASS:  Redundent data compare of %1: %2 PASSES >> %3
goto done

:fail_parcomp
echo VERIFY FAIL: Redundent data compare of %1: %2 FAILED >> %3
if not "%nopause%" == "" goto done 
fttest -mVERIFY FAIL: Redundent data compare %1: %2 FAILED
goto done

:bad_arg
echo BLOCKED: bad args passed to doparcmp.cmd
if not "%nopause%" == "" goto done
fttest -mBLOCKED: bad args passed to doparcmp.cmd 

:done
echo.                                   >> %3
echo END_TEST: End of doparcmp %1: test >> %3
echo.                                   >> %3

call dotimer "END_TEST: End of doparcmp %1: test" "." time.log
