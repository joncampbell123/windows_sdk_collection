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
@rem   doformat label fileSystem driveLetter logfile [force_to_cmdline]
@rem
@rem   Notes
@rem   -----
@rem
@rem   The force_to_cmdline argument is optional.  If a value is given,
@rem   format.com will be run, as opposed to formatting from windisk.
@rem
@rem   Note that there are still other conditions that might force the
@rem   format to the cmdline.  It is a bit of a hack that this arg exists.
@rem   If we define an env var ft_type (e.g. ft_type == "mirrors") this
@rem   would no longer be necessary.  The problem is: you cannot quick
@rem   format swp or mirrors from windisk.
@rem
@rem   Revision History:
@rem   -----------------
@rem   steveko - 1993 - wrote it
@rem   Mitch Millar  (a-mitchm) 4-8-94 - changes to do format from windisk
@rem
@rem -------------------------------------------------------------------------

@rem
@rem
@rem
call dotimer "BEGIN_FORMAT %3 %2" "." time.log


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
if not "%nopause%" == "" goto done
fttest -mBLOCKED: Cannot format drive %3: because it is already formatted
goto done

:okay

@rem
@rem Check to see if we can run format.mst
@rem   run it when:
@rem      1. the file system is not HPFS.
@rem      2. the NT version is 1.0a+
@rem

if "%fileSystem%"=="HPFS"  goto commandlineformat
if "%fileSystem%"=="hpfs"  goto commandlineformat
if "%FT_VERSION%"=="1.0"   goto commandlineformat

@rem
@rem Don't use FORMAT.MST if calling script sends 
@rem arg %5 set.  He is MIRROR or SWP requesting 
@rem quick format.  We cannot Quick Format SWP or 
@rem MIRRORs from Windisk.
@rem
if not "%5"==""               goto commandlineformat

@echo Starting FORMAT.MST run
@echo Start FORMAT.MST run                      >> %4

mtrun format.pcd /h

@echo End FORMAT.MST run                        >> %4

@echo Sleep for 5 seconds before verification   >> %4
sleep 5

goto verification

@rem
@rem Generate a data file to pipe into cmd to do it
@rem
:commandlineformat

set fileSys=%2
if "%2" == "hpfs" set fileSys=sfph
if "%2" == "HPFS" set fileSys=sfph

label %3: %1 

if "%FT_QFORMAT%"=="" goto regformat

echo %intlYes% | format %3: /fs:%fileSys% /v:%1 /q %_formatoptions% >> %4  2>&1 
goto verification

:regformat
echo %intlYes% | format %3: /fs:%fileSys% /v:%1 %_formatoptions% >> %4  2>&1 


@rem
@rem For verification, do a dir and see if the volume label is found
@rem

:verification

dir %3:\ > tmp.tmp
findstr /i %1 tmp.tmp

if errorlevel 1 goto not_found
echo VERIFY PASS: Successfully formatted drive %3: >> %4
goto verification_2

:not_found
echo VERIFY FAIL: Format of drive %3: to filesystem %2 failed >> %4
if not "%nopause%" == "" goto done
fttest -mVERIFY FAIL: Format of drive %3: to filesystem %2 failed 
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
if not "%nopause%" == "" goto done
fttest -mVERIFY FAIL: Could not copy a file to the newly formatted drive 
goto done

:bad_arg
echo BLOCKED: bad args passed to doformat.cmd
if not "%nopause%" == "" goto done
fttest -mBLOCKED: bad args passed to doformat.cmd
goto done

:done
echo.                                              >> %4
echo END_TEST: End of format %3: /fs:%2 /v:%1 test >> %4
echo.                                              >> %4

@rem
@rem Log the ending time
@rem
call dotimer "END___FORMAT: %3 %2" "." time.log 


