@echo off
rem test01b.bat
rem This batch file runs SEQUENTIAL write and read tests on a
rem 128K byte file using SMALL RECORDS.
rem The results of the tests are written to file test01b.out.
rem The record sizes are 32, 50, 100, 512, 1800, 2048
rem and 2400 bytes.
rem Files are opened Deny None. All machines share the same file.
echo on

call test01bw   32  6 2
call test01bw   50  6 2
call test01bw  100  6 2
call test01bw  512 15 5
call test01bw 1800 15 5
call test01bw 2048 15 5
call test01bw 2400 15 5

call sync
rem make file for reads
if %machine%==1 %root%\pf /n:1 /w /l:128 /m:1 /o:test.fil /s:32K >nul

call test01br   32  6 2
call test01br   50  6 2
call test01br  100  6 2
call test01br  512 15 5
call test01br 1800 15 5
call test01br 2048 15 5
call test01br 2400 15 5

call sync
if %machine%==1 del test.fil
