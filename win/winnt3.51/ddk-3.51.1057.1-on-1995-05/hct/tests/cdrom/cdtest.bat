@REM
@REM Requires a Win32 July PDC SDK CD-ROM inserted
@REM
@REM
@REM First parameter is CD-ROM Drive letter
@REM Second parameter is harddrive pathname containing 10M
@REM freespace.
@REM

mkdir %2\cd-test >NUL
del %2\cd-test\*.* <y.dat

echo >cdtest.tmp "Starting CDROM test"
echo Status: FAIL - Test not completed >cdtest.log

for %%i in (1,2,3,4,5,6,7,8,9,0) do call cdtest2.bat %1 %2 %%i

@echo off

compchk cdtest.tmp > cdtest.log

del cdtest.tmp

rd /s %2\cd-test <y.dat

copy cdtest.log %HCTDIR%\logs
