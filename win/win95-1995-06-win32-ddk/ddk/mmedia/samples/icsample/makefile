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

TARGETBASE	=icsamp
EXT      =dll
TARGETNAME	=$(TARGETBASE)32
LOBJ		=WIN32
OBJS		=$(LOBJ)\drvproci.obj $(LOBJ)\icsample.obj
LIBS	=kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib LIBC.LIB #crtdll.lib
GOALS	=$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).sym

!if "$(DEBUG)" == "NO"
DEF	=
CDEBUG	=$(DEF)
LDEBUG=-debug:none
!else
DEF	=-DDEBUG
CDEBUG	=-Zi $(DEF)
LDEBUG=-debug:full -debugtype:cv
!endif

!if "$(VERBOSE)" == "YES"
NOLOGO	=
RCNOLOGO=-v
!else
NOLOGO	=-nologo
RCNOLOGO=
!endif

TARGETWARN	=-W3 -WX
TARGETOPT	=-Ox
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -DIS_32 -Gs -G3 -D_X86_ $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS		=-r -I..\include -D_WIN32 -DIS_32 $(RDEBUG) #RC 
LFLAGS		=-nodefaultlib -align:0x1000 $(LDEBUG)
RC		=rc $(RCNOLOGO)
CC		=cl -c $(NOLOGO)
LINK		=link $(NOLOGO)
MAPSYM		=mapsym $(NOLOGO)
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

$(LOBJ)\drvproci.obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\icsample.obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<


$(LOBJ)\$(TARGETNAME).res:	icsample.rc # icsample.rcv
	@$(RC) $(RCFLAGS) -fo$@ -I$(PVER) -I..\INCLUDE icsample.rc

$(LOBJ)\$(TARGETNAME).sym:	$$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map


$(LOBJ)\$(TARGETNAME).$(EXT): $(OBJS) $(LOBJ)\$$(@B).res $(TARGETNAME).def
	$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:IX86
-subsystem:windows,4.0
-base:0x7f840000
-map:$(@R).map
-def:$(@B).def
-dll
$(@R).res
-implib:$(@R).lib
$(OBJS)
$(LIBS)	
<<
