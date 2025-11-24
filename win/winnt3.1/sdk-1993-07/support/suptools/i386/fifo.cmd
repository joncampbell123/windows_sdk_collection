@echo off

if "X%1X"=="X/?X" goto usage

if %1#==# goto usage
if %1#==/help# goto usage

if %1#==ON# goto fifo_on
if %1#==on# goto fifo_on
if %1#==On# goto fifo_on

if %1#==OFF# goto fifo_off
if %1#==Off# goto fifo_off
if %1#==off# goto fifo_off

goto usage

:fifo_on

regini fifo_on.ini
if errorlevel 1 goto error
echo FIFO is ON
goto end

:fifo_off
regini fifo_off.ini
if errorlevel 1 goto error
echo FIFO is OFF
goto end

:error
echo Make sure REGINI.EXE is in your path and you have administrator privledges
goto end

:usage
echo Used to enable or disable the use of FIFO UART's with RAS
echo "Usage - FIFO [ ON | OFF ]"

:end
