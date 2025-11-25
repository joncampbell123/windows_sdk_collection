@echo off

if "%1" == "" goto usage
cpu >cpu.log
msd /f msd.log
copy cpu.log+msd.log %1.log
del cpu.log
del msd.log
goto end

:usage
echo usage: preboot machineid
echo.
echo where MachineId is an 8 character identifier for the machine run on.
echo e.g. ASTPrem2
echo.

:end
