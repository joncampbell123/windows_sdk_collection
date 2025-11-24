#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
#                                                                           *
#****************************************************************************

DEVICE      = ASYNCW32
OBJS        = bin\asyncw32.obj bin\sysctrl.obj bin\wrappers.obj
CFLAGS      = -DWIN32 -DCON -Di386 -D_X86_ -D_NTWIN -W3 -Gs -D_DEBUG -Zi
CVXDFLAGS   = -Zdp -Gs -c -DIS_32 -Zl -DDEBLEVEL=1 -DDEBUG
LFLAGS      = -machine:i386 -debug:notmapped,full -debugtype:cv -subsystem:console kernel32.lib 
LVXDFLAGS   = -MACHINE:i386 -DEBUG:NONE -PDB:NONE -NOD
AFLAGS      = -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -DDEBLEVEL=1 -DDEBUG
DESCRIPTION = DOS386 Async W32 Devioctl Sample Device

all: direxist bin\con_aw32.exe bin\asyncw32.vxd

direxist:
        if not exist bin\nul md bin

bin\asyncw32.obj: asyncw32.c
        cl $(CVXDFLAGS) -Fo$@ %s

bin\sysctrl.obj: sysctrl.asm
        ml $(AFLAGS) -Fo$@ %s

bin\wrappers.obj: wrappers.asm
        ml -coff -DBLD_COFF -W2 -c -Cx -DMASM6 -DSEGNUM=1 -Fo$@ %s

bin\$(DEVICE).res: 
        $(SDKROOT)\binw16\rc -r -fo $@ <<$(DEVICE).rc
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
#define VER_LEGALCOPYRIGHT_YEARS	"1995"
#define VER_ORIGINALFILENAME_STR	"$(DEVICE).VXD"

#include <common.ver>
<<

bin\$(DEVICE).vxd: $(OBJS) bin\$(DEVICE).res
        link @<<$(DEVICE).lnk /def:<<$(DEVICE).def
$(LVXDFLAGS) /vxd
/out:bin\$(DEVICE).vxd
/map:bin\$(DEVICE).map
$(OBJS)
vxdwraps.clb
<<
VXD $(DEVICE) DYNAMIC
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
    $(DEVICE)_DDB @1
<<
        adrc2vxd.exe $@ bin\$(DEVICE).res
        mapsym -s -o bin\$(DEVICE).sym bin\$(DEVICE).map
        del bin\$(DEVICE).exp
        del bin\$(DEVICE).lib

bin\con_aw32.obj: con_aw32.c
        cl $(CFLAGS) -c -Fo$@ %s

bin\con_aw32.exe: bin\con_aw32.obj
        link @<<con_aw32.crf
$(LFLAGS)
-PDB:NONE -OUT:bin\con_aw32.exe -MAP:bin\con_aw32.map bin\con_aw32.obj
<<

clean:
        del bin\*.obj
        del bin\*.vxd
        del bin\*.exe
        del bin\*.map
        del bin\*.sym
        del bin\*.res
        rd  bin
        del *.pdb

