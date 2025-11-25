@echo off
@echo FNTMETS Suite
:* ----------------------------------------------------------------------------
:* RUN FNTMETS SUITE
:*   This batch-file runs through the FNTMETS tests.  This script requires
:*   that the %TEMPPATH%\FNTMETS port be added through PRINTMAN.
:*
:*
:*
:*
:*
:* ----------------------------------------------------------------------------


:SUITE
scripter fntmets
call printfm1.bat
call printfm2.bat
parser fntmets FMtest*.log
