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
#  Makefile for VDEF (default FSD)
#
#############################################################################
DEVICE = VDEF
OBJS   = fsmount.obj defdata.obj defaer.obj vdefios.obj deffcn.obj \
         vxdinit.obj defioctl.obj vdefdda.obj
AFLAGS = -coff -DBLD_COFF -DIS_32 -W3 -Zd -c -Cx -DMASM6 -DINITLOG 
AFLAGS = $(AFLAGS) -DSAFE=3 -DDOS_VXD=1 -DWIN32_APIS -DDEBLEVEL=1 -DDEBUG
ASMENV = ML

.asm.obj:
    set $(ASMENV)=$(AFLAGS)
    ml -Fo$*.obj $< 

$(DEVICE).vxd : $(OBJS)
    link @<<$(DEVICE).lnk /DEF:<<$(DEVICE).def
/VXD /NOD /OUT:$(DEVICE).vxd /MAP:$(DEVICE).map
$(OBJS)
<<
VXD $(DEVICE)
DESCRIPTION 'DOS386 $(DEVICE) Device  (Version 4.0)'
SEGMENTS
        _LPTEXT         CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _LTEXT          CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _LDATA          CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _TEXT           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _DATA           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        CONST           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _TLS            CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _BSS            CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _ITEXT          CLASS 'ICODE'   DISCARDABLE
        _IDATA          CLASS 'ICODE'   DISCARDABLE
        _PTEXT          CLASS 'PCODE'   NONDISCARDABLE
        _PDATA          CLASS 'PDATA'   NONDISCARDABLE SHARED
        _STEXT          CLASS 'SCODE'   RESIDENT
        _SDATA          CLASS 'SCODE'   RESIDENT
        _DBOSTART       CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
        _DBOCODE        CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
        _DBODATA        CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
        _16ICODE        CLASS '16ICODE' PRELOAD DISCARDABLE
        _RCODE          CLASS 'RCODE'
EXPORTS
        $(DEVICE)_DDB @1
<<
        mapsym -s $(DEVICE).map
