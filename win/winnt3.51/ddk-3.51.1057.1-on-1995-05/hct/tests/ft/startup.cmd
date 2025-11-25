@rem -------------------------------------------------------------------------
@rem
@rem Copyright (c) 1991 Microsoft Corporation
@rem
@rem startup.cmd
@rem -----------
@rem
@rem This is the script to run on each boot to run the FT tests.
@rem
@rem One should invoke this script from the startup group in progman.
@rem
@rem Usage:
@rem ------
@rem
@rem   startup
@rem
@rem Revision History:
@rem -----------------
@rem
@rem   SteveKo 3/30/93 - original
@rem   t-kenp  9/24/94 - added the "init" opition for the 1st run startup.
@rem
@rem -------------------------------------------------------------------------

@rem 
@rem Do some 1st time only initialization
@rem

if not "%1" == "init" goto begin

del state.ini
del *.sig
copy do.0 donext

@rem
@rem remove old logs
@rem

if "%2" == "" goto begin
del %2

@rem
@rem copy run.ini to save fttest startup information for FOREVER loop.
@rem Quietly delete scendat.cmd from previous run of fttest.
@rem

copy run.ini run.bak
del scendat.cmd

@rem
@rem In a quite hacky fashion, get rid of any possible sticky net
@rem connections and turn the persistent bit off.
@rem

net use d: /d
net use e: /d
net use f: /d
net use g: /d
net use h: /d
net use i: /d
net use j: /d
net use k: /d
net use l: /d
net use m: /d
net use n: /d
net use o: /d
net use p: /d
net use q: /d
net use r: /d
net use s: /d
net use t: /d
net use u: /d
net use v: /d
net use w: /d
net use x: /d
net use y: /d
net use z: /d
net use /pers:no

@rem
@rem Increase the size of the system log
@rem

regini eventlog.reg
 

@rem
@rem Start the server side debugger, enable autologon, etc...
@rem

:begin

echo. >> console.log
echo -----------  startup: start from fresh boot -----------  >> console.log
echo. >> console.log

regini autolog.reg

@rem
@rem Startup the status monitor.
@rem

start statmon -n50 -mStatus Monitor restarted. -cFTtest

@rem
@rem Enable the CSR Debug flag and launch ntsd.
@rem 

rem
rem BUGBUG: This puts up an ntsd popup that interferes with tests.
rem BUGBUG: the fix for this almost works, but we don't have time
rem BUGBUG: to pursue this now.  Will address post PPC.
rem

rem This should now be fixed.

chkdbg

if errorlevel 2 goto enable_csr_debugging
goto skip_enable_csr_debugging

:enable_csr_debugging
sendstat "." "Shutting down to enable CSR Debugging."
sleep 5
shutdown -r
pause

:skip_enable_csr_debugging

@rem
@rem Copy current script (filename == donext) to do.cmd and run it.
@rem Collect all console output in console.log.  Ensure "shutdown.sig"
@rem does not intially exist.
@rem

:do_next

del shutdown.sig

copy donext do.cmd

echo -----------  startup: executing next phase  -----------  >> console.log
echo.      >> console.log
time < nul >> console.log
date < nul >> console.log
echo.      >> console.log

cmd /c do.cmd >> console.log 2>&1

@rem
@rem If shutdown.sig exists, then the ms-test scripts are shutting the
@rem system down, do not execute any more code, wait to be killed.
@rem

if exist shutdown.sig pause

@rem 
@rem If the tests are done then let us exit
@rem
if exist ftdone.sig goto done

@rem
@rem Do the next step
@rem

goto do_next

:done
