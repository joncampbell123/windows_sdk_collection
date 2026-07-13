@echo off
rem test02b.bat
rem This batch file runs SEQUENTIAL write and read tests on a
rem 1024K byte file using LARGE RECORDS.
rem The results of the tests are written to file test02b.out.
rem The record sizes are 16KB, 32KB, 48KB, and 64KB-1(65535).
rem Files are opened Deny None. There is a single file for all machines.
echo on

call test02bw   16K 4 1
call test02bw   32K 4 1
call test02bw   48K 4 1
call test02bw 65535 4 1

call sync
rem Make correct file size file for reads.
if %machine%==1 del test.fil
if %machine%==1 %root%\pf /n:1 /w /l:1024 /m:1 /s:32K /o:test.fil >nul

call test02br   16K 4 1
call test02br   32K 4 1
call test02br   48K 4 1
call test02br 65535 4 1

call sync
del test.fil
