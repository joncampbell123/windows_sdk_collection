@rem -------------------------------------------------------------------------
@rem
@rem Copyright (c) 1991 Microsoft Corporation
@rem
@rem config.cmd
@rem ----------
@rem
@rem Command file to initialize the FT tests.
@rem
@rem This script will reset everything and allow the user to select
@rem which tests to run.
@rem
@rem Usage:
@rem ------
@rem
@rem   config <infile>
@rem
@rem   The infile is optional.  If specified it will be used as the
@rem   input to queryusr.
@rem
@rem
@rem -------------------------------------------------------------------------

@rem
@rem Get rid of old state.ini (which holds the current variation info)
@rem

del state.ini
del runover.sig

@rem
@rem Get rid of any log files
@rem

if "%2" == "/n" goto no_del
del /p *.log
:no_del

@rem
@rem Ask user for config info
@rem

if "%1" == "" goto no_in_file
queryusr.exe < %1
goto continue1

:no_in_file
queryusr
goto continue1

@rem
@rem Check the errorlevel from queryusr
@rem

:continue1
if errorlevel 1 goto end

@rem
@rem Make do.0 the next script to run
@rem

copy do.0 donext

:end
