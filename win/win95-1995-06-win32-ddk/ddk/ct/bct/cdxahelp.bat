@ECHO OFF
ECHO .
ECHO Testing CDXA file %1...
ECHO Testing CDXA file %1... >>%3
IF ERRORLEVEL 1 GOTO Section2
REM In this area, we still only have disable responses
%2\XATOOL %1 /v:5 /lf:%2\CDXATEMP.HLP
IF ERRORLEVEL 4 GOTO StillDisable
IF ERRORLEVEL 3 GOTO PassOne
GOTO Fail
:Section2
%2\XATOOL %1 /v:5 /lf:%2\CDXATEMP.HLP
IF ERRORLEVEL 3 GOTO PassOne
:Fail
ECHO CDXA file %1 FAILED!
ECHO CDXA file %1 FAILED! >%3
COPY %3 + %2\CDXATEMP.HLP %3 >NUL
DEL %2\CDXATEMP.HLP >NUL
RETURN2.COM
Goto Done
:StillDisable
ECHO CDXA file %1 returned Disabled!
ECHO CDXA file %1 returned Disabled! > %3
COPY %3 + %2\CDXATEMP.HLP %3 >NUL
DEL %2\CDXATEMP.HLP >NUL
RETURN0.COM
GOTO Done
:PassOne
ECHO CDXA file %1 PASSED!
ECHO CDXA file %1 PASSED! >%3
COPY %3 + %2\CDXATEMP.HLP %3 >NUL
DEL %2\CDXATEMP.HLP >NUL
RETURN1.COM
:Done

