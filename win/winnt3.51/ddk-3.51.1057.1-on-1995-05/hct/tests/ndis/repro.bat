set testcard=
set trustedcard=

if "%1" == "" goto USAGE
if "%1" == "%2" goto USAGE
set testcard=%1

if "%2" == "" goto RUNIT
set trustedcard=%2

:RUNIT
hapi -x -l0x000003ff scripts/repro
goto END

:USAGE
echo.
echo USAGE: repro <testcard> [trustedcard]

:END
