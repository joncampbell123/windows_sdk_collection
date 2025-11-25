############################################################################
#
#  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
#
#  File:        d3dbld.mk
#  Content:     Master makefile for Microsoft Visual C++ 2.0
#               For controlling what gets built (debug, retail, clean) 
#
############################################################################

goal:   debug.mak 

all : debug.mak retail.mak

debug retail: $@.mak

!ifndef MAKENAME
MAKENAME = default.mk
!endif

debug.mak retail.mak:
        @if not exist $(@B)\nul md $(@B)
        @cd $(@B)
        @nmake -nologo -f ..\$(MAKENAME) DEBUG="$(@B)"
        @cd ..
        @echo *** Done making $(@B) ***

clean:  debug.cln retail.cln

debug.cln retail.cln:
        @if exist $(@B)\nul del $(@B) < ..\yes >nul
        @if exist $(@B)\nul rd $(@B) >nul
        @echo *** $(@B) is clean ***
