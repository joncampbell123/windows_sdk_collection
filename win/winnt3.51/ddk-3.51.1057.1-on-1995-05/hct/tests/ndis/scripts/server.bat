del *.log
del *.dbg
set testcard=
set trustedcard=

if "%1" == "" goto USAGE
set trustedcard=%1

hapi -x -l0x00000300 scripts\server
goto END

:USAGE
echo USAGE: server <servercard>

:END
