############################################################################
#
#  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
#
#  File:        mssdk.mk
#  Content:     Rules for building the components of the SDK.
#               For use with MSVC 2.0 or higher
#
############################################################################

!if "$(GSDKROOT)" == ""
GSDKROOT=..\..\..
!endif

#############################################################################
#
# Set up include & lib path
#
#############################################################################
INCLUDE=$(GSDKROOT)\inc;$(INCLUDE)
LIB=$(GSDKROOT)\lib;$(LIB)

#############################################################################
#
# new suffixes
#
#############################################################################
.SUFFIXES:
.SUFFIXES: .asm .c .cpp .exe .dll .h .inc .lib .sym .rc .res

#############################################################################
#
# C compiler definitions
#
#############################################################################
CC      =cl
CFLAGS = $(CFLAGS) -W3 -WX -c -Zp
CFLAGS  =$(CFLAGS) -Gf4ys -DIS_32 -DWIN32 
!ifndef LOGO
CFLAGS  =$(CFLAGS) -nologo
!endif

#############################################################################
#
# Linker definitions
#
#############################################################################
LINK    =link -link
LFLAGS  =$(LFLAGS) -nodefaultlib -align:0x1000
!ifndef LOGO
LFLAGS  =$(LFLAGS) -nologo
!endif

#############################################################################
# 
# resource compiler definitions
#
#############################################################################
RCFLAGS =$(RCFLAGS) -I..
RCFLAGS =$(RCFLAGS) -DWIN32 -DIS_32
RC = rc

#############################################################################
#
# assembler definitions
#
#############################################################################
ASM = ml
AFLAGS  =$(AFLAGS) -DIS_32 -DWIN32
AFLAGS  =$(AFLAGS) -W3 -WX -Zd -c -Cx -DMASM6

#############################################################################
#
# librarian definitions
#
#############################################################################
LIBEXE = lib

#############################################################################
#
# targets
#
#############################################################################

goal:   $(GOALS)

{..}.c{}.obj:
        @$(CC) @<<
$(CFLAGS) -Fo$(@B).obj ..\$(@B).c
<<

{..\..\misc}.c{}.obj:
        @$(CC) @<<
$(CFLAGS) -Fo$(@B).obj ..\..\misc\$(@B).c
<<

{..}.cpp{}.obj:
        @$(CC) @<<
$(CFLAGS) -Fo$(@B).obj ..\$(@B).cpp
<<

{..\..\misc}.cpp{}.obj:
        @$(CC) @<<
$(CFLAGS) -Fo$(@B).obj ..\..\misc\$(@B).cpp
<<
    
{..}.asm{}.obj:
        $(ASM) $(AFLAGS) -Fo$(@B).obj ..\$(@B).asm
        
{..}.rc{}.res:
        $(RC) $(RCFLAGS) -r -Fo$(@B).res ..\$(@B).rc
