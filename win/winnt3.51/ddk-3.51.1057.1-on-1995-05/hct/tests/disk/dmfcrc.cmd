@echo off

if "%4" == "" goto usage
if "%5" == "" goto usage

if "%5" == "2" GOTO 2FLOP
if "%5" == "1" GOTO 1FLOP
goto usage

:1FLOP

@echo Please Insert NT Workstation Install Disk4
pause
@echo Testing DMF Floppy1
crccheck -f crcdmf1.dat -r %4\ *.* > hctdmf.log 2>&1

goto endcopy

:2FLOP

@echo Please Insert NT WKSTA Install Disk4
pause
@echo Testing DMF on NT WKSTA Install Disk4
crccheck -f crcdmf1.dat -r %4\ *.* > hctdmf1.log 2>&1

@echo Please Remove NT WKSTA Install Disk4 and Insert NT WKSTA Install Disk5
pause
@echo Testing DMF on NT WKSTA Install Disk5
crccheck -f crcdmf2.dat -r %4\ *.* > hctdmf2.log 2>&1

@copy /a hctdmf1.log + hctdmf2.log hctdmf.log

@echo Please REMOVE Floppy from %4
pause
goto endcopy

:usage

@echo dmfcrc floppydrive: #floppies(1 or 2)
goto end

:endcopy

@copy hctdmf.log %hctdir%\logs

:end
