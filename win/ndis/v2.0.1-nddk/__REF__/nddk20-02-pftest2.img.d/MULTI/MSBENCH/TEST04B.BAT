@echo off
rem  file: test04b.bat
rem This batch file runs TRANSACTION tests to a 128K byte file using
rem SMALL RECORDS.
rem The results of the tests are written to file test04.out.
rem The record sizes are 32, 50, 100, 512, 1800, 2048
rem and 2400 bytes.
rem There are four sets of tests.
rem File is opened Deny None. There is a single file shared by all machines.
echo on

call sync
if %machine%==1 %root%\pf /n:1 /w /l:128 /m:1 /s:32K /o:test.fil >nul
call sync

call test04br r   32  6 2
call test04br r   50  6 2
call test04br r  100  6 2
call test04br r  512 15 5
call test04br r 1800 15 5
call test04br r 2048 15 5
call test04br r 2400 15 5

call test04br 1   32  6 2
call test04br 1   50  6 2
call test04br 1  100  6 2
call test04br 1  512 15 5
call test04br 1 1800 15 5
call test04br 1 2048 15 5
call test04br 1 2400 15 5

call test04br 3   32  6 2
call test04br 3   50  6 2
call test04br 3  100  6 2
call test04br 3  512 15 5
call test04br 3 1800 15 5
call test04br 3 2048 15 5
call test04br 3 2400 15 5
		      
call test04br 2   32  6 2
call test04br 2   50  6 2
call test04br 2  100  6 2
call test04br 2  512 15 5
call test04br 2 1800 15 5
call test04br 2 2048 15 5
call test04br 2 2400 15 5

call sync
if %machine%==1 del test.fil
