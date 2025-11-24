#===========================================================================;
#
#   MidiPlyr
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
#===========================================================================;

TARGETBASE      =midiplyr
EXT             =exe
TARGETNAME      =midipl32
LOBJ            =win32
LIBS            =libc.lib user32.lib kernel32.lib gdi32.lib comdlg32.lib winmm.lib comctl32.lib shell32.lib
OBJ1            =$(LOBJ)\$(TARGETBASE).obj $(LOBJ)\mainwnd.obj $(LOBJ)\timewnd.obj
OBJ2            =$(LOBJ)\uiutils.obj $(LOBJ)\sequence.obj $(LOBJ)\smf.obj 
OBJ3            =$(LOBJ)\smfread.obj $(LOBJ)\debug.obj

OBJS            =$(OBJ1) $(OBJ2) $(OBJ3)
GOALS           =$(LOBJ)\$(TARGETNAME).$(EXT)

!if "$(DEBUG)" == "NO"
DEF             =
CDEBUG          =$(DEF)
TARGETOPT       =-Ox
LDEBUG          =-debug:none
!else
DEF             =-DDEBUG
CDEBUG          =-Zi $(DEF)
TARGETOPT       =-Od
LDEBUG          =-debug:full -debugtype:cv
!endif

TARGETWARN      =-W3 -WX
CFLAGS          =$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ -DWIN32 $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS         =-r -I..\include -I..\verinfo -DWIN32 $(RDEBUG)
LFLAGS          =-nodefaultlib -align:0x1000 $(LDEBUG)
RC              =rc
CC              =cl -c -nologo
LINK            =link /nologo
MAPSYM          =mapsym -nologo
MFLAGS          =

goal:   makedirs $(GOALS)
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

$(LOBJ)\$(TARGETBASE).obj:      $$(@B).c $$(@B).h 
        @$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\mainwnd.obj:            mainwnd.c
$(LOBJ)\timewnd.obj:            timewnd.c
$(LOBJ)\uiutils.obj:            uiutils.c
$(LOBJ)\sequence.obj:           sequence.c
$(LOBJ)\smf.obj:                smf.c
$(LOBJ)\smfread.obj:            smfread.c
$(LOBJ)\debug.obj:              debug.c

{}.c{$(LOBJ)}.obj:
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\$(TARGETNAME).sym:      $$(@R).map
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
