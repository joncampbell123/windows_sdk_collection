#===========================================================================;
#
#   capcpp
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

TARGETBASE	=capcpp
EXT		=exe
TARGETNAME	=$(TARGETBASE)32
LIBS		=libcmt.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib \
			winmm.lib shell32.lib ole32.lib msacm32.lib \
			vfw32.lib advapi32.lib
LOBJ		=win32
OBJS		=$(LOBJ)\capparms.obj $(LOBJ)\captedoc.obj $(LOBJ)\captest.obj \
		$(LOBJ)\captevw.obj $(LOBJ)\mainfrm.obj $(LOBJ)\stdafx.obj

GOALS		=$(LOBJ)\$(TARGETNAME).$(EXT)

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
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -MT -Gs -D_X86_ -DWIN32 -I..\include -DSTRICT $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS		=-r -I..\include  -DWIN32 $(RDEBUG)
LFLAGS		=-align:0x1000 $(LDEBUG)
RC		=rc
CC		=cl -c -nologo
LINK		=link 
MAPSYM		=mapsym
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

$(LOBJ)\capparms.obj:	$$(@B).cpp
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\captedoc.obj:	$$(@B).cpp
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\captest.obj:	$$(@B).cpp
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\captevw.obj:	$$(@B).cpp
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\mainfrm.obj:	$$(@B).cpp
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\stdafx.obj:	$$(@B).cpp
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<


$(LOBJ)\$(TARGETBASE).res: $(TARGETBASE).rc $(TARGETBASE).rcv
	@$(RC) $(RCFLAGS) -fo$@ -I. $(TARGETBASE).rc

$(LOBJ)\$(TARGETNAME).$(EXT): $(OBJS) $$(@D)\$(TARGETBASE).res
	@$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:i386
-subsystem:windows
-map:$(@R).map
$(@D)\$(TARGETBASE).res
$(LIBS)
$(OBJS)
<<
