@echo off

@rem
@rem Tester Setup for new Ndis Tester for Windows NT 3.51
@rem

    echo.
    echo Windows NT 3.51 Ndis Tester Installation
    echo -----------------------------------------------
    echo.

    if "%1"=="" goto usage

    set _dest=c:\ndis
    if "%1"=="c:"  goto doinstall
    if "%1"=="c:\" goto doinstall
    if "%1"=="C:"  goto doinstall
    if "%1"=="C:\" goto doinstall

    set _dest=d:\ndis
    if "%1"=="d:"  goto doinstall
    if "%1"=="d:\" goto doinstall
    if "%1"=="D:"  goto doinstall
    if "%1"=="D:\" goto doinstall

    set _dest=e:\ndis
    if "%1"=="e:"  goto doinstall
    if "%1"=="e:\" goto doinstall
    if "%1"=="E:"  goto doinstall
    if "%1"=="E:\" goto doinstall

    set _dest=f:\ndis
    if "%1"=="f:"  goto doinstall
    if "%1"=="f:\" goto doinstall
    if "%1"=="F:"  goto doinstall
    if "%1"=="F:\" goto doinstall

    set _dest=g:\ndis
    if "%1"=="g:"  goto doinstall
    if "%1"=="g:\" goto doinstall
    if "%1"=="G:"  goto doinstall
    if "%1"=="G:\" goto doinstall

    set _dest=h:\ndis
    if "%1"=="h:"  goto doinstall
    if "%1"=="h:\" goto doinstall
    if "%1"=="H:"  goto doinstall
    if "%1"=="H:\" goto doinstall

    if "%1"=="/?"  goto usage
    if "%1"=="/h"  goto usage
    if "%1"=="/H"  goto usage

    set _dest=%1

:doinstall
    shift

    set _cpu=x86
    if "%PROCESSOR_ARCHITECTURE%"=="MIPS"  set _cpu=mips
    if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" set _cpu=alpha
    if "%PROCESSOR_ARCHITECTURE%"=="PPC"   set _cpu=ppc

    if "%2"=="mips" set _cpu=mips
    if "%2"=="MIPS" set _cpu=mips

    if "%2"=="alpha" set _cpu=alpha
    if "%2"=="ALPHA" set _cpu=alpha

    if "%2"=="ppc" set _cpu=ppc
    if "%2"=="PPC" set _cpu=ppc

    echo.
    echo.
    echo.
    echo                +------------------------+
    echo                  Installing NDIS Tester
    echo                +------------------------+
    echo.


    md %_dest%
    md %_dest%\scripts

    xcopy *.*           %_dest%
    xcopy %_cpu%\*.*    %_dest%
    xcopy scripts\*.*   %_dest%\scripts

    copy %_dest%\ndistest.sys "%SYSTEMROOT%"\system32\drivers
    %_dest%\regini %_dest%\ndistest.ini

    goto done

:usage
    echo.
    echo Usage: ndsetup target [cpu]
    echo.
    echo Where target is the target drive\directory (required)
    echo Note: Default Directory is drive:\ndis
    echo       [cpu] is x86, mips or alpha (optional - default is current PROCESSOR ARCHITECTURE)
    echo.
    echo If a drive is specified without a path, tests will be place in drive:\ndis.
    echo.
    echo.

    goto end


:done
    echo.
    echo.
    echo.
    echo                +----------------------------+
    echo                  NDIS Tester setup complete
    echo                +----------------------------+
    echo.

:end
