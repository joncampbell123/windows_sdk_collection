@rem -------------------------------------------------------------------------
@rem
@rem   doformat
@rem   --------
@rem
@rem   This script will format the specified drive and verify whether it
@rem   was successful or not.
@rem
@rem   Usage
@rem   -----
@rem   doformat label fileSystem driveLetter logfile
@rem
@rem -------------------------------------------------------------------------

echo.                                                                   >> %4
echo BEGIN_TEST: Start of format %3: /fs:%2 /v:%1 %_formatoptions% test >> %4
echo.                                                                   >> %4

if "%1" == "" goto bad_arg
if "%2" == "" goto bad_arg
if "%3" == "" goto bad_arg
if "%4" == "" goto bad_arg

@rem
@rem First try to copy a file to the target drive.  If this works then
@rem the target file system already has a file system, and this is not
@rem good.
@rem

echo hello > foo
copy foo %3:\
if errorlevel 1 goto okay
echo BLOCKED: Cannot format drive %3: because it is already formatted >> %4
goto done
:okay

@rem
@rem Generate a data file to pipe into cmd to do it
@rem

echo format %3: /fs:%2 /v:%1 %_formatoptions% ^>^> %4 2^>^&1 >  formatft.scp 
echo y                                                       >> formatft.scp
echo s                                                       >> formatft.scp
echo o                                                       >> formatft.scp
echo j                                                       >> formatft.scp
echo exit                                                    >> formatft.scp

@rem
@rem Pipe in the data
@rem

type formatft.scp | cmd.exe

@rem
@rem For verification, do a dir and see if the volume label is found
@rem

dir %3:\ > tmp.tmp
findstr /i %1 tmp.tmp

if errorlevel 1 goto not_found
echo VERIFY PASS: Successfully formatted drive %3: >> %4
goto verification_2

:not_found
echo VERIFY FAIL: Format of drive %3: to filesystem %2 failed >> %4
if "%nopause%" == "" pause
goto done

@rem
@rem For next verification, copy a file to it
@rem

:verification_2
echo hello > foo
copy foo %3:\

if errorlevel 1 goto copy_failed
echo VERIFY PASS: Successfully copied a file to newly formatted volume >> %4
goto done

copy_failed:
echo VERIFY FAIL: Could not copy a file to the newly formatted drive >> %4
if "%nopause" == "" pause
goto done

:bad_arg
echo BLOCKED: bad args passed to doformat.cmd
if "%nopause" == "" pause
goto done

:done
echo.                                              >> %4
echo END_TEST: End of format %3: /fs:%2 /v:%1 test >> %4
echo.                                              >> %4

