@echo off
rem 
rem Installation Script For Evaluation MDGM6 Chicago NDIS 3.1 Driver
rem
rem Copyright (c) Madge Networks Ltd 1994
rem
echo IMPORTANT: Please read the file README.TXT before continuing. Press
echo CTRL-C to abandon this installation or any other key to continue.
pause
if [%1] == [] goto USAGE
echo.
echo Copying the file NETMADG1.INF to directory %1\INF
copy NETMADG1.INF %1\INF
echo Finished.
goto END
:USAGE
echo Incorrect usage, please consult README.TXT.
:END

