@echo off
rem test01a.bat
rem This batch file runs SEQUENTIAL write and read tests on a
rem 128K byte file using SMALL RECORDS.
rem The results of the tests are written to file test01a.out.
rem The record sizes are 32, 50, 100, 512, 1800, 2048
rem and 2400 bytes.
rem Files are opened exclusively. Each machine has its own file.
echo on

call test01aw   32  6 2
call test01aw   50  6 2
call test01aw  100  6 2
call test01aw  512 15 5
call test01aw 1800 15 5
call test01aw 2048 15 5
call test01aw 2400 15 5

call sync
rem make file for reads
%root%\pf /n:1 /w /l:128 /m:1 /o:test.%machine% /s:32K >nul

call test01ar   32  6 2
call test01ar   50  6 2
call test01ar  100  6 2
call test01ar  512 15 5
call test01ar 1800 15 5
call test01ar 2048 15 5
call test01ar 2400 15 5

call sync
del test.%machine%
