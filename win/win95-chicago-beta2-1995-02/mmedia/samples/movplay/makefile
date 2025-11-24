#===========================================================================;
#
#   MOVPLAY -- Movie Player Application
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

TARGETBASE	=movplay
EXT		=exe
TARGETNAME1	=$(TARGETBASE)1
TARGETNAME2	=$(TARGETBASE)2
LOBJ		=win32
LIBS		=libc.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib winmm.lib vfw32.lib
OBJS		=$(LOBJ)\$(TARGETNAME1).obj $(LOBJ)\$(TARGETNAME2).obj
GOALS		=$(LOBJ)\$(TARGETNAME1).$(EXT) $(LOBJ)\$(TARGETNAME1).sym $(LOBJ)\$(TARGETNAME2).$(EXT) $(LOBJ)\$(TARGETNAME2).sym

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
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ -DWIN32 $(CDEBUG) -I..\include -Fd$* -Fo$@
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

$(LOBJ)\$(TARGETNAME1).obj:	$$(@B).c $(TARGETBASE).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\$(TARGETNAME2).obj:	$$(@B).c $(TARGETBASE).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\$(TARGETNAME1).sym:	$$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map

$(LOBJ)\$(TARGETNAME2).sym:	$$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map

$(LOBJ)\$(TARGETNAME1).res: $(TARGETBASE).rc $(TARGETBASE).rcv $(TARGETBASE).h
	@$(RC) $(RCFLAGS) -fo$@ $(TARGETBASE).rc

$(LOBJ)\$(TARGETNAME2).res: $(TARGETBASE).rc $(TARGETBASE).rcv $(TARGETBASE).h
	@$(RC) $(RCFLAGS) -fo$@ $(TARGETBASE).rc

$(LOBJ)\$(TARGETNAME1).$(EXT) $(LOBJ)\$(TARGETNAME1).map: $(OBJS) $$(@D)\$(TARGETNAME1).res $$(@B).def
	@$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:i386
-subsystem:windows
-map:$(@R).map
-def:$(@B).def
$(@D)\$(TARGETNAME1).res
$(LIBS)
$(LOBJ)\$(TARGETNAME1)
<<

$(LOBJ)\$(TARGETNAME2).$(EXT) $(LOBJ)\$(TARGETNAME2).map: $(OBJS) $$(@D)\$(TARGETNAME2).res $$(@B).def
	@$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:i386
-subsystem:windows
-map:$(@R).map
-def:$(@B).def
$(@D)\$(TARGETNAME2).res
$(LIBS)
$(LOBJ)\$(TARGETNAME2)
<<
