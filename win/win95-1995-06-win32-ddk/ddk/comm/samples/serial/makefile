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
#       Makefile for serial device
#
##########################################################################

DEVICE = SERIAL

OBJS = serial.obj serutil.obj serfunc.obj serinit.obj serdbg.obj

########## Definitions ###############################

ASM     = ml
AFLAGS  = -coff -DBLD_COFF -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6
ASMENV  = ML

LFLAGS  = /VXD /NOD

############## VxD device link rule ###################

.asm.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

$(DEVICE).SYM: $(DEVICE).MAP 
	mapsym -s -o $(DEVICE).sym $(DEVICE).map

$(DEVICE).VXD $(DEVICE).MAP: $(OBJS)
	link @<<$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(LFLAGS)
/OUT:$(DEVICE).VXD
/MAP:$(DEVICE).map
$(OBJS)
<<
VXD $(DEVICE) DYNAMIC
DESCRIPTION '$(DEVICE) Device'

SEGMENTS
	_LPTEXT  CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_LTEXT   CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_LDATA   CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_TEXT    CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_DATA    CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	CONST    CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_BSS     CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_ITEXT   CLASS 'ICODE'   DISCARDABLE
	_IDATA   CLASS 'ICODE'   DISCARDABLE
	_PTEXT   CLASS 'PCODE'   NONDISCARDABLE
	_PDATA   CLASS 'PCODE'   NONDISCARDABLE
	_STEXT   CLASS 'SCODE'   RESIDENT
	_SDATA   CLASS 'SCODE'   RESIDENT
	_16ICODE CLASS '16ICODE' PRELOAD DISCARDABLE
	_RCODE   CLASS 'RCODE'
EXPORTS
	$(DEVICE)_DDB @1
<<
