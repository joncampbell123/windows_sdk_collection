@echo off
@echo .
@echo FCOMTEST [LONG/SHORT/ALL] [READ/SEND] [COMx] [V] [X]
@echo .
@echo DOSCOM sample testing batch file
@echo runs all tests, long tests only, or short tests only
@echo starting at 2400 baud (short) or 4800 baud (long) with FIFO
@echo .
@echo DOSCOM RULES: one PC must READ, the other SEND
@echo [LONG/SHORT/ALL] - both PCs must run same test
@echo [COMx] - COM1, COM2, COM3 or COM4
@echo [V]    - if errors, save data
@echo [X]    - test to highest possible baud rate
@echo .
@echo BATCH FILE RULES:
@echo DOSCOM.EXE doesn't care what order parameters are in,
@echo but this batch file does!
@echo .

if %1%.==. goto NONE
if %2%.==. goto ONE
if %3%.==. goto TWO
if %4%.==. goto THREE
goto THREE

:INVALID
echo First parameter MUST be LONG or SHORT!
echo Second parameter MUST be READ or SEND!
goto END

:NONE
:ONE
:TWO
echo Not enough options - example:
echo FCOMTEST SHORT READ COM2
@echo .
goto END

:THREE
if %1%==LONG goto LONG
if %1%==long goto LONG

if %1%==SHORT goto SHORT
if %1%==short goto SHORT

if %1%==ALL goto ALL
if %1%==all goto ALL
goto INVALID

:LONG
if %2%==READ goto LONGREAD
if %2%==read goto LONGREAD

if %2%==SEND goto LONGSEND
if %2%==send goto LONGSEND
goto INVALID

:SHORT
if %2%==READ goto SHORTREAD
if %2%==read goto SHORTREAD

if %2%==SEND goto SHORTSEND
if %2%==send goto SHORTSEND
goto INVALID

:LONG
if %2%==READ goto LONGREAD
if %2%==read goto LONGREAD

if %2%==SEND goto LONGSEND
if %2%==send goto LONGSEND
goto INVALID

:ALL
if %2%==READ goto ALLREAD
if %2%==read goto ALLREAD

if %2%==SEND goto ALLSEND
if %2%==send goto ALLSEND
goto INVALID

:LONGREAD
echo Long tests take about 8 hours to run.
echo Start the SEND test first
pause
DOSCOM  fifo14  4800 l:f14p.r read %3 %4
DOSCOM  fifo8   4800 l:f8p.r  read %3 %4
DOSCOM  fifo4   4800 l:f4p.r  read %3 %4
DOSCOM  fifo1   4800 l:f1p.r  read %3 %4

DOSCOM  irq     4800 l:i2p.r  read %3 %4
DOSCOM  port    4800 l:p2p.r  read %3 %4
goto end

:LONGSEND
echo Long tests take about 8 hours to run.
echo Start the SEND test first
pause
DOSCOM  port    4800 l:pf14.s send %3 %4
DOSCOM  port    4800 l:pf8.s  send %3 %4
DOSCOM  port    4800 l:pf4.s  send %3 %4
DOSCOM  port    4800 l:pf1.s  send %3 %4

DOSCOM  port    4800 l:p2i.s  send %3 %4
DOSCOM  port    4800 l:p2p.s  send %3 %4
goto end

:SHORTREAD
echo Short tests take about 3 hours to run.
echo Start the SEND test first
pause
DOSCOM  dos     2400 l:d2d.r  read %3 %4
DOSCOM  dos     2400 l:d2b.r  read %3 %4
DOSCOM  dos     2400 l:d2a.r  read %3 %4

DOSCOM  bios    2400 l:b2d.r  read %3 %4
DOSCOM  bios    2400 l:b2b.r  read %3 %4
DOSCOM  bios    2400 l:b2a.r  read %3 %4

DOSCOM  aux     2400 l:a2d.r  read %3 %4
DOSCOM  aux     2400 l:a2b.r  read %3 %4
DOSCOM  aux     2400 l:a2a.r  read %3 %4
goto end

:SHORTSEND
echo Short tests take about 3 hours to run.
echo Start the SEND test first
pause
DOSCOM  dos     2400 l:d2d.s  send %3 %4
DOSCOM  bios    2400 l:b2d.s  send %3 %4
DOSCOM  aux     2400 l:a2d.s  send %3 %4

DOSCOM  dos     2400 l:d2b.s  send %3 %4
DOSCOM  bios    2400 l:b2b.s  send %3 %4
DOSCOM  aux     2400 l:a2b.s  send %3 %4

DOSCOM  dos     2400 l:d2a.s  send %3 %4
DOSCOM  bios    2400 l:b2a.s  send %3 %4
DOSCOM  aux     2400 l:a2a.s  send %3 %4
goto end

:ALLREAD
echo All tests take about 12 hours to run.
echo Start the SEND test first
pause
DOSCOM  fifo14  4800 l:f14p.r read %3 %4
DOSCOM  fifo8   4800 l:f8p.r  read %3 %4
DOSCOM  fifo4   4800 l:f4p.r  read %3 %4
DOSCOM  fifo1   4800 l:f1p.r  read %3 %4

DOSCOM  irq     4800 l:i2p.r  read %3 %4
DOSCOM  port    4800 l:p2p.r  read %3 %4

DOSCOM  dos     2400 l:d2d.r  read %3 %4
DOSCOM  dos     2400 l:d2b.r  read %3 %4
DOSCOM  dos     2400 l:d2a.r  read %3 %4

DOSCOM  bios    2400 l:b2d.r  read %3 %4
DOSCOM  bios    2400 l:b2b.r  read %3 %4
DOSCOM  bios    2400 l:b2a.r  read %3 %4

DOSCOM  aux     2400 l:a2d.r  read %3 %4
DOSCOM  aux     2400 l:a2b.r  read %3 %4
DOSCOM  aux     2400 l:a2a.r  read %3 %4
goto end

:ALLSEND
echo All tests take about 12 hours to run.
echo Start the SEND test first
pause
DOSCOM  port    4800 l:pf14.s send %3 %4
DOSCOM  port    4800 l:pf8.s  send %3 %4
DOSCOM  port    4800 l:pf4.s  send %3 %4
DOSCOM  port    4800 l:pf1.s  send %3 %4

DOSCOM  port    4800 l:p2i.s  send %3 %4
DOSCOM  port    4800 l:p2p.s  send %3 %4

DOSCOM  dos     2400 l:d2d.s  send %3 %4
DOSCOM  bios    2400 l:b2d.s  send %3 %4
DOSCOM  aux     2400 l:a2d.s  send %3 %4

DOSCOM  dos     2400 l:d2b.s  send %3 %4
DOSCOM  bios    2400 l:b2b.s  send %3 %4
DOSCOM  aux     2400 l:a2b.s  send %3 %4

DOSCOM  dos     2400 l:d2a.s  send %3 %4
DOSCOM  bios    2400 l:b2a.s  send %3 %4
DOSCOM  aux     2400 l:a2a.s  send %3 %4
goto end

:end
echo Testing is complete
@echo .

