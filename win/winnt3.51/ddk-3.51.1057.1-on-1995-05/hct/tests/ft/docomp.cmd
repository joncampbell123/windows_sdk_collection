@rem -------------------------------------------------------------------------
@rem
@rem   docomp
@rem   ------
@rem
@rem   Script to comp the 2 given files and determine whether it worked or not.
@rem
@rem   Usage
@rem   -----
@rem   docomp file1 file2 logfile
@rem
@rem -------------------------------------------------------------------------

echo.                                     >> %3
echo BEGIN_TEST: Start of comp %1 %2 test >> %3
echo.                                     >> %3

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg
if "%3" == "" goto bad_arg

@rem
@rem Generate a data file to pipe into cmd to do it
@rem

echo comp %1 %2 ^>^> tmp.tmp 2^>^&1  >  comp.scp
echo %intlNo%                        >> comp.scp
echo exit                            >> comp.scp

@rem
@rem Pipe in the data
@rem

type comp.scp | cmd.exe

@rem
@rem cat the tmp.tmp output from comp into the log file
@rem

type tmp.tmp >> %3

@rem
@rem For verification, look for string "Files compare OK", if this is
@rem not found verification fails
@rem

findstr /c:"%intlCompFilesOk%" tmp.tmp
if errorlevel 1 goto fail_comp
echo VERIFY PASS: Comparison of %1 and %2 passes >> %3
goto done
:fail_comp
echo VERIFY FAIL: Comparison of %1 and %2 failed >> %3
if "not %nopause%" == "" goto done 
fttest -mVERIFY FAIL: Comparison of %1 and %2 failed 
goto done

:bad_arg
echo BLOCKED: bad args passed to docomp.cmd >> %3
if not "%nopause%" == "" goto done
fttest -mBLOCKED: bad args passed to docomp.cmd 
goto done

:done
echo.                                 >> %3
echo END_TEST: End of comp %1 %2 test >> %3
echo.                                 >> %3
