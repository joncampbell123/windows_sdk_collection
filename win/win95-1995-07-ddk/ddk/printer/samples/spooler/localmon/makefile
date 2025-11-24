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
MASM = ml -coff -c -Zi -DMASM6
OBJS_EXT =
LIBS = winspool.lib libc.lib advapi32.lib mpr.lib shell32.lib
MAPFILE_OPTION = -map:localmon.map

CC   = cl $(COPTS) 

all: localmon.dll

localmon.obj: localmon.c localmon.h
        $(CC) localmon.c

util.obj: util.c
	$(CC) util.c

data.obj: data.c
	$(CC) data.c

dialogs.obj: dialogs.c dialogs.h
	$(CC) dialogs.c

helper.obj: helper.asm
    $(MASM) helper.asm

localmon.res: localmon.rc
    rc -r -DWIN32 localmon.rc

localmon.dll: localmon.obj util.obj dialogs.obj data.obj helper.obj localmon.res
        echo @<<LOCALMON.CRF
$(LINK_OPTS) 
LOCALMON.OBJ DATA.OBJ UTIL.OBJ DIALOGS.OBJ HELPER.OBJ
$(OBJS_EXT)
-OUT:LOCALMON.DLL
$(MAPFILE_OPTION)
LOCALMON.RES
$(LIBS)
-def:localmon.def
<<
        link @LOCALMON.CRF

        mapsym -s localmon
