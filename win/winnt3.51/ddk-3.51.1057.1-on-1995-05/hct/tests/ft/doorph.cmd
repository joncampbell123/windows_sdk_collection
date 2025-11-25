@rem -------------------------------------------------------------------------
@rem
@rem   doorph
@rem   ------
@rem
@rem   This script will orphan a whole member of the currently tested swp
@rem
@rem   Usage
@rem   -----
@rem   doorph drive_letter size_partition time_factor logfile
@rem
@rem -------------------------------------------------------------------------

if "%4" == "" goto bad_arg

echo TRACE: spawning orphan >> %4

cmd /c doorph2.cmd %1 %2 %3 >> consorph.log 2>&1

goto done

:bad_arg

@echo.
@echo usage:  doorph drive_letter size_partition time_factor logfile
@echo.

:done
