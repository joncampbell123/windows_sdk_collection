@rem -------------------------------------------------------------------------
@rem
@rem   doclrbad
@rem   --------
@rem
@rem   This script will clear the bad sectors lists from each partition
@rem   with simulated bad sectors
@rem
@rem   Usage
@rem   -----
@rem   doclrbad logfile
@rem
@rem -------------------------------------------------------------------------

if "%1" == "" goto bad_arg

echo TRACE: clearing bad sectors >> %1

if not exist clrbad.cmd goto fail_message

call clrbad >> %1 
goto done

:bad_arg
echo FAIL: bad argument
if not "%nopause%" == "" goto done
fttest -mdoclrbad.cmd %1 missing argument.
goto done

:fail_message
echo FAIL: clrbad.cmd not found >> %1
if not "%nopause%" == "" goto done
fttest -mclrbad.cmd not found.

:done
