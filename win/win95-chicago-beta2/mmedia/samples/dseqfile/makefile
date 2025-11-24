#===========================================================================;
#
#   dseqfile - DIB Sequence File Handler
#
#
#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1992 - 1995	Microsoft Corporation.	All Rights Reserved.
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

TARGETBASE	=dseqf
EXT		=dll
TARGETNAME	=$(TARGETBASE)32
LOBJ		=win32
LIBS		=libc.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib \
			winmm.lib shell32.lib ole32.lib uuid.lib
OBJS		=$(LOBJ)\factory.obj $(LOBJ)\$(TARGETBASE).obj
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
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ -DWIN32 -DSTRICT $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS		=-r -I. -I..\include -DWIN32 $(RDEBUG) -Fo$@
LFLAGS		=-nodefaultlib -align:0x1000 $(LDEBUG) -DLL
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

$(LOBJ)\factory.obj:	$$(@B).cpp handler.h
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\$(TARGETBASE).obj:	$$(@B).cpp handler.h
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\junk.obj:	$$(@B).cpp
	@echo $(CC) $(CFLAGS) $(@B).cpp
	@$(CC) @<<
$(CFLAGS)
$(@B).cpp
<<

$(LOBJ)\$(TARGETBASE).res: handler.rc handler.rcv
	@$(RC) $(RCFLAGS) handler.rc

$(LOBJ)\$(TARGETNAME).$(EXT): $(OBJS) $$(@D)\$(TARGETBASE).res $$(@B).def
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
