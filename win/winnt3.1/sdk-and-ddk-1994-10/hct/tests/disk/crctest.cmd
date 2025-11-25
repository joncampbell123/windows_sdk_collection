
@echo off

setlocal


rem
rem     C R C T E S T
rem
rem     This test xcopies a directory to test directory and
rem     computes crc of all the files.  If any error in crcchk
rem     the test stops.  Will stop when runtime expires.
rem






set _async_write=
set _fwflags=
set _qNbuffer=-q 8 -b 0x4000


:loop1
    echo "%1"| findstr /i /r /c:"-w" > nul
    if errorlevel 1 goto next_flag1
        set _async_write=-w
        shift
        goto loop1

    :next_flag1
    echo "%1"| findstr /i /r /c:"-f" > nul
    if errorlevel 1 goto next_flag2
        set _fwflags=-fw %2
        shift
        shift
        goto loop1

    :next_flag2
:endloop1





:next_arg2
if "%1" == "" goto usage
if "%2" == "" goto usage
if "%3" == "" goto generate
if "%4" == "" goto usage

goto test




:test

if exist %4 del %4
sleep 5

@echo off



:loop2

    @rem
    @rem starts xcopy then do crcchk
    @rem

    @rem ntsd -d -g -G xcopy /herdiq %2 %3

    xcopy /herdiq %2 %3
    crcchk %_async_write% -r %3 -f %1 -err %4 %_fwflags% %_qNbuffer%
    if errorlevel 1 goto testfail

    :testpass
    echo PASS >> %4
    echo y | rd /s %3


    @rem
    @rem check for end of runtime
    @rem

    if "%5" == "" goto loop2
    tstend %5
    if errorlevel 1 goto endloop2
    goto loop2

:endloop2

goto end



:testfail
echo FAIL >> %4
goto end






:generate
crcchk -gn -f %1 -r %2 *.*
goto end





:usage
echo crctest  [options] crcfile source [target logfile [tstdrv]]
echo.
echo ^  options:
echo ^    -w        -  do writes.
echo ^    -f flags  -  CreateFile flags, default to 0x68000000
echo.
echo ^    crcfile   -  name of crc data file.
echo ^    source    -  source directory.
echo ^    target    -  target directory, if not specified then.
echo ^                 crc file for the source will be generated.
echo ^    logfile   -  name of log file.
echo ^    tstdrv    -  driver letter of the test drive, used for checking runtime.

goto end



endlocal
:end
