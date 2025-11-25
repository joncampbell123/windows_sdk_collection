@rem -------------------------------------------------------------------------
@rem
@rem   doprtcmp
@rem   ------
@rem
@rem   Script to comp the 2 given partitions and determine whether they
@rem        compare.
@rem
@rem   Usage
@rem   -----
@rem   doprtcmp drive_letter1 drive_letter2 options logfile
@rem
@rem   author
@rem   ------
@rem   Mitch Millar 3/26/94
@rem
@rem -------------------------------------------------------------------------

echo.                                      >> %4
echo BEGIN_TEST: Start of doprtcmp %1 test >> %4
echo.                                      >> %4

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg
if "%3" == "" goto bad_arg
if "%4" == "" goto bad_arg

@rem
@rem Run the partcomp.  If these were "failremaps" varations, then pass
@rem the bad sector list into partcomp so it knows which sectors to not
@rem expect matches on.
@rem
@rem The -i option ignore the differing of sector sizes of the two partitions
@rem if the sector size differes it chooses the smaller of the two as the
@rem length of the compare.
@rem The -q is a "quiet" mode where it does not output the upate of current
@rem sectors scanned
@rem

if "%_options1%" == "failremaps" set cmd_args=-f badsect.lst

partcomp %1: %2: %cmd_args% -q -i -d >> %4

if errorlevel 1 goto fail_partcomp

echo VERIFY PASS: Partition Comparison of %1 and %2 PASSES >> %4
goto done

:fail_partcomp
echo VERIFY FAIL: Partition Comparison of %1 and %2 FAILS >> %4
if not "%nopause%" == "" goto done 
fttest -mVERIFY FAIL: Partition Comparison of %1: and %2: failed 
goto done

:bad_arg
echo BLOCKED: bad args passed to doprtcmp.cmd
if not "%nopause%" == "" goto done
fttest -mBLOCKED: bad args passed to doprtcmp.cmd 

:done
echo.                                       >> %4
echo END_TEST: End of doprtcmp %1: %2: test >> %4
echo.                                       >> %4
