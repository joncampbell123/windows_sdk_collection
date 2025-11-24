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
# Makefile for EXAMENUM - Example Enumerator
#
##########################################################################

DEVICE  = EXAMENUM
OBJS    = pch.obj sysctrl.obj enum.obj examenum.obj
CL      = cl
CFLAGS  = -DBLD_COFF -Zdp -Gs -c -DWANTVXDWRAPS -DIS_32 -Zl -bzalign
CFLAGS  = -W4 -WX -Ogaisb1 -Gf -Yupch.h $(CFLAGS)
ASM     = ml
AFLAGS  = -coff -DBLD_COFF -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6
ASMENV  = ML
LFLAGS  = /VXD /NOD

## Inference rules

.asm.obj:
        set $(ASMENV)=$(AFLAGS)
        $(ASM) -Fo$*.obj $< 

.c.obj:
        set CL=$(CFLAGS)
        $(CL) -Fo$*.obj $<

## Targets

default: $(DEVICE).vxd

pch.obj: pch.c
        set CL=$(CFLAGS)
        $(CL) -Ycpch.h pch.c

sysctrl.obj: sysctrl.c

enum.obj: enum.c

examenum.obj: examenum.asm

$(DEVICE).vxd: $(OBJS)
        LINK @<<$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(LFLAGS) 
/OUT:$(DEVICE).vxd
/MAP:$(DEVICE).map
$(OBJS)
VXDWRAPS.CLB
<<$(KEEPFLAG)
VXD $(DEVICE) DYNAMIC
DESCRIPTION '$(DEVICE) Device (Version 4.0)'
SEGMENTS
    _LPTEXT     CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _LTEXT      CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _LDATA      CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _TEXT       CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _DATA       CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    CONST       CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _TLS        CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _BSS        CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _ITEXT      CLASS 'ICODE'   DISCARDABLE
    _IDATA      CLASS 'ICODE'   DISCARDABLE
    _PTEXT      CLASS 'PCODE'   NONDISCARDABLE
    _PDATA      CLASS 'PDATA'   NONDISCARDABLE SHARED
    _STEXT      CLASS 'SCODE'   RESIDENT
    _SDATA      CLASS 'SCODE'   RESIDENT
    _DBOSTART   CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
    _DBOCODE    CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
    _DBODATA    CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
    _16ICODE    CLASS '16ICODE' PRELOAD DISCARDABLE
    _RCODE      CLASS 'RCODE'
EXPORTS
    $(DEVICE)_DDB @1
<<
