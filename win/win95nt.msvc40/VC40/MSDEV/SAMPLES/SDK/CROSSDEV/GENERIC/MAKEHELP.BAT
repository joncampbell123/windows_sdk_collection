@echo off
if exist generic.err del generic.err > nul

if "%1"=="MAC" goto :Mac

:Intel
if not "%1" == "" goto :Error
if not "%2" == "" goto :Error

echo Building Win32 Help files....
hcrtf -x generic.hpj
goto :done

:Mac
echo Building Macintosh Help files....
hc35 gen_mac.hpj

if %2x == x goto :done
echo Copying to remote machine....
mfile copy -c MSH2 -t HELP gen_mac.hlp %2
goto :done

:Error
echo Usage MAKEHELP [MAC [macintosh-path]]

:done
if exist generic.err type generic.err
if exist generic.err del generic.err > nul
