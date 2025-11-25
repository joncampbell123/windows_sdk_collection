############################################################################
#
#  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
#
#  File:        watbld.mk
#  Content:     Master makefile for WATCOM C 10.0
#               For controlling what gets built (debug, retail, clean) 
#
############################################################################

.before
        @set include=..\..\..\inc;..\..\watinc;$(%include)
        
goal: debug.mak 

all : debug.mak retail.mak

debug : debug.mak .SYMBOLIC
        @%null
        
retail : retail.mak .SYMBOLIC
        @%null

!ifndef MAKENAME
MAKENAME = default.mk
!endif

debug.mak: .SYMBOLIC
        @if not exist debug\nul md debug
        @cd debug
        @wmake -f ..\$(MAKENAME) DEBUG=1 MAKENAME=$(MAKENAME)
        @cd ..
        @echo *** Done making debug ***
        
retail.mak: .SYMBOLIC
        @if not exist retail\nul md retail
        @cd retail
        @wmake -f ..\$(MAKENAME) MAKENAME=$(MAKENAME)
        @cd ..
        @echo *** Done making retail ***

clean:  debug.cln retail.cln .SYMBOLIC
        @%NULL

debug.cln:  .SYMBOLIC
        @if exist debug\nul del debug < ..\yes >nul
        @if exist debug\nul rd debug >nul
        @echo *** debug is clean ***

retail.cln: .SYMBOLIC
        @if exist retail\nul del retail < ..\yes >nul
        @if exist retail\nul rd retail >nul
        @echo *** retail is clean ***
