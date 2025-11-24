# 
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR 
# PURPOSE.
# 
# Copyright 1993-1995 Microsoft Corporation, all rights reserved.
#
# Windows Telephony Dialer sample makefile
# usage: nmake [DEBUG=0|1]
#

CC      = cl
RC      = rc
MAKE    = nmake

!if "$(DEBUG)" == "1"
CFLAGS = /nologo /G2 /W3 /Zi /Zp1 /AL /Od /DDEBUG /GA /NT $(@B)_TEXT
LFLAGS = /nologo /ONERROR:NOEXE /NOD /CO /NOE /ALIGN:4 /MAP:FULL
RESFLAGS = /DDEBUG 
!else
CFLAGS = /nologo /G3 /W3 /Zp1 /AL /O1 /Ox /DNDEBUG /GA /NT $(@B)_TEXT
LFLAGS = /nologo /ONERROR:NOEXE /NOD /NOE /ALIGN:4 /NONULLS /PACKD /PACKF /MAP:FULL
RESFLAGS = /K
!endif


LIBS = libw ldllcew toolhelp ver commdlg
APPLIBS = libw llibcew tapi


all:    clean release 

clean:  
	del *.obj 
	del *.dll 
	del *.map 
	del *.sym 
	del *.cpl
	del *.res
	del *.exe
	del *.lib
	del *.sbr
	del *.bsc
	del *.hlp
	del *.tsp
	del *.asm


release: dialer.exe

ALLOBJS = iniprefs.obj dialer.obj tapifu.obj

dialer.exe:: $(ALLOBJS) $(@B).def $(TLIB)
	echo >NUL @<<$(@B).crf
$(ALLOBJS)
$(@B).exe
$(@B).map
$(APPLIBS)
$(@B).def;
<<
	link $(LFLAGS) /NOPACKC @$(@B).crf
	$(RC) $(RESFLAGS) $(@B).rc $@
