@echo off
setlocal

rem
rem     Formats a volume.  Label will be set to FORMATST
rem
rem     usage:
rem         doformat d f logfile
rem
rem         where:  d       is the drive letter (without : (colon))
rem                 f       is the file system type, FAT, HPFS, or NTFS. Must be
rem                         in uppercase.
rem                 logfile is the name of the logfile
rem



echo %1| findstr /r ^[a-zA-Z]$ > nul
if errorlevel 1 goto usage

echo %2| findstr /r "^NTFS$ ^HPFS$ ^FAT$" > nul
if errorlevel 1 goto usage

if "%3" == "" goto usage





rem
rem See if the volume if already formated
rem


set datafile=formated.dat

label %1: FORMATST
if errorlevel 1 set datafile=unformat.dat


sleep 5

format %1: /fs:%2 < %datafile%

sleep 5

goto end




:usage

echo usage:
echo^    dochkfmt  d f logfile
echo.
echo^    where:  d       is the drive letter (without : (colon))
echo^            f       is the file system type, FAT, HPFS, or NTFS. Must be
echo^                    in uppercase.
echo^            logfile is the name of the logfile
echo.




:end
endlocal
