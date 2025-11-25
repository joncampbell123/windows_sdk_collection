@echo off


rem
rem     A S Y N C C R C
rem
rem     This test start several console windows to run crctest.cmd.  Each
rem     console window gets its own directory of files for xcopying and
rem     async crc check.
rem
rem     This test is same as docrc.cmd except it assumes all *.exe and
rem     *.cmd files are in a flat directory.
rem
rem     Must have following files:
rem         asynccrc.cmd
rem         crctest.cmd
rem         freedisk.exe
rem         sizecopy.exe
rem         tstbegin.exe
rem         tstend.exe
rem         crcchk.exe
rem






setlocal



set _percentfree=100
set _mindiskspace=20
set _write=
set _stress=stress
set _crctest=%_stress%\crctest
set _crc_nt=%_stress%\crc_nt

set _sys_size1=60
set _sys_size2=120
set _sys_size3=180
set _sys_size4=240
set _sys_size5=300

set _drv_size1=6
set _drv_size2=12
set _drv_size3=18
set _drv_size4=24
set _drv_size5=30

set _flags=-f 0xe0000000



rem let it run for 6 hr.

set _runtime=360




:loop1

    echo "%1"| findstr /i /r /c:"-w" > nul
    if errorlevel 1 goto next_flag1
        set _flags=%_flags% -w
        shift
        goto loop1

    :next_flag1
    echo "%1"| findstr /i /r /c:"-f" > nul
    if errorlevel 1 goto next_flag2
        set _flags=%_flags% -f %2
        shift
        shift
        goto loop1

    :next_flag2
    echo "%1" | findstr /i /r /c:"-p" > nul
    if errorlevel 1 goto next_flag3
        set _percentfree=%2
        shift
        shift
        goto loop1

    :next_flag3
    echo "%1" | findstr /i /r /c:"-t" > nul
    if errorlevel 1 goto next_flag4
        set _runtime=%2
        shift
        shift
        goto loop1

    :next_flag4
:endloop1




:get_srcdrv
if "%1" == "" goto usage
    set _srcdrv=%1
    set _tstdrv=%_srcdrv%



:get_tstdrv
echo %2| findstr /i "i386 mips" > nul
if errorlevel 1 goto loop2
    set _tstdrv=%1
    shift





:loop2

    rem all files are in a flat dir whether they are mips or i386, we can't tell.
    goto start

    echo %2| findstr /i /r /c:^mips$ > nul
    if errorlevel 1 goto next_platform1
        set _machine=i386
        goto endloop2

    :next_platform1
    echo %2| findstr /i /r /c:^i386$ > nul
    if errorlevel 1 goto next_platform2
        set _machine=mips
        goto endloop2

    :next_platform2
    goto usage

:endloop2









rem
rem
rem start the tests
rem
rem


:start



if exist %_tstdrv%\%_crctest% echo y | rd /s %_tstdrv%\%_crctest% > nul
if exist %_srcdrv%\%_crc_nt%  echo y | rd /s %_srcdrv%\%_crc_nt%  > nul


if not exist %_tstdrv%\%_stress%  md %_tstdrv%\%_stress%
if not exist %_tstdrv%\%_crctest% md %_tstdrv%\%_crctest%




rem
rem copy test .cmd and .exe
rem


goto done_copy

rem not need to copy, they are all here.

copy *.cmd %_tstdrv%\%_crctest%
copy obj\%_machine%\*.exe %_tstdrv%\%_crctest%
copy ..\crcchk\obj\%_machine%\*.exe %_tstdrv%\%_crctest%

%_tstdrv%
cd \%_crctest%


:done_copy



if exist *.log del *.log


rem
rem log the start of the test
rem

tstbegin %_tstdrv% %_runtime%







rem
rem copy test files, from system32
rem

:copy_system32

set _required_space=%_sys_size1%
if "%_srcdrv%" == "%_tstdrv%" set _required_space=%_sys_size2%

freedisk %_srcdrv%\ %_required_space% %_percentfree%
if errorlevel 1 goto copy_drivers

sizecopy %windir%\system32 %_srcdrv%\%_crc_nt%\system32 *.* %_sys_size1%

call crctest %_tstdrv%\%_crctest%\sys.crc %_srcdrv%\%_crc_nt%
call crctest %_tstdrv%\%_crctest%\drv.crc %_srcdrv%\%_crc_nt%\system32\drivers

goto crc_system32




rem
rem copy test files, from drivers
rem

:copy_drivers

freedisk %_srcdrv%\ %_drv_size2% %_percentfree%
if errorlevel 1 goto more_disk_space

sizecopy %windir%\system32\drivers %_srcdrv%\%_crc_nt%\system32\drivers *.* %_drv_size1%

call crctest %_tstdrv%\%_crctest%\drv.crc %_srcdrv%\%_crc_nt%\system32\drivers

goto crc_drivers






:crc_system32

freedisk %_tstdrv%\ %_sys_size1% %_percentfree%
if errorlevel 1 goto crc_drivers

start "crc_sys1" crctest %_flags% %_tstdrv%\%_crctest%\sys.crc %_srcdrv%\%_crc_nt% %_tstdrv%\%_crctest%\crc_sys1 %_tstdrv%\%_crctest%\crc_sys1.log %_tstdrv%

freedisk %_tstdrv%\ %_sys_size2% %_percentfree%
if errorlevel 1 goto crc_drivers

start "crc_sys2" crctest %_flags% %_tstdrv%\%_crctest%\sys.crc %_srcdrv%\%_crc_nt% %_tstdrv%\%_crctest%\crc_sys2 %_tstdrv%\%_crctest%\crc_sys2.log %_tstdrv%

freedisk %_tstdrv%\ %_sys_size3% %_percentfree%
if errorlevel 1 goto crc_drivers

start "crc_sys3" crctest %_flags% %_tstdrv%\%_crctest%\sys.crc %_srcdrv%\%_crc_nt% %_tstdrv%\%_crctest%\crc_sys3 %_tstdrv%\%_crctest%\crc_sys3.log %_tstdrv%

freedisk %_tstdrv%\ %_sys_size4% %_percentfree%
if errorlevel 1 goto crc_drivers

start "crc_sys4" crctest %_flags% %_tstdrv%\%_crctest%\sys.crc %_srcdrv%\%_crc_nt% %_tstdrv%\%_crctest%\crc_sys4 %_tstdrv%\%_crctest%\crc_sys4.log %_tstdrv%

freedisk %_tstdrv%\ %_sys_size5% %_percentfree%
if errorlevel 1 goto crc_drivers

start "crc_sys5" crctest %_flags% %_tstdrv%\%_crctest%\sys.crc %_srcdrv%\%_crc_nt% %_tstdrv%\%_crctest%\crc_sys5 %_tstdrv%\%_crctest%\crc_sys5.log %_tstdrv%




:crc_drivers

freedisk %_tstdrv%\ %_drv_size1% %_percentfree%
if errorlevel 1 goto end

start "crc_drv1" crctest %_flags% %_tstdrv%\%_crctest%\drv.crc %_srcdrv%\%_crc_nt%\system32\drivers %_tstdrv%\%_crctest%\crc_drv1 %_tstdrv%\%_crctest%\crc_drv1.log %_tstdrv%

freedisk %_tstdrv%\ %_drv_size2% %_percentfree%
if errorlevel 1 goto end

start "crc_drv2" crctest %_flags% %_tstdrv%\%_crctest%\drv.crc %_srcdrv%\%_crc_nt%\system32\drivers %_tstdrv%\%_crctest%\crc_drv2 %_tstdrv%\%_crctest%\crc_drv2.log %_tstdrv%

freedisk %_tstdrv%\ %_drv_size3% %_percentfree%
if errorlevel 1 goto end

start "crc_drv3" crctest %_flags% %_tstdrv%\%_crctest%\drv.crc %_srcdrv%\%_crc_nt%\system32\drivers %_tstdrv%\%_crctest%\crc_drv3 %_tstdrv%\%_crctest%\crc_drv3.log %_tstdrv%

freedisk %_tstdrv%\ %_drv_size4% %_percentfree%
if errorlevel 1 goto end

start "crc_drv4" crctest %_flags% %_tstdrv%\%_crctest%\drv.crc %_srcdrv%\%_crc_nt%\system32\drivers %_tstdrv%\%_crctest%\crc_drv4 %_tstdrv%\%_crctest%\crc_drv4.log %_tstdrv%

freedisk %_tstdrv%\ %_drv_size5% %_percentfree%
if errorlevel 1 goto end

start "crc_drv5" crctest %_flags% %_tstdrv%\%_crctest%\drv.crc %_srcdrv%\%_crc_nt%\system32\drivers %_tstdrv%\%_crctest%\crc_drv5 %_tstdrv%\%_crctest%\crc_drv5.log %_tstdrv%


goto end








:usage
echo usage:
echo ^    docrc  [options] [src_drv] tst_drv
echo.
echo ^    options:
echo ^      -w          - enables async read/write test.
echo ^      -f flags    - CreateFile flags, (default 0x68000000)
echo ^      -p n        - use up to n%% of free diskspace. (default 100%%)
echo ^      -t min      - run for min minutes. (default 360)
echo.
echo ^      src_drv     - is the drive the sources of xcopy will be copied to.
echo ^                    Will copy to tst_drv if not specified.
echo ^      tst_drv     - is the drive for doing xcopy/crc tests.
echo.
echo ^      ** you must have more than %_mindiskspace% MB free on tst_drv to run **
goto end


:more_disk_space
echo ** you must have more than %_mindiskspace% MB free on test_drive to run **



:end
endlocal
