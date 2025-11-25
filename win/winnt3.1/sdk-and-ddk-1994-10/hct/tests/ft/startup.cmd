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
@rem
@rem -------------------------------------------------------------------------

@rem
@rem Start the server side debugger, enable autologon, etc...
@rem

chkdbg
smash
net use /pers:no

@rem
@rem Copy current script to do.cmd and run it.  Collect all console output
@rem in console.log.
@rem

copy donext do.cmd

echo -------------  In startup  -------------  >> console.log
echo.      >> console.log
time < nul >> console.log
echo.      >> console.log

cmd /c do.cmd | tee -a console.log
