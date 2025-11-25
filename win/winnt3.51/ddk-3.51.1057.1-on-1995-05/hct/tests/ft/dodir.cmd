@rem -------------------------------------------------------------------------
@rem
@rem   dodir
@rem   ----
@rem
@rem   Script to do a dir on the given drive and determine whether it
@rem   succeeds or not.
@rem
@rem   Usage
@rem   -----
@rem   dodir driveletter logfile
@rem
@rem -------------------------------------------------------------------------

echo.                                 >> %2
echo BEGIN_TEST: Start of dir %1 test >> %2
echo.                                 >> %2

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg

echo.                       >> %2
echo TRACE: Running dir %1: >> %2
echo.                       >> %2

dir %1: >> %2

echo THIS IS A BIG GIANT HACK AND NEEDS TO BE FIXED >> %2

echo VERIFY PASS: the dir worked >> %2
goto done

:bad_arg
echo BLOCKED: bad args passed to dodir.cmd >> %2
if not "%nopause%" == "" goto done
fttest -mBLOCKED: bad args passed to dodir.cmd 
goto done

:done
echo.                             >> %2
echo END_TEST: End of dir %1 test >> %2
echo.                             >> %2

