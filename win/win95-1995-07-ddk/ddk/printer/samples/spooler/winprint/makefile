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

COPTS = -nologo -W3 -Zdp -Gs -c -DIS_32 -D_X86_ -DWIN32 -DDEBUG -Gzfy -Oxsy
LINK = link
LINK_OPTS = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib kernel32.lib winspool.lib /ENTRY:DllEntryPoint
OBJS_EXT =
LIBS = winspool.lib libc.lib advapi32.lib mpr.lib shell32.lib
MAPFILE_OPTION = -map:winprint.map

CC   = cl $(COPTS) 

all: winprint.dll

winprint.obj: winprint.c winprint.h
        $(CC) winprint.c

util.obj: util.c
	$(CC) util.c

banner.obj: banner.c
	$(CC) banner.c

winprint.res: winprint.rc
    rc -r -DWIN32 winprint.rc

winprint.dll: winprint.obj util.obj banner.obj winprint.res
        echo @<<WINPRINT.CRF
$(LINK_OPTS) 
WINPRINT.OBJ UTIL.OBJ BANNER.OBJ 
$(OBJS_EXT)
-OUT:WINPRINT.DLL
$(MAPFILE_OPTION)
WINPRINT.RES
$(LIBS)
-def:winprint.def
<<
        link @WINPRINT.CRF

        mapsym -s winprint
