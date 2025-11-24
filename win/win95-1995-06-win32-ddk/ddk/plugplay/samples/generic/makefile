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

INCLUDE =.;$(INCLUDE)

##########################################################################
#                                                                                Microsoft Confidential
#       Makefile for Sample
#
##########################################################################

DEVICE          = SAMPLE
LIBS            = vxdwraps.clb
OBJS            = devload.obj driver.obj enum.obj sample.obj sysctrl.obj

######### Definitions for the debug level #########

!ifdef DEBUG
DDEBUG =-DDEBLEVEL=1 -DDEBUG
CFLAGS =$(CFLAGS) -Oy1
!else
DDEBUG =-DDEBLEVEL=0
!endif

########## Definitions for the Assembler ##########

ASM     = ml
AFLAGS  = -coff -DBLD_COFF $(DDEBUG) -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6
ASMENV  = ML

########## Definitions for C compiler #############

CL      = cl 
CFLAGS  = -DBLD_COFF -Zdp -Gs -c -DWANTVXDWRAPS -DIS_32 -Zl $(CFLAGS) 
CFLAGS  = $(CFLAGS) $(DDEBUG) 

########## Definitions for VxD linker #############

LFLAGS  = /VXD /NOD

########## Base inference rules ###################


#       MASM 6 only inference rules

.asm.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 


#       32 bit C compiler inference rules

.c.obj:
	set CL=$(CFLAGS)
	$(CL) -Fo$*.obj $<

############### VxD device section ####################

KEEPFLAG = NOKEEP
VERSION = 4.0
DDB_NAME = $(DEVICE)_DDB
DEVICETYPE = DYNAMIC

$(DEVICE).sym: $(DEVICE).map
	mapsym -s -o $(DEVICE).sym $(DEVICE).map

$(DEVICE).map: $(DEVICE).VXD 

$(DEVICE).VXD: $(OBJS) {$(LIB)}$(LIBS) $(STUB)
	LINK @<<$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(LFLAGS) 
/OUT:$(DEVICE).vxd
/MAP:$(DEVICE).map
$(OBJS)
$(LIBS)
<<$(KEEPFLAG)
VXD $(DEVICE) $(DEVICETYPE)
DESCRIPTION '$(DEVICE) Device (Version $(VERSION))'
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
	$(DDB_NAME) @1
<<$(KEEPFLAG)
