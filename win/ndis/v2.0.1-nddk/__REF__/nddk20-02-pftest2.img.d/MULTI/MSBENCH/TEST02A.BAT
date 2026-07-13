@echo off
rem test02a.bat
rem This batch file runs SEQUENTIAL write and read tests on a
rem 1024K byte file using LARGE RECORDS.
rem The results of the tests are written to file test02a.out.
rem The record sizes are 16KB, 32KB, 48KB, and 64KB-1(65535).
rem Files are opened Exclusively. Each machine has its own file.
echo on

call test02aw   16K 4 1
call test02aw   32K 4 1
call test02aw   48K 4 1
call test02aw 65535 4 1

call sync
rem make correct file size file for reads
%root%\pf /n:1 /w /l:1024 /m:1 /s:32K /o:test.%machine% >nul

call test02ar   16K 4 1
call test02ar   32K 4 1
call test02ar   48K 4 1
call test02ar 65535 4 1

call sync
del test.%machine%
