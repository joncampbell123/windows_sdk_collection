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

TARGETBASE	=msrle
EXT		=dll
TARGETNAME	=$(TARGETBASE)32
LOBJ		=WIN32
OBJS		=$(LOBJ)\drvproc.obj $(LOBJ)\msrle.obj $(LOBJ)\rle.obj \
		$(LOBJ)\rlec.obj
objBOGUS=		$(LOBJ)\libinit.obj $(LOBJ)\df.obj \
		$(LOBJ)\rlea.obj
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

TARGETWARN	=-W3 -WX
TARGETOPT	=-Ox
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -Gs -D_X86_ -DWIN32 -DIS_32 $(CDEBUG) -I..\include -Fd$* -Fo$@
RCFLAGS		=-r -I..\include -DWIN32 -DIS_32 $(RDEBUG)
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

$(LOBJ)\libinit.obj $(LOBJ)\df.obj $(LOBJ)\rlea.obj:	libinit.asm  df.asm rlea.asm
	@echo $(@B).asm
	@$(ASM) $(AFLAGS) -DSEGNAME=MSRLEC ..\..\$(@B),$@;

$(LOBJ)\drvproc.obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\rlec.obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\rle.obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\msrle.obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS)
$(@B).c
<<

$(LOBJ)\$(TARGETNAME).res:	$(TARGETBASE).rc $(TARGETBASE).rcv
	@$(RC) $(RCFLAGS) -fo$@ -I$(PVER) -I..\INCLUDE $(TARGETBASE).rc

$(LOBJ)\$(TARGETNAME).sym:	$$(@R).map
	@$(MAPSYM) $(MFLAGS) -mo $@ $*.map


$(LOBJ)\$(TARGETNAME).lib $(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).map: \
	$(OBJS) $(LOBJ)\$$(@B).res $(TARGETNAME).def
	$(LINK) $(LFLAGS) @<<
-out:$(@R).$(EXT)
-machine:i386
-subsystem:windows,4.0
-base:0x7f840000
-map:$(@R).map
-def:$(@B).def
-dll
-entry:DLLEntryPoint@12
-implib:$(@R).lib
$(@R).res
$(OBJS)
$(LIBS)
<<
