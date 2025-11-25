@echo off
setlocal

rem
rem     Format volumes.  All label will be set to FORMATST
rem
rem     usage:
rem         formatst d...
rem
rem         where:  d...    is one or more drive letter (without : (colon))
rem


if "%1" == "" goto usage

if exist format.log del format.log


:loop

    if "%1" == "" goto endloop
    echo %1| findstr /r ^[a-zA-Z]$ >nul
    if errorlevel 1 goto usage

    call doformat %1 NTFS format.log
    call dochkfmt %1 NTFS format.log
    call doformat %1 FAT  format.log
    call dochkfmt %1 FAT  format.log
    call doformat %1 HPFS format.log
    call dochkfmt %1 HPFS format.log
    call doformat %1 NTFS format.log
    call dochkfmt %1 NTFS format.log

    shift
    goto loop

:endloop

goto end




:usage

echo.
echo usage
echo^    formatst d...
echo.
echo^    where:  d...    is one or more drive letter (without : (colon))
echo.




:end
