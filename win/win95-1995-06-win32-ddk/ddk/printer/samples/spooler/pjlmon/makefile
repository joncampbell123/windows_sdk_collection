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
MAPFILE_OPTION = -map:pjlmon.map

CC   = cl $(COPTS) 

all: pjlmon.dll

pjlmon.obj: pjlmon.c parsepjl.h dialogs.h
        $(CC) pjlmon.c

utillm.obj: utillm.c
	$(CC) utillm.c

pjlmon.res: pjlmon.rc
    rc -r -DWIN32 pjlmon.rc

pjlmon.dll: pjlmon.obj utillm.obj parsepjl.obj pjlmon.res
        echo @<<PJLMON.CRF
$(LINK_OPTS) 
PJLMON.OBJ UTILLM.OBJ PARSEPJL.OBJ 
$(OBJS_EXT)
-OUT:PJLMON.DLL
$(MAPFILE_OPTION)
PJLMON.RES
$(LIBS)
-def:pjlmon.def
<<
        link @PJLMON.CRF

        mapsym -s pjlmon
