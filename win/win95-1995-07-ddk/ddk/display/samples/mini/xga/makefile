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

DRVNAME = XGA
RESDIR  = ..\res

L16EXE  = $(DRVNAME).drv        # Name of exe.
L16DEF  = $(DRVNAME).def        # Our def file.
L16MAP  = $(DRVNAME).map        # Our map file.
L16SYM  = $(DRVNAME).sym        # Our sym file.
L16LIBS = libw.lib dibeng.lib
L16RES  = $(RESDIR)\display.res # Resource file.
LINK    = link /NOE/NOD/LI/MAP/AL:16

AFLAGS  = -c -Zm -Cp -nologo
ASM     = ml
ASMENV  = ML

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1 = diblink.obj disable.obj  control.obj sswitch.obj 
OBJ2 = init.obj    inkready.obj ssb.obj     cursor.obj
OBJ3 = bitblt.obj  xgablt.obj   vga.obj     enable.obj
OBJ4 = palette.obj getresid.obj strblt.obj  access.obj

L16OBJS= $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) 

.asm.obj:
        set $(ASMENV)=$(AFLAGS)
        $(ASM) -Fo$*.obj $< 

#-----------------------------------------------------------------------
# Goals
#-----------------------------------------------------------------------

default: $(L16EXE) $(L16RES)

$(L16RES): 
        copy $(DRVNAME).rcv $(RESDIR)\display.rcv
        cd $(RESDIR)
        $(MAKE) 
        cd ..\$(DRVNAME)

$(L16EXE): $(L16OBJS) $(L16RES) $(L16DEF)
        $(LINK) @<<
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4)
$(L16EXE)
$(L16MAP)
$(L16LIBS)
$(L16DEF)
<<
        rc -b $(L16RES) $(L16EXE)
        copy $(L16MAP) display.map
        -mapsym /n  display
        del display.map
        del $(L16SYM)
        rename display.sym $(L16SYM)

