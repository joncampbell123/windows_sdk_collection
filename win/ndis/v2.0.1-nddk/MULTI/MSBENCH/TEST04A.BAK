@echo off
rem  file: test04a.bat
rem This batch file runs TRANSACTION tests to a 128K byte file using
rem SMALL RECORDS.
rem The results of the tests are written to file test04a.out.
rem The record sizes are 32, 50, 100, 512, 1800, 2048
rem and 2400 bytes.
rem There are four sets of tests.
rem Files are opened Exclusively. Each machine has its own file.
echo on

call sync
del test.%machine%
%root%\pf /n:1 /w /l:128 /m:1 /s:32K /o:test.%machine% >nul
call sync

call test04ar r   32  6 2
call test04ar r   50  6 2
call test04ar r  100  6 2
call test04ar r  512 15 5
call test04ar r 1800 15 5
call test04ar r 2048 15 5
call test04ar r 2400 15 5

call test04ar 1   32  6 2
call test04ar 1   50  6 2
call test04ar 1  100  6 2
call test04ar 1  512 15 5
call test04ar 1 1800 15 5
call test04ar 1 2048 15 5
call test04ar 1 2400 15 5

call test04ar 3   32  6 2
call test04ar 3   50  6 2
call test04ar 3  100  6 2
call test04ar 3  512 15 5
call test04ar 3 1800 15 5
call test04ar 3 2048 15 5
call test04ar 3 2400 15 5

call test04ar 2   32  6 2
call test04ar 2   50  6 2
call test04ar 2  100  6 2
call test04ar 2  512 15 5
call test04ar 2 1800 15 5
call test04ar 2 2048 15 5
call test04ar 2 2400 15 5

call sync
del test.%machine%


