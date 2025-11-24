#===========================================================================;
#
#   vcr - 
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

TARGETBASE	=vcr
EXT		=mci
TARGETNAME	=$(TARGETBASE)
LOBJ		=win16
LIBS		=libw mdllcew
OBJS		=$(LOBJ)\libinit.obj $(LOBJ)\$(TARGETBASE).obj
GOALS		=$(LOBJ)\$(TARGETNAME).$(EXT)

!if "$(DEBUG)" == "NO"
DEF		=
ADEBUG		=$(DEF)
CDEBUG		=$(DEF)
LDEBUG		=
!else
DEF		=-DDEBUG
ADEBUG		=-Zi $(DEF)
CDEBUG		=-Zi $(DEF)
LDEBUG		=/CO/LI
!endif

TARGETWARN	=-W3 -WX
TARGETOPT	=-Ox
AFLAGS		=-Cx -W2 -Zd -D?MEDIUM -D?QUIET -DMASM6 $(ADEBUG) -Fo$@
CFLAGS		=$(TARGETWARN) $(TARGETOPT) -Z7dp -ASw -Gs -GD3 -DSTRICT $(CDEBUG) -I..\include -Fo$@
RCFLAGS		=-z -r -I..\include -I. $(RDEBUG) -Fo$@
LFLAGS		=/L/MAP/NOPACKCODE/NOE/NOD/ONERROR:NOEXE/ALIGN:16$(LDEBUG)
RESFLAGS	=-t
RC		=rc -nologo
ASM		=ml -c -nologo
CC		=cl -c -nologo
LINK		=link 

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

$(LOBJ)\libinit.obj:		$$(@B).asm
	@$(ASM) $(AFLAGS) -DSEGNAME=_TEXT %s

$(LOBJ)\$(TARGETBASE).obj:	$$(@B).c
	@$(CC) @<<
$(CFLAGS) -NT _TEXT
$(@B).c
<<

$(LOBJ)\$(TARGETBASE)s.res: $(TARGETBASE).rc $(TARGETBASE).rcv $$(@B).rc
	@$(RC) $(RCFLAGS) $(@B).rc

$(LOBJ)\$(TARGETNAME).$(EXT): $(OBJS) $$(@D)\$(TARGETBASE)s.res $$(@B).def
	@$(LINK) $(LFLAGS) @<<
$(OBJS),
$(@R).$(EXT),
$(@R).map,
$(LIBS),
$(@B).def
<<
	@$(RC) $(RESFLAGS) $(@D)\$(TARGETBASE)s.res $*.$(EXT)
	@mapsym $*.map
