@echo off
rem /**********************************************************************
rem * Batch name: oglcfm.bat
rem *
rem * This batch file tests all the display pixel formats and the result is
rem * logged.
rem *
rem * Created: 17-Aug-1994 11:39:40
rem *
rem \**********************************************************************

rem -----------------------------------------------------------------------
rem Initialize variables for controling the execution.  These are cleaned
rem up at the completion of test
:Setup

set _VLogName=oglcfm.log
set _VRunSeed=1

@echo CONFORMANCE TEST INITIATED
@echo CONFORMANCE TEST INITIATED                    > %_VLogName%

rem -----------------------------------------------------------------------
rem Run through coverage tests

@echo -- GL Coverage Test
@echo -- GL Coverage Test                           >> %_VLogName%
covgl -s                                            >> %_VLogName%

@echo -- GL Utilities Coverage Test
@echo -- GL Utilities Coverage Test                 >> %_VLogName%
covglu                                              >> %_VLogName%

rem -----------------------------------------------------------------------
rem CONFORMANCE BEGINS
:RunTest

rem @ _VRunSeed=%_VRunSeed% + 1

@echo -- Conformance ECP[-]
@echo -- Conformance ECP[-]                         >> %_VLogName%
conform -S -r %_VRunSeed% -f testlist               >> %_VLogName%

@echo -- Conformance ECP[1]
@echo -- Conformance ECP[1]                         >> %_VLogName%
conform -S -r %_VRunSeed% -f testlist -p 1          >> %_VLogName%

@echo -- Conformance ECP[2]
@echo -- Conformance ECP[2]                         >> %_VLogName%
conform -S -r %_VRunSeed% -f testlist -p 2          >> %_VLogName%

@echo -- Conformance ECP[3]
@echo -- Conformance ECP[3]                         >> %_VLogName%
conform -S -r %_VRunSeed% -f testlist -p 3          >> %_VLogName%

rem -----------------------------------------------------------------------
rem Run through primitives test

@echo -- GL Primitives Test
@echo -- GL Primitives Test                         >> %_VLogName%
primtest -s                                         >> %_VLogName%

@echo CONFORMANCE TEST COMPLETED
@echo CONFORMANCE TEST COMPLETED                    >> %_VLogName%

rem -----------------------------------------------------------------------
rem Eliminate environment variables for
rem this batch-file.
rem
:CleanUp

set _VLogName=
set _VRunSeed=

:EndTest
