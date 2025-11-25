@rem -------------------------------------------------------------------------
@rem
@rem   dochorph
@rem   --------
@rem
@rem   Script to check that an orphaning did in fact ocurr.
@rem
@rem   It determines this by calling ftstate -o which looks in the registry.
@rem
@rem -------------------------------------------------------------------------

echo.                                        >> %1
echo BEGIN_TEST: check orphaning in registry >> %1
echo.                                        >> %1

@rem
@rem If the "orphover.sig" file does not exist, then the orphaning thread]
@rem either did not complete in time, or has not completed at all.  In
@rem either event, there is something really wrong.
@rem

if exist orphover.sig goto orphaning_completed
echo VERIFY FAIL: orphover.sig does not exist >> %1
if not "%nopause%" == "" goto test1_done
fttest -mERROR: The orphaning thread did not complete in time.
goto done

:orphaning_completed

@rem
@rem Find out the test drive's drive letter
@rem

call getdat.cmd dummy dummy ft_sizeMB driveLetter1 driveLetter2 _options1

@rem
@rem Check that one of the members is marked ORPHANED
@rem

ftstate %driveLetter1%: -o                   >> %1
if errorlevel 1 goto not_orphaned
echo VERIFY PASS: registry says a member was orphaned >> %1
goto test1_done

:not_orphaned
echo VERIFY FAIL: registry does not say a member was orphaned >> %1
if not "%nopause%" == "" goto test1_done
fttest -mVERIFY FAIL: registry does not say a member was orphaned 
goto test1_done

:test1_done
echo.                                          >> %1
echo END_TEST: End of check for orphaning test >> %1
echo.                                          >> %1

echo.                                         >> %1
echo TRACE: Below is the orphan.log from the  >> %1
echo        async orphan that ocurred earlier >> %1
echo.                                         >> %1

type orphan.log >> %1
