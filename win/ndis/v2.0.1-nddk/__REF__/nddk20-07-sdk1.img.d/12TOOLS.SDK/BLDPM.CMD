rem Batch file to build all DDK PM binaries.
rem echo off
if "%1"=="" goto usage

call %1\tools\env %1

cd %1\pmsrc\src\epsonptr
call bld

cd %1\pmsrc\src\plotter
call bld

cd %root%\pmsrc\src\pcl
call mk retail us

cd %1\pmsrc\src\tests\engdraw
dmake engdraw.mak

cd %1\pmsrc\src\pscript
call bld

cd %1\pmsrc\src\pmdisp\egafam\cga
call mk all

cd %1\pmsrc\src\pmdisp\egafam\egavga
call mk all

cd %1\pmsrc\src\pmdisp\ppxy\8514
call mk all

goto end
:usage
echo
echo Usage:     BLDPM dir
echo            where dir is the parent directory of pmsrc\src
echo Example:   BLDPM C:\DDK12

:end
@echo on
