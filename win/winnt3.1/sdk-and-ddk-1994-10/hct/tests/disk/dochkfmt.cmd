@echo off
setlocal

rem
rem     Checks the format of a volume.  Assumes the volume
rem     lable is FORMATST.
rem
rem     usage:
rem         dochkfmt d f logfile
rem
rem         where:  d       is the drive letter (without : (colon))
rem                 f       is the file system type, FAT, HPFS, or NTFS. Must be
rem                         in uppercase.
rem                 logfile is the name of the logfile
rem


rem
rem Check command line arguments
rem

echo %1| findstr /r ^[a-zA-Z]$ >nul
if errorlevel 1 goto usage

echo %2| findstr /r "^NTFS$ ^HPFS$ ^FAT$" >nul
if errorlevel 1 goto usage

if "%3" == "" goto usage






rem
rem Now create some file on the volume make sure it is
rem realy usable
rem

copy copychk.dat %1:\
echo n | comp copychk.dat %1:\copychk.dat
if errorlevel 1 goto error 4




rem
rem Check volume label
rem

:chklabel
echo FORMATST | label %1: | findstr FORMATST >nul
if errorlevel 1 goto error3




rem
rem Use chkdsk to check for error and file system format
rem

:chkdsk

chkdsk %1: > %1_chkdsk.dat


findstr %2 %1_chkdsk.dat >nul
if errorlevel 1 goto error1


findstr -i error %1_chkdsk.dat >nul
if not errorlevel 1 goto error2



goto noerror






:usage

echo usage:
echo^    dochkfmt  d f logfile
echo.
echo^    where:  d       is the drive letter (without : (colon))
echo^            f       is the file system type, FAT, HPFS, or NTFS. Must be
echo^                    in uppercase.
echo^            logfile is the name of the logfile
echo.




:error1
echo FAIL - format %1: /fs:%2, did not format to %2 >> %3
goto end


:error2
echo FAIL - format %1: /fs:%2, chkdsk found error >> %3
goto end


:error3
echo FAIL - format %1: /fs:%2, found incorrect label >> %3
goto end


:error4
echo FAIL - format %1: /fs:%2, file copied to it was corrupted >> %3
goto end



:noerror
echo PASS - format %1: /fs:%2 >> %3
goto end




:end
endlocal
