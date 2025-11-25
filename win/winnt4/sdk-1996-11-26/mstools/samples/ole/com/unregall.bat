@echo off
: /*+========================================================================
:  File:      UNREGALL.BAT
:
:  Summary:   This Batch file Unregisters all the COM servers in the
:             OLE Tutorial Code Sample series.  It assumes that the
:             servers have already been built.  It does build the
:             REGISTER code sample if needed.
:
:  Usage:     UNREGALL
:               To Unregister all COM servers.
:
:  Origin:    10-9-95: atrent - Created for OLE Tutorial Code Sample series.
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
..\register\register.exe /u dllserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on LICSERVE...
cd ..\licserve
..\register\register.exe /u licserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on MARSHAL...
cd ..\marshal
..\register\register.exe /u marshal.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on LOCSERVE...
cd ..\locserve
..\register\register.exe /u /e locserve.exe
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on APTSERVE...
cd ..\aptserve
..\register\register.exe /u /e aptserve.exe
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on FRESERVE...
cd ..\freserve
..\register\register.exe /u freserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on CONSERVE...
cd ..\conserve
..\register\register.exe /u conserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
echo ================ Working on STOSERVE...
cd ..\stoserve
..\register\register.exe /u stoserve.dll
IF ERRORLEVEL 1 GOTO ERROR
:
cd ..
goto :DONE
:ERROR
echo ================ UNREGALL Error.
goto :END
:DONE
echo ================ UNREGALL Work Done.
:END
