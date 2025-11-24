#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

DRVNAME=mini
SRCDIR=.
RESDIR=..\res
TARGETS = $(DRVNAME).drv

L16EXE=$(TARGETS)		# Name of exe.
L16DEF=$(DRVNAME).def		# Our def file.
L16MAP=$(DRVNAME).map		# Our map file.
L16SYM=$(DRVNAME).sym		# Our sym file.
L16LIBS=libw.lib dibeng.lib
L16RES=$(RESDIR)\display.res	# Resource file.
LINK = link /NOE/NOD/LI/MAP/AL:16
AFLAGS = -c -Zm -Cp -nologo
ASM = ml
ASMENV  = ML

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1 = diblink.obj disable.obj enable.obj getresid.obj init.obj
OBJ2 = palette.obj sswitch.obj vga.obj 

L16OBJS= $(OBJ1) $(OBJ2) 

#-----------------------------------------------------------------------
# Goals
#-----------------------------------------------------------------------

default: $(L16EXE) $(L16RES)

.asm.obj:
        set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

$(L16RES): 
    copy $(DRVNAME).rcv $(RESDIR)\display.rcv
    cd $(RESDIR)
    $(MAKE) 
    cd ..\$(DRVNAME)

$(L16EXE): $(L16OBJS) $(L16RES) $(L16DEF)
        $(LINK) @<<
$(OBJ1) +
$(OBJ2)
$(L16EXE)
$(L16MAP)
$(L16LIBS)
$(L16DEF)
<<
        rc -31 -b $(L16RES) $(L16EXE)
	copy $(L16MAP) display.map
        -mapsym /n  display
	del display.map
	del $(L16SYM)
	rename display.sym $(L16SYM)
