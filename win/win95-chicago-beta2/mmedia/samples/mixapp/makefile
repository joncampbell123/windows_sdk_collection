#===========================================================================;
#
#   Mix Application
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
#       nmake DEBUG=OxZi will build with full optimization and debugging symbols
#
#   You can also set the environment variable DEBUG to NO or OxZi
#
#===========================================================================;

TARGETBASE	=mixapp
EXT		=exe
TARGETNAME	=$(TARGETBASE)32
LOBJ		=win32
LIBS		=libc.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib winmm.lib vfw32.lib advapi32.lib
OBJS		=$(OBJ1) $(OBJ2)
OBJ1		=$(LOBJ)\$(TARGETBASE).obj $(LOBJ)\mainit.obj $(LOBJ)\malines.obj \
		$(LOBJ)\mactrls.obj $(LOBJ)\macustom.obj $(LOBJ)\mameter.obj \
		$(LOBJ)\maswitch.obj
OBJ2		=$(LOBJ)\manumber.obj $(LOBJ)\maslider.obj $(LOBJ)\mafader.obj \
		$(LOBJ)\matime.obj $(LOBJ)\malist.obj $(LOBJ)\tlb.obj \
		$(LOBJ)\debug.obj
GOALS		=$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).sym

!if "$(DEBUG)" == "NO"
TARGETOPT	=-Ox
DEF		=
CDEBUG		=$(DEF)
LDEBUG		=-debug:none
!else if "$(DEBUG)" == "OxZi"
TARGETOPT	=-Ox
DEF		=-DDEBUG
CDEBUG		=-Zi $(DEF)
LDEBUG		=-debug:full -debugtype:cv
!else
TARGETOPT	=-Od
DEF		=-DDEBUG
CDEBUG		=-Zi $(DEF)
LDEBUG		=-debug:full -debugtype:cv
!endif

TARGETWARN	=-W3 -WX
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ -DWIN32 $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS		=-r -I..\include -I..\verinfo -DWIN32 $(RDEBUG)
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

$(LOBJ)\mainit.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\malines.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\mactrls.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\macustom.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\mameter.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\maswitch.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\manumber.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\malist.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<


$(LOBJ)\maslider.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\debug.obj:	$$(@B).c $$(@B).h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\matime.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\mafader.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\tlb.obj:	$$(@B).c $(TARGETBASE).h appport.h
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<


$(LOBJ)\$(TARGETBASE).obj:	$$(@B).c $$(@B).h appport.h
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
