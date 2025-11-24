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
#
#    Makefile for mouse.drv
#
#############################################################################

LFLAGS  = /L /MAP /NOPACKCODE /NOE /ALIGN:16
MFLAGS  = -m
ASM     = ml
AFLAGS  = -W2 -Zd -c -Cx -Zm -DIS_16 -DNot_VxD -Sg
ASMENV  = ML

.asm.obj:
        set $(ASMENV)=$(AFLAGS)
        $(ASM) -Fo$*.obj $< 

.rc.res:
	rc -r $*.rc

mouse.drv: mouse.obj mouse.res mouse.def
        link $(LFLAGS) @<<mouse.lnk
mouse.obj
mouse.drv 
mouse.map 
libw.lib 
mouse.def;
<<
        rc mouse.res mouse.drv
	mapsym -m -s -o mouse.sym mouse.map

mouse.obj: mouse.asm 

mouse.res: mouse.rc 
