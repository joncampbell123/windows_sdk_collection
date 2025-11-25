@echo off
: /*+========================================================================
:  File:      REGALL.BAT
:
:  Summary:   This Batch file registers all the COM servers in the
:             OLE Tutorial Code Sample series.  It assumes that the
:             servers have already been built.  It does build the
:             REGISTER code sample if needed.
:
:  Usage:     REGALL
:               To register all COM servers.
:
:  Origin:    4-30-96: atrent - Revised for OLE Tutorial Code Sample series.
:
: --------------------------------------------------------------------------
:
:  This file is part of the Microsoft OLE Tutorial Code Samples.
:
:  Copyright (C) Microsoft Corporation, 1996.  All rights reserved.
:
:  This source code is intended only as a supplement to Microsoft
:  Development Tools and/or on-line documentation.  See these other
:  materials for detailed information regarding Microsoft code samples.
:
:  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
:  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
:  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
:  PARTICULAR PURPOSE.
: ==========================================================================+*/
:
cls
IF NOT EXIST register\register.exe GOTO ERROR
echo ================ Working on DLLSERVE...
cd dllserve
..\register\register.exe dllserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on LICSERVE...
cd ..\licserve
..\register\register.exe licserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on MARSHAL...
cd ..\marshal
..\register\register.exe marshal.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on LOCSERVE...
cd ..\locserve
..\register\register.exe /e locserve.exe
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on APTSERVE...
cd ..\aptserve
..\register\register.exe /e aptserve.exe
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on FRESERVE...
cd ..\freserve
..\register\register.exe freserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on CONSERVE...
cd ..\conserve
..\register\register.exe conserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on STOSERVE...
cd ..\stoserve
..\register\register.exe stoserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
cd ..
goto :DONE
:ERROR
echo ================ REGALL Error.
goto :END
:DONE
echo ================ REGALL Work Done.
:END
