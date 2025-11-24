#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993-1995  Microsoft Corporation.  All Rights Reserved.
#
#############################################################################

TARGETNAME      =mmcaps
EXT             =exe
LOBJ            =win32
LIBS            =libc.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib winmm.lib vfw32.lib
OBJS            =$(LOBJ)\$(TARGETNAME).obj $(LOBJ)\midspids.obj $(LOBJ)\lowlevel.obj \
		 $(LOBJ)\zyztlb.obj $(LOBJ)\debug.obj
GOALS           =$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).sym

!if "$(DEBUG)" == "NO"
DEF             =
CDEBUG          =$(DEF)
LDEBUG          =-debug:none
!else
DEF             =-DDEBUG
CDEBUG          =-Zi $(DEF)
LDEBUG          =-debug:full -debugtype:cv
!endif

TARGETWARN      =-W3 -WX
TARGETOPT       =-Ox
CFLAGS          =$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ -DWIN32 $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS         =-r -I..\include -DWIN32 $(RDEBUG)
LFLAGS          =-nodefaultlib -align:0x1000 $(LDEBUG)
RC              =rc
CC              =cl -c -nologo
LINK            =link /nologo
MAPSYM          =mapsym -nologo
MFLAGS          =
OS              =i386

{}.c{$(LOBJ)}.obj:
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

.SUFFIXES:      .c

goal:   makedirs $(GOALS)
	@echo ***** Finished making $(TARGETNAME) *****

makedirs:
!if !exists($(LOBJ))
	@md $(LOBJ) >nul
!endif

clean:
!if exists($(LOBJ))
	@echo y | del $(LOBJ) >nul
	@rd $(LOBJ) >nul
!endif
	@echo ***** Finished cleaning $(TARGETNAME) *****

$(LOBJ)\lowlevel.obj:   $$(@B).c appport.h
$(LOBJ)\zyztlb.obj:     $$(@B).c $$(@B).h appport.h
$(LOBJ)\midspids.obj:   $$(@B).c appport.h
$(LOBJ)\debug.obj:      $$(@B).h $$(@B).c appport.h
$(LOBJ)\$(TARGETNAME).obj:      $$(@B).c $$(@B).h appport.h

$(LOBJ)\$(TARGETNAME).sym:      $$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map

$(LOBJ)\$(TARGETNAME).res: $$(@B).rc $$(@B).rcv $$(@B).h
	@$(RC) $(RCFLAGS) -fo$@ -i. $(@B).rc

$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).map: $(OBJS) $$(@D)\$(TARGETNAME).res $$(@B).def
	@$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:$(OS)
-subsystem:windows,4.0
-map:$(@R).map
-def:$(@B).def
$(@D)\$(TARGETNAME).res
$(LIBS)
$(OBJS)
<<
