@rem ------------------------------------------------------------------------
@rem
@rem   chsimbad.cmd
@rem   ------------
@rem
@rem   This script checks if simbad is loaded or not, then it checks the
@rem   current variation options, and loads or unloads simbad accordingly.
@rem
@rem   If simbad needs to be loaded or unloaded, this script will munge
@rem   the registry and reboot the machine.
@rem
@rem   This script relies on the following idw tools: regini, drivers,
@rem   and shutdown.
@rem
@rem   This script is called by do.0 once the current variation's settings
@rem   are known.  When this script forces a shutdown, it will create a file
@rem   named "simbad.sig".  Otherwise, it will ensure that this file does
@rem   not exist.
@rem
@rem   History
@rem   -------
@rem   4/94 steveko - wrote it
@rem
@rem ------------------------------------------------------------------------

@rem
@rem Default the signal to "no reboot was necessary".
@rem

del simbad.sig

@rem
@rem Be sure there is a simbad.sys in the drivers dir.
@rem

if not exist %systemroot%\system32\drivers\simbad.sys copy simbad.sys %systemroot%\system32\drivers

@rem
@rem Set the simbad_loaded variable accordingly
@rem

set simbad_loaded=1
drivers > drivers.out
findstr -i simbad drivers.out
if errorlevel 1 set simbad_loaded=

@rem
@rem Check to see if we need simbad or not
@rem

if "%options1%" == "none" goto donotloadsimbad

@rem
@rem We need simbad, if it is not already loaded, enable it and reboot.
@rem Note: do not go anywhere after initiating the shutdown.  Otherwise
@rem the caller will start executing again.
@rem

if not exist %systemroot%\system32\drivers\simbad.sys goto no_driver
if not "%simbad_loaded%" == "" goto done
regini simbade.reg
@echo TRACE: Enabling simbad driver, must reboot
echo "shutting down for simbad" > simbad.sig
shutdown -r
pause

@rem
@rem Ensure simbad is not loaded.  If it is, disable it and reboot.
@rem

:donotloadsimbad

if "%simbad_loaded%" == "" goto done
regini simbadd.reg
@echo TRACE: Disabling simbad driver, must reboot
echo "shutting down for simbad" > simbad.sig
shutdown -r
pause

:no_driver
fttest -mBLOCKED: The simbad.sys driver is not installed.

:done
