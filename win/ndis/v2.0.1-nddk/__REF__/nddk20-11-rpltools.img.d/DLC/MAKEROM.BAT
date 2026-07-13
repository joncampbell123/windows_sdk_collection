echo off
rem  ;-------------------------------------------------------------------;
rem  ;  Batch File Name: MAKEROM.BAT                                     ;
rem  ;                                                                   ;
rem  ;  Description: This batch file builds the RIPL ROM image.          ;
rem  ;                                                                   ;
rem  ;                                                                   ;
rem  ;  Dependencies: MAKEROM.MAK - Make-ROM Make File                   ;
rem  ;                ROMDLC.LNK - Linker Control File                   ;
rem  ;                                                                   ;
rem  ;-------------------------------------------------------------------;

cd ..\titok
make /s makedrvr.mak
IF NOT ERRORLEVEL 0 GOTO exit
copy *.obj ..\dlc
cd ..\dlc
make /s makerom.mak
IF NOT ERRORLEVEL 0 GOTO exit
binfile /seg:cc00 /sum:0 romdlc.exe romdlc.out
checksum romdlc.out romdlc.bin
intelhex romdlc.bin /scc000 /1

:exit
