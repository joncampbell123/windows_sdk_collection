@echo off
rem /**********************************************************************
rem * Batch name: oglpixel.bat
rem *
rem * This batch file tests all the pixel formats (well almost)
rem * and the result is logged.
rem *
rem * Warning: This batch file is equivalent to running
rem *          1. oglcfm.bat, and
rem *          2. oglpix.bat, less
rem *          3. primtest -b
rem *
rem *          Change the primtest option from -s to -a if you also want
rem *          primtest to be tested on the bitmap pixel formats.
rem *
rem * Created: 17-Aug-1994 11:39:40
rem *
rem \**********************************************************************

rem -----------------------------------------------------------------------
rem Initialize variables for controling the execution.  These are cleaned
rem up at the completion of test
:Setup

set _VLogName=oglpixel.log
set _VRunSeed=1

@echo CONFORMANCE TEST INITIATED
@echo CONFORMANCE TEST INITIATED                    > %_VLogName%

rem -----------------------------------------------------------------------
rem Run through coverage tests
rem
rem warning: -a option is used.  Ie. all, display+bitmap, formats will be
rem                    tested.
rem

@echo -- GL Coverage Test
@echo -- GL Coverage Test                           >> %_VLogName%
covgl -a                                            >> %_VLogName%

@echo -- GL Utilities Coverage Test
@echo -- GL Utilities Coverage Test                 >> %_VLogName%
covglu                                              >> %_VLogName%

rem -----------------------------------------------------------------------
rem CONFORMANCE BEGINS
rem
rem warning: -A option is used.  Ie. all, display+bitmap, formats will be
rem                    tested.
rem

:RunTest

rem @ _VRunSeed=%_VRunSeed% + 1

@echo -- Conformance ECP[-]
@echo -- Conformance ECP[-]                         >> %_VLogName%
conform -A -r %_VRunSeed% -f testlist               >> %_VLogName%

@echo -- Conformance ECP[1]
@echo -- Conformance ECP[1]                         >> %_VLogName%
conform -A -r %_VRunSeed% -f testlist -p 1          >> %_VLogName%

@echo -- Conformance ECP[2]
@echo -- Conformance ECP[2]                         >> %_VLogName%
conform -A -r %_VRunSeed% -f testlist -p 2          >> %_VLogName%

@echo -- Conformance ECP[3]
@echo -- Conformance ECP[3]                         >> %_VLogName%
conform -A -r %_VRunSeed% -f testlist -p 3          >> %_VLogName%

rem -----------------------------------------------------------------------
rem Run through primitives test
rem
rem warning: -s option is used only. Ie, only display formats are tested.
rem          -a        should be used for all, display+bitmap, formats
rem

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
