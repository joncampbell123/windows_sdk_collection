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

COPTS = -nologo -W3 -Zl -G3 -c -DIS_32 -bzalign -D_X86_ -DWIN32 -Oxsy
LINK = link
LINK_OPTS = /NOLOGO -merge:.bss=.data -merge:.rdata=.text /SUBSYSTEM:windows /ENTRY:LibMain -align:0x1000
OBJS_EXT =
LIBS = libc.lib kernel32.lib gdi32.lib user32.lib rasapi32.lib tapi32.lib
MAPFILE_OPTION = -map:smmhook.map

CC   = cl $(COPTS) 

all: smmhook.dll

smmhook.obj: smmhook.c smmhook.h
        $(CC) smmhook.c

smmthrd.obj: smmthrd.c smmhook.h
        $(CC) smmthrd.c

util.obj: util.c smmhook.h
	$(CC) util.c

terminal.obj: terminal.c rcids.h
        $(CC) terminal.c

smmhook.res: smmhook.rc smmhook.rcv
    rc -r -DWIN32 smmhook.rc

smmhook.dll: smmhook.obj smmthrd.obj util.obj terminal.obj smmhook.res
        echo @<<SMMHOOK.CRF
$(LINK_OPTS)
-dll
-base:0x00400000
SMMHOOK.OBJ SMMTHRD.OBJ UTIL.OBJ TERMINAL.OBJ
$(OBJS_EXT)
-OUT:SMMHOOK.DLL
$(MAPFILE_OPTION)
SMMHOOK.RES
$(LIBS)
-def:smmhook.def
<<
        link @SMMHOOK.CRF

        mapsym -s smmhook
