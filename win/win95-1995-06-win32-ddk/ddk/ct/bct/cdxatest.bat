@ECHO OFF
IF NOT "%1"=="" GOTO Commence
ECHO CDXATEST - Tests a CDXA drive and CD
ECHO .
ECHO Format: CDXATEST Drive: CT-Directory Logfile
ECHO Example: CDXATEST E: C:\CT\BCT C:\CT\BCT\CDXA.LOG
Goto Done
:Commence
ECHO CDXA Test commencing - note: This test will read your ENTIRE CDROM!
ECHO CDXA Test commencing >%3
IF NOT EXIST %1\MPEGAV\NUL GOTO DISABLENODISK
ECHO CDXA Files found, testing:
ECHO CDXA Files found, testing: >>%3
%2\RETURN0.COM
FOR %%9 IN (%1\MPEGAV\*.DAT) DO IF NOT ERRORLEVEL 2 CALL %2\CDXAHELP.BAT %%9 %2 %3
IF ERRORLEVEL 2 GOTO Fail
IF ERRORLEVEL 1 GOTO Pass
:Disable
ECHO Probable non-CDXA compatible drive!
ECHO Probable non-CDXA compatible drive! >>%3
RETURN4.COM
Goto Done
:Pass
ECHO CDXA compatible drive, all tests passed!
ECHO CDXA compatible drive, all tests passed! >>%3
RETURN3.COM
Goto Done
:Fail
ECHO Last CDXA file test failed!
ECHO Last CDXA file test failed! >>%3
RETURN2.COM
Goto Done
:DISABLENODISK
ECHO A CDXA disk was not found in the CDROM drive!
ECHO A CDXA disk was not found in the CDROM drive! >>%3
RETURN4.COM
:DONE
ERRVALUE.BAT
