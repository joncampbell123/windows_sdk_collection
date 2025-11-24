#===========================================================================;
#
#   AVIEdit -- AVI Editor sample application
#
#
#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993 - 1995	Microsoft Corporation.	All Rights Reserved.
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

TARGETBASE	=aviedit
EXT		=exe
TARGETNAME	=aviedit
LOBJ		=win32
LIBS		=libc.lib user32.lib gdi32.lib kernel32.lib comdlg32.lib vfw32.lib winmm.lib shell32.lib
OBJS		=$(LOBJ)\$(TARGETBASE).obj $(LOBJ)\audio.obj
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

$(LOBJ)\$(TARGETBASE).obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\audio.obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\$(TARGETNAME).sym:	$$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map

$(LOBJ)\$(TARGETBASE).res: $$(@B).rc $$(@B).rcv $$(@B).h
	@$(RC) $(RCFLAGS) -fo$@ $(@B).rc

$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).map: $(OBJS) $$(@D)\$(TARGETBASE).res
	@$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:i386
-subsystem:windows
-map:$(@R).map
$(@D)\$(TARGETBASE).res
$(LIBS)
$(OBJS)
<<
