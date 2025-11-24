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
#    Makefile for vcd
#                                                                          
#############################################################################

DEVICE        = VCD
OBJS          = vcd.obj vcdint.obj vcdmsg.obj 
DDB_NAME      = $(DEVICE)_DDB
DESCRIPTION   = $(DEVICE) Virtual Device (Version 4.0)
LFLAGS        = /NOD
AFLAGS        = -DSTANDARD_CCALL -DIS_32 -coff -DBLD_COFF -W2 -Zd -c -Cx 
AFLAGS        = $(AFLAGS) -DMASM6 -Sg -DDEBLEVEL=0 -DSHARE_IRQ
BASECOPYRIGHT = 1988

default: $(DEVICE).vxd

vcd.obj: vcd.asm
        ml $(AFLAGS) vcd.asm

vcdint.obj: vcdint.asm
        ml $(AFLAGS) vcdint.asm

vcdmsg.obj: vcdmsg.asm
        ml $(AFLAGS) vcdmsg.asm

vcd.res: 
        $(SDKROOT)\binw16\rc -r <<$(DEVICE).rc
#include <version.h>
#define Not_VxD
#include <vmm.h>

#define VER_FILETYPE			VFT_VXD
#ifdef $(DEVICE)_DEVICE_ID
#define VER_FILESUBTYPE 		$(DEVICE)_DEVICE_ID
#else
#define VER_FILESUBTYPE 		UNDEFINED_DEVICE_ID
#endif
#define VER_FILEDESCRIPTION_STR 	"$(DESCRIPTION)"
#define VER_INTERNALNAME_STR		"$(DEVICE)"
#define VER_LEGALCOPYRIGHT_YEARS	"$(BASECOPYRIGHT)-1995"
#define VER_ORIGINALFILENAME_STR	"$(DEVICE).VXD"

#include <common.ver>
<<KEEP

$(DEVICE).vxd: $(OBJS) $(DEVICE).res
        link @<<$(DEVICE).lnk /def:<<$(DEVICE).def
$(LFLAGS) /vxd
/out:$(DEVICE).vxd
/map:$(DEVICE).map
$(OBJS)
<<KEEP
VXD $(DEVICE)
DESCRIPTION '$(DESCRIPTION)'
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
<<KEEP
        adrc2vxd.exe $(DEVICE).vxd $(DEVICE).res
        mapsym -s $(DEVICE).map

clean:
        del *.vxd
        del *.map
        del *.sym
        del *.obj
        del *.lib
        del *.exp
        del *.res
