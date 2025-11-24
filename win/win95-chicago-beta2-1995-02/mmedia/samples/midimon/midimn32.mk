#===========================================================================;
#
#   Midimon - 32 bit makefile
#
#
#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
#
#############################################################################

TARGETBASE	=midimon
EXT		=exe
TARGETNAME	=midimn32
LOBJ		=win32
LIBS		=libc.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib winmm.lib vfw32.lib
OBJS		=$(LOBJ)\$(TARGETBASE).obj $(LOBJ)\prefer.obj $(LOBJ)\instdata.obj \
$(LOBJ)\filter.obj $(LOBJ)\display.obj $(LOBJ)\circbuf.obj $(LOBJ)\about.obj \
$(LOBJ)\callback.obj
GOALS		=$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).sym

!if "$(DEBUG)" == "NO"
DEF		=
CDEBUG		=$(DEF)
LDEBUG		=-debug:none
!else
DEF		=-DDEBUG
CDEBUG		=-Zi $(DEF)
LDEBUG		=-debug:full -debugtype:cv
!endif

TARGETWARN	=-W3 -WX
TARGETOPT	=-Ox
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ -D_WIN32 $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS		=-r -I..\include -DWIN32 $(RDEBUG)
LFLAGS		=-nodefaultlib -align:0x1000 $(LDEBUG)
RC		=rc
CC		=cl -c -nologo
LINK		=link /nologo
MAPSYM		=mapsym -nologo
MFLAGS		=

goal:	makedirs $(GOALS)
	@echo ***** Finished making $(TARGETBASE) *****

makedirs:
!if !exists($(LOBJ))
	@md $(LOBJ) >nul
!endif

clean:
!if exists($(LOBJ))
	@echo y | del $(LOBJ) >nul
	@rd $(LOBJ) >nul
!endif
	@echo ***** Finished cleaning $(TARGETBASE) *****

$(LOBJ)\prefer.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\instdata.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\about.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\filter.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\display.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\circbuf.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\callback.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<



$(LOBJ)\$(TARGETBASE).obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\$(TARGETNAME).sym:	$$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map

$(LOBJ)\$(TARGETBASE).res: $$(@B).rc $$(@B).rcv $$(@B).h
	@$(RC) $(RCFLAGS) -fo$@ $(@B).rc

$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).map: $(OBJS) $$(@D)\$(TARGETBASE).res $$(@B).def
	@$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:i386
-subsystem:windows
-map:$(@R).map
-def:$(@B).def
$(@D)\$(TARGETBASE).res
$(LIBS)
$(OBJS)
<<
