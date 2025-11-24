#===========================================================================;
#
#   IDF file editor  
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
#
#   to make a NON-DEBUG build, type the following line:
#       nmake DEBUG=NO
#   just typing 'nmake' will build a DEBUG build
#
#   You can also set the environment variable DEBUG to NO
#
#===========================================================================;

TARGETBASE	=idfedt
EXT		=exe
TARGETNAME	=$(TARGETBASE)32
LOBJ		=win32
LIBS		=libc.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib winmm.lib vfw32.lib advapi32.lib
OBJS		=$(LOBJ)\main.obj $(LOBJ)\about.obj $(LOBJ)\edit.obj \
	 	 $(LOBJ)\globals.obj $(LOBJ)\file.obj $(LOBJ)\proc.obj
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
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS		=-r -I..\include $(RDEBUG)
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

$(LOBJ)\main.obj:	$$(@B).c about.h globals.h idfd.h preclude.h res.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\about.obj:	$$(@B).c about.h globals.h idfd.h preclude.h res.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\edit.obj:	$$(@B).c about.h globals.h idfd.h preclude.h res.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\globals.obj:	$$(@B).c about.h globals.h idfd.h preclude.h res.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\file.obj:	$$(@B).c about.h globals.h idfd.h preclude.h res.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\proc.obj:	$$(@B).c about.h globals.h idfd.h preclude.h res.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\$(TARGETNAME).sym:	$$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map

$(LOBJ)\$(TARGETBASE).res: $$(@B).rc $$(@B).rcv
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

findit:
    \bin\where $$include:ver.h
