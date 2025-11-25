@rem ************************************************************************
@rem Copyright (c) 1991 Microsoft Corporation
@rem
@rem Module Name:
@rem   dotimer.cmd
@rem
@rem Abstract:
@rem   logs the start and stop times indivdual tests.
@rem   also sends status to statmon (Status Monitor)
@rem 
@rem Usage:
@rem           
@rem   dotimer.cmd  ( VARIATION_START | VARIATION_END | 
@rem                  (( BEGIN | END__ )_<testname>) )
@rem                machine 
@rem                logfile
@rem
@rem Author:
@rem   Kenieth Peery (kpeery) 3-19-94
@rem
@rem Revision History:
@rem *************************************************************************

@rem
@rem Note. a single period "." can be used to represent the local machine.
@rem (of parameter %2)
@rem

if "VARIATION_START"=="%1" goto start_end_variation
if "VARIATION_END"=="%1" goto start_end_variation

rem echo.                                                       >> %3
echotime %1  /t                                             >> %3
rem echo.                                                       >> %3

sendstat %2 %1
goto done

:start_end_variation

@rem
@rem Get the varation info
@rem 

call getdat.cmd ft_test fileSystem sizeMB dummy dummy options

echo.                                                         >> %3
echotime %1 %sizeMB% Meg %fileSystem% %ft_test% %options% /t  >> %3
echo.                                                         >> %3
sendstat %2 "%1 %sizeMB% Meg %fileSystem% %ft_test% %options%" 

:done
